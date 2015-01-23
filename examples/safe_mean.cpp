#include "vta/algorithms.hpp"

#include <limits>
#include <iostream>
#include <type_traits>

template <typename T, int Size>
struct running {
	T total;
	T remainder;
};

template <typename T, int N>
constexpr running<T, N> safe_mean_foldl(running<T, N> r, T t) noexcept {
	return running<T, N>{(r.total + t / N) + (r.remainder + t % N) / N,
	                     (r.remainder + t % N) % N};
}

template <typename Arg,
          typename... Args,
          int N = sizeof...(Args) + 1,
          typename std::enable_if<vta::are_same_after<std::decay, Arg, Args...>::value
                               && std::is_arithmetic<Arg>::value>::type* = nullptr>
constexpr Arg safe_mean(Arg arg, Args... args) noexcept {
	return vta::add_const(vta::foldl(&safe_mean_foldl<Arg, N>))
	  (running<Arg, N>{0, 0}, arg, args...).total;
}

template <typename Arg, typename... Args>
void print_mean(Arg first, Args... args) {
	std::cout << "Arithmetic mean of " << first;
	vta::map([](auto x){ std::cout << ", " << x; })(args...);
	std::cout << " is ";
	std::cout << safe_mean(first, args...);
	std::cout << std::endl;
}

template <typename T>
constexpr T sum(T lhs, T rhs) noexcept {
	return lhs + rhs;
}

template <typename Arg,
          typename... Args,
          int N = sizeof...(Args) + 1,
          typename std::enable_if<vta::are_same_after<std::decay, Arg, Args...>::value
                               && std::is_arithmetic<Arg>::value>::type* = nullptr>
constexpr Arg unsafe_mean(Arg arg, Args... args) noexcept {
	return vta::add_const(vta::foldl(&sum<Arg>))(arg, args...) / N;
}

int main()
{
	print_mean(1);
	print_mean(1, 2);
	print_mean(-7, -8);
	print_mean(1, 2, 3);
	print_mean(-1, 15, 2, -3, 0, 0, 2, 3);

	static_assert(safe_mean(1, 2) == 1, "");
	static_assert(safe_mean(std::numeric_limits<int>::max(), std::numeric_limits<int>::max() - 2)
	  == std::numeric_limits<int>::max() - 1, "");

	static_assert(unsafe_mean(1, 2) == 1, "");
	// following line may not compile due to integer overflow
	//static_assert(unsafe_mean(std::numeric_limits<int>::max(), std::numeric_limits<int>::max() - 2)
	  //== std::numeric_limits<int>::max() - 1, "");
}
