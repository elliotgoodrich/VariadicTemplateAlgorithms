#include "vta/algorithms.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <type_traits>
#include <string>

namespace {

struct printer {
	printer(std::ostream& str)
	: m_str{&str} {
	}

	template <typename T>
	void operator()(T const& obj) {
		*m_str << obj;
	}

	std::ostream* m_str;
};

struct string_concat {
	template <typename LHS, typename RHS>
	std::string operator()(LHS const& lhs, RHS const& rhs) const {
		std::stringstream ss;
		ss << lhs << rhs;
		return ss.str();
	}
};

struct is_positive_int {
	bool operator()(int x) const {
		return x > 0;
	}

	template <typename T>
	bool operator()(T const&) const {
		return false;
	}
};

bool is_true(bool p) {
	return p;
}

struct adjacent_printer {
	adjacent_printer(std::string& str)
	: m_str{&str} {
	}

	void operator()(int i, int j) {
		std::stringstream ss;
		ss << i * j;
		*m_str += ss.str();
	}

	std::string* m_str;
};

}

BOOST_AUTO_TEST_SUITE(algorithms)

BOOST_AUTO_TEST_CASE(is_homogenous) {
	static_assert(vta::is_homogenous<>::value, "");
	static_assert(vta::is_homogenous<int>::value, "");
	static_assert(vta::is_homogenous<int, int>::value, "");
	static_assert(vta::is_homogenous<int, int, int>::value, "");
	static_assert(!vta::is_homogenous<int, float>::value, "");
	static_assert(!vta::is_homogenous<int&, int>::value, "");
	static_assert(!vta::is_homogenous<int const&, int&>::value, "");
	static_assert(!vta::is_homogenous<int&&, int&>::value, "");
	static_assert(!vta::is_homogenous<const int, int>::value, "");
	static_assert(!vta::is_homogenous<volatile int, int>::value, "");
}

BOOST_AUTO_TEST_CASE(is_homogenous_after) {
	static_assert(vta::is_homogenous_after<std::remove_reference, int&&, int&>::value, "");
}

BOOST_AUTO_TEST_CASE(is_unique) {
	static_assert(vta::is_unique<>::value, "");
	static_assert(vta::is_unique<1>::value, "");
	static_assert(vta::is_unique<1, 2>::value, "");
	static_assert(!vta::is_unique<1, 1>::value, "");
	static_assert(vta::is_unique<1, -1>::value, "");
	static_assert(vta::is_unique<1, 2, 3, 4, 5>::value, "");
	static_assert(!vta::is_unique<1, 2, 3, 1, 5>::value, "");
	static_assert(!vta::is_unique<2, 3, 4, 5, 6, 5>::value, "");
}

BOOST_AUTO_TEST_CASE(head) {
	auto f = [](){
		return vta::head(0);
	};
	static_assert(std::is_same<int, decltype(f())>::value, "");
	BOOST_CHECK_EQUAL(f(), 0);

	auto g = [](){
		return vta::head(3l, 1u);
	};
	static_assert(std::is_same<long, decltype(g())>::value, "");
	BOOST_CHECK_EQUAL(g(), 3l);

	auto h = [](){
		return vta::head(std::string{}, 0, 3u);
	};
	static_assert(std::is_same<std::string, decltype(h())>::value, "");
	BOOST_CHECK_EQUAL(h(), std::string{});
}

/**************************************************************************************************
 * Transformations                                                                                *
 **************************************************************************************************/

BOOST_AUTO_TEST_CASE(flip) {
	{
		std::stringstream ss;
		vta::forward_after<vta::flip>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "2134.5six");
	}
}

BOOST_AUTO_TEST_CASE(left_shift) {
	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift<4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six1234.5");
	}
}

BOOST_AUTO_TEST_CASE(right_shift) {
	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six1234.5");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift<4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six1");
	}
}

BOOST_AUTO_TEST_CASE(shift) {
	{
		std::stringstream ss;
		vta::forward_after<vta::shift<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift<-2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "4.5six123");
	}
}

BOOST_AUTO_TEST_CASE(left_shift_tail) {
	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "134.5six2");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::left_shift_tail<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "14.5six23");
	}
}

BOOST_AUTO_TEST_CASE(right_shift_tail) {
	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1six234.5");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::right_shift_tail<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "14.5six23");
	}
}

