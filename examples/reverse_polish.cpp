#include "vta/algorithms.hpp"

#include <stdexcept>
#include <iostream>
#include <type_traits>

struct start_type {};
static constexpr start_type start{};

struct add_type {};
static constexpr add_type add{};

struct minus_type {};
static constexpr minus_type minus{};

struct mult_type {};
static constexpr mult_type mult{};

struct div_type {};
static constexpr div_type divide{};

struct fact_type {};
static constexpr fact_type fact{};

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>* = nullptr>
struct polish_calculator {
	constexpr T operator()(std::pair<T, T> stack, add_type) noexcept {
		return stack.first + stack.second;
	}

	constexpr T operator()(std::pair<T, T> stack, minus_type) noexcept {
		return stack.first - stack.second;
	}

	constexpr T operator()(std::pair<T, T> stack, mult_type) noexcept {
		return stack.first * stack.second;
	}

	constexpr T operator()(std::pair<T, T> stack, div_type) noexcept {
		return stack.first / stack.second;
	}

	// Only allow factorial if the type is integral (e.g. disable for double)
	// This function can only throw if T is signed, so mark as noexcept otherwise
	constexpr auto operator()(T stack, fact_type) noexcept(std::is_unsigned<T>::value)
	  -> typename std::enable_if<std::is_integral<T>::value, T>::type {
		return (stack < 0) ? throw std::runtime_error{"Negative value passed to factorial"}
		                   : (stack == 0) ? 1 : stack * (*this)(stack - 1, fact);
	}

	constexpr std::pair<T, T> operator()(T first, T second) noexcept {
		return {first, second};
	}

	constexpr T operator()(start_type, T second) noexcept {
		return second;
	}
};

// This function can only throw if call factorial on a signed value, so mark as nothrow if we
// are operating on an unsigned type or fact_type is not in Args...
template <typename T, typename... Args>
constexpr T rev_polish_calc(Args... args)
  noexcept(std::is_unsigned<T>::value || vta::are_unique<fact_type, Args...>::value) {
	return vta::add_const(vta::foldl(polish_calculator<T>{}))(start, args...);
}

int main()
{
	std::cout << "2 3 + = " << rev_polish_calc<int>(2, 3, add) << std::endl;
	std::cout << "1 4 * = " << rev_polish_calc<int>(1, 4, mult) << std::endl;
	std::cout << "5 2 - 7 * = " << rev_polish_calc<int>(5, 2, minus, 5, mult) << std::endl;
	std::cout << "15 4 / 1 + = " << rev_polish_calc<int>(15, 4, divide, 1, add) << std::endl;

	static_assert(rev_polish_calc<int>(3, fact, 2, minus, 8, mult, 2, add) == 34,
	  "3 ! 2 - 8 * 2 + should equal 34");
}
