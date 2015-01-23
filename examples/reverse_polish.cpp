#include "vta/algorithms.hpp"

#include <iostream>

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

template <typename T>
struct polish_calculator {
	constexpr T operator()(std::pair<T, T> stack, add_type) {
		return stack.first + stack.second;
	}

	constexpr T operator()(std::pair<T, T> stack, minus_type) {
		return stack.first - stack.second;
	}

	constexpr T operator()(std::pair<T, T> stack, mult_type) {
		return stack.first * stack.second;
	}

	constexpr T operator()(std::pair<T, T> stack, div_type) {
		return stack.first / stack.second;
	}

	constexpr T operator()(T stack, fact_type) {
		return (stack == 0) ? 1 : stack * (*this)(stack - 1, fact);
	}

	constexpr std::pair<T, T> operator()(T first, T second) {
		return {first, second};
	}

	constexpr T operator()(start_type, T second) {
		return second;
	}
};

template <typename T, typename... Args>
constexpr T rev_polish_calc(Args... args) {
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