BOOST_AUTO_TEST_CASE(shift_tail) {
	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<0>>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "134.5six2");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::shift_tail<-1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1six234.5");
	}
}

BOOST_AUTO_TEST_CASE(drop) {
	{
		std::stringstream ss;
		vta::forward_after<vta::drop<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "34.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::drop<5>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "");
	}
}

BOOST_AUTO_TEST_CASE(take) {
	{
		std::stringstream ss;
		vta::forward_after<vta::take<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "12");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::take<4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5");
	}
}

BOOST_AUTO_TEST_CASE(cycle) {
	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<-3>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "1234.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "2134.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 1, 2>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "3124.5six");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<-1, 0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six234.51");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 2, 4>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six214.53");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<4, 1, 2, 0>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "3six24.51");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::cycle<0, 4, 2, 3, 1>>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "24.5six31");
	}
}

BOOST_AUTO_TEST_CASE(reverse) {
	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))();
		BOOST_CHECK_EQUAL(ss.str(), "");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))(1);
		BOOST_CHECK_EQUAL(ss.str(), "1");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))(1, '2');
		BOOST_CHECK_EQUAL(ss.str(), "21");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::reverse>(vta::map(printer{ss}))(1, '2', 3u, 4.5, "six");
		BOOST_CHECK_EQUAL(ss.str(), "six4.5321");
	}
}

// Functions

BOOST_AUTO_TEST_CASE(map) {
	std::stringstream ss;
	vta::map(printer{ss})(1, 2, 3u, ' ', 4.5, " hello", " world!");
	BOOST_CHECK_EQUAL(ss.str(), "123 4.5 hello world!");
}

BOOST_AUTO_TEST_CASE(adjacent_map) {
	std::string s;
	adjacent_printer printer{s};
	vta::adjacent_map<2>(std::ref(printer))(1, 2, 3, 4, 5, 6);
	BOOST_CHECK_EQUAL(s, "26122030");
}

BOOST_AUTO_TEST_CASE(foldl) {
	auto s = string_concat{};
	auto const result = vta::foldl(s)(1, 2, 3);
	BOOST_CHECK_EQUAL(result, "123");
}

BOOST_AUTO_TEST_CASE(foldr) {
	//auto s = string_concat{};
	//auto const result = vta::foldr(s)(1, 2, 3);
	//BOOST_CHECK_EQUAL(result, "123");
}

BOOST_AUTO_TEST_CASE(compose) {
	{
		std::stringstream ss;
		vta::forward_after<vta::compose<vta::left_shift<1>>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "12340");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::compose<vta::left_shift<1>, vta::flip>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "21340");
	}
}

BOOST_AUTO_TEST_CASE(swap) {
	{
		std::stringstream ss;
		vta::forward_after<vta::swap<2, 4>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01432");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<2, -1>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01432");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<0, 2>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "21034");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<1, 1>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01234");
	}

	{
		std::stringstream ss;
		vta::forward_after<vta::swap<2, -3>>(vta::map(printer{ss}))(0, 1, 2, 3, 4);
		BOOST_CHECK_EQUAL(ss.str(), "01234");
	}
}

BOOST_AUTO_TEST_CASE(all_of) {
	auto const all_positive_int = vta::all_of(is_positive_int{});
	BOOST_CHECK_EQUAL(all_positive_int(1, 2), true);
	BOOST_CHECK_EQUAL(all_positive_int("hello", 2), false);
	BOOST_CHECK_EQUAL(all_positive_int(0u, 2), false);
	BOOST_CHECK_EQUAL(all_positive_int(-2), false);
	BOOST_CHECK_EQUAL(all_positive_int(), true);

	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(), true);
	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(true), true);
	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(false), false);
	BOOST_CHECK_EQUAL(vta::all_of(&is_true)(true, false), false);
}

BOOST_AUTO_TEST_CASE(any_of) {
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(1, 2), true);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})("hello", 2), true);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(0u, 2), true);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(-2), false);
	BOOST_CHECK_EQUAL(vta::any_of(is_positive_int{})(), false);
}

BOOST_AUTO_TEST_CASE(none_of) {
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(1, 2), false);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})("hello", 2), false);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(0u, 2), false);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(-2), true);
	BOOST_CHECK_EQUAL(vta::none_of(is_positive_int{})(), true);
}

BOOST_AUTO_TEST_SUITE_END()
