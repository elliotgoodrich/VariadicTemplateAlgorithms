Variadic Template Algorithms
============================

[![Build Status](https://travis-ci.org/elliotgoodrich/VariadicTemplateAlgorithms.svg?branch=master)](https://travis-ci.org/elliotgoodrich/VariadicTemplateAlgorithms)

Working with parameter packs in C++ can be difficult. Getting a reference to the last argument of a parameter pack or reversing the order of arguments to a function is much harder than it should be. VTA is a header-only C++14 library to make this easier.

Grabbing the nth parameter is very simple with `vta::head`, `vta::at`, and `vta::last`:

```cpp
auto i = vta::head(1, "hello"s, 3.14, '!');   // i is an int with value 1
auto c = vta::last(1, "hello"s, 3.14, '!');   // l is a char with value '!'
auto s = vta::at<1>(1, "hello"s, 3.14, '!');  // s is a std::string with value "hello"

// vta::at can take negative values to index backward from the end
auto d = vta::at<-2>(1, "hello"s, 3.14, '!'); // d is a double with value 3.14
```

There are many functions which return functors that apply a function across any number of arguments. Writing a max function is simple when you can [fold](http://en.wikipedia.org/wiki/Fold_%28higher-order_function%29) across any number of parameters,

```cpp
template <typename... Args>
auto max(Args... args) {
	// Check that all of the arguments are of the same type
	static_assert(vta::are_same<typename std::decay<Args...>::type>::value,
	  "Arguments to max must have the same type");

	// Create a max lambda that can operate on any type
	// (note this line is not needed if the VTA_FN_TO_FUNCTOR macro is used)
	auto max = [](auto const& l, auto const& r){ return std::max(l, r); };

	// Perform a left fold across the arguments, returning the maximum
	return vta::foldl(max)(args...);
}
```

Reversing arguments and forwarding them on is also easy,

```cpp
// Prints an argument
auto printer = [](auto const& x){ std::cout << x; };

// Prints all arguments in order
template <typename... Args>
void print(Args... args) {
	// vta::map applies a function to each argument in order
	vta::map(printer)(args...);
}

// Prints all arguments in reverse order
template <typename... Args>
void reverse_print(Args... args) {
	// vta::forward_after forwards the arguments to a function after a transformation
	vta::forward_after<vta::reverse>(vta::map(printer))(args...);
}
```

The `vta::forward_after` function can perform many other transformations such as swapping arguments or shifting all arguments to the left. The full list of transformation can be found in the [api reference](API_REFERENCE.md#transformations).
