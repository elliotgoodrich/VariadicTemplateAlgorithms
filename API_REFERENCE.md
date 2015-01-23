VTA API Reference
=================

 * [Predicates](#predicate)
 * [Miscellaneous Functions](#misc)
 * [Type aliases](#alias)
 * [Variadic Functors](#functor)
 * [Tranformations](#transformation)
 * [Macros](#macro)

<a name="predicate"></a>Predicates
----------

VTA contains similar tools to those found in `<type_traits>` to decide whether one or more types have a certain property.

---
#### `are_same`
    template <typename... Args>
    struct are_same {
        static bool const value;
    };

This struct is an extension of `std::is_same` that operates on multiple types. `value` is `true` only if all of the types `Args...` are the same, `false` otherwise. If there are 0 or 1 types, then `value` is always `true`.

##### example

    static_assert(vta::are_same<float, float, float, float>::value, "");
    static_assert(!vta::are_same<int, int, double>::value, "");

---
#### `are_same_after`
    template <typename TypeTransformation, typename... Args>
    struct are_same_after {
        static bool const value
          = vta::are_same<typename TypeTransformation<Args>::type...>::value;
    };

This struct is a shorthand way to apply a type transformation to all of `Args...` and the variable `value` is the result of whether these are all the same type or not.

##### example

    static_assert(vta::are_same_after<std::decay, int, int&, const int&>::value, "");

---
#### `are_unique`
    template <typename... Args>
    struct are_unique{
        static bool const value;
    };

`value` is true if and only if all `Args...` are unique types

##### example

    static_assert(vta::are_unique<int, float, double>::value, "");
    static_assert(!vta::are_unique<int, int, double>::value, "");

---
#### `are_unique_after`
    template <typename TypeTransformation, typename... Args>
    struct are_unique_after{
        static bool const value
          = vta::are_unique<typename TypeTransformation<Args>::type...>::value;
    };

This struct is a shorthand way to apply a type transformation to all of `Args...` and the variable `value` is the result of whether all of these are unique.

##### example

    static_assert(!vta::are_unique_after<std::remove_reference, int, int&, int&&>::value, "");

---
#### `are_unique_ints`
    template <int... Ns>
    struct are_unique_ints {
        static bool const value;
    };

`value` is true if no `int` that appears in `Ns...` appears more than once.

##### examples

    static_assert(vta::are_unique_ints<1, 2, 3>::value, "");
    static_assert(!vta::are_unique_ints<1, 2, 3, 2>::value, "");


<a name="misc"></a>Miscellaneous Functions
---------
Some of the signatures of functions and classes are not 100% correct C++, but they are written this way for clarity (for example `vta::last` and `vta::at`).

---
#### `head`
    template <typename Head, typename... Args>
    constexpr Head&& head(Head&&, Args&&...) noexcept;

Returns a reference to the first argument with the same value category it had when it was passed in to `head`.

##### examples

    std::string s = vta::head("hello", 1, '2');       // s = "hello"
    std::string t = vta::head(std::move(s), "world"); // s is moved into t

---
#### `last`
    template <typename... Args, typename Last>
    constexpr Last&& head(Args&&..., Last&&) noexcept;

Returns a reference to the last argument with the same value category it had when it was passed in to `last`.

##### examples

    int i = vta::last('1', 2); // i = 2

---
#### `at`
    template <int N, typename... Before, typename Arg, typename... After>
    constexpr Arg&& at(Before&&..., Arg&&, After&&...) noexcept;

Returns a reference to the `N`-th argument with the same value category it had when it was passed in to `at`. Negative values of `N` are allowed and these count backwards from the last argument. e.g. `at<-1>` returns the last parameter and `at<-2>` returns the penultimate parameter.

##### examples

    int i = vta::at<1>('1', 2, "3");          // i = 2
    std::string s = vta::at<-1>('1', 2, "3"); // s = "3"
    char c = vta::at<-3>('1', 2, "3");        // c = '1'

---
#### `add_const`
    template <typename T>
    constexpr T const& add_const(T&&) noexcept;

When dealing with variadic functors created by this library, the `const` overload of `operator()` is `constexpr` and the non-`const` is not. In `constexpr` functions it is necessary to use the `const` overload, `add_const` is very useful in these situations.

##### examples

    auto sum = [](auto l, auto r){ return l + r; };

    template <typename Args...>
    constexpr auto sum(Args... args) {
        return vta::add_const(vta::foldl(sum))(0, args...);
    }

<a name="alias"></a>Type aliases
------------

#### `head_t`

    template <typename... Args>
    using head_t = decltype(head(std::declval<Args>()...));

`head_t` is a type alias for the first type in `Args...`.

---
#### `last_t`
    template <typename... Args>
    using last_t = decltype(last(std::declval<Args>()...));

`last_t` is a type alias for the last type in `Args...`.

---
#### `at_t`
    template <int N>
    struct at_t {
        template <typename... Args>
        using type = decltype(at<N>(std::declval<Args>()...));
    };

`at_t<N>::type` is a type alias for the `N`th type in `Args...`.

<a name="functor"></a>Variadic Functor functions
--------------------------

The following functions in the Variadic Template Algorithm library create a variadic functor. These functors have an unspecified type, but have the following member function signatures.

    struct /*unspecified*/ {
        template <typename... Args>
        constexpr /*depends*/ operator()(Args&&... args) const;

        template <typename... Args>
        /*depends*/ operator()(Args&&... args);
    };

The return type of `operator()` differs and will be specified for each function.

Like the standard library, all functions in VTA may copy functors that are passed to it an unspecified number of times. If you want to have reference semantics for functors, use `std::ref` and `std::cref` in `<functional>`. No copies of other parameters passed are ever copied, only references are passed around.

For example:

    // functor can be copied any number of times, whereas arg1, arg2, ... never
    // have copies made and are only ever forwarded
    vta::map(functor)(arg1, arg2, arg3, arg4);

When discussed, the index of parameters always begins at 0.

More than std::numeric_limits<int>::max() parameters may not be passed to any variadic functor (though this situation is unlikely to ever happen naturally, and I'm sure would struggle to compile regardless).

The variadic functors created are copyable/movable if the function they are created with is copyable/movable.

---
#### `map`
    template <typename Function>
    constexpr /*VariadicFunctor*/ map(Function&& f);

 `map` returns a functor that, when applied to any number of parameters, applies the function `f` to each parameter in order. This functor always returns `void`.

##### examples

    auto printer = [](auto const& x){ std::cout << x; };
    vta::map(printer)("I can count to", 4, '!'); // prints "I can count to 4!"

---
#### `foldl`
    template <typename Function>
    constexpr /*VariadicFunctor*/ foldl(Function&& f);

`foldl` returns a variadic functor that performs a left fold across it's parameters. If the parameters `arg1`, `arg2`, ..., `argN` are passed, the functor returns `f(...f(f(arg1, arg2), arg3), ...), argN)`. If one parameter is passed, it is the value returned. This variadic functor does not work with 0 parameters.

##### examples

    auto append = [](std::string const& s, auto x){
        std::stringstream ss;
        ss << s << x;
        return ss.str();
    };

    std::string str = vta::foldl(append)("", 1, ',', 2, " and ", 3);
    std::cout << str; // prints "1,2 and 3"

---
#### `foldr`
    template <typename Function>
    constexpr /*VariadicFunctor*/ foldr(Function&& f);

`foldr` returns a variadic functor that performs a right fold across it's parameters. If the parameters `arg1`, `arg2`, ..., `argM`, `argN` are passed, the functor returns `f(arg1, f(arg2, f(... f(argM, argN)...)`. If one parameter is passed, it is the value returned. This variadic functor does not work with 0 parameters.

##### examples

    auto subtract = [](auto l, auto r){ return l - r; };
    
    // prints -2 = (0 - (1 - (2 - 3)))
    std::cout << vta::foldr(subtract)(0, 1, 2, 3);

---
#### `all_of`
    template <typename Function>
    constexpr /*VariadicFunctor*/ all_of(Function&& f);

`all_of` returns a variadic functor that returns `true` if and only if `f(a)` returns `true` (or any type convertible to `true`) for all arguments. `f` is applied to the arguments in order. If `f(b)` returns `false` for some parameter `b`, then `f` is no longer applied to the rest of the arguments.

##### examples

    struct is_positve_int {
        bool operator()(int i) const { return i > 0; }

        template <typename T>
        bool operator()(T const&) const { return false; }
    };

    bool x = vta::all_of(is_positive_int{})(1, 2, 3);    // x = true
    bool y = vta::all_of(is_positive_int{})(-4, 2, "s"); // y = false
    bool z = vta::all_of(is_positive_int{})('2', 0);     // z = false

---
#### `any_of`
    template <typename Function>
    constexpr /*VariadicFunctor*/ any_of(Function&& f);

`any_of` returns a variadic functor that returns `true` if and only if `f(a)` returns `true` (or any type convertible to `true`) for any argument `a`. `f` is applied to the arguments in order. If `f(b)` does return `true` for some parameter `b`, then `f` is no longer applied to the rest of the arguments.

##### examples

    struct is_positve_int; // see definition under all_of documentation

    bool x = vta::any_of(is_positive_int{})(1, 2, 3);    // x = true
    bool y = vta::any_of(is_positive_int{})(-4, 2, "s"); // y = true
    bool z = vta::any_of(is_positive_int{})('2', 0);     // z = false

---
#### `none_of`
    template <typename Function>
    constexpr /*VariadicFunctor*/ none_of(Function&& f);

`any_of` returns a variadic functor that returns `true` if and only if `f(a)` returns `false` (or any type convertible to `false`) for all arguments. `f` is applied to the arguments in order. If `f(b)` returns `true` for some parameter `b`, then `f` is no longer applied to the rest of the arguments.


##### examples

    struct is_positve_int; // see definition under all_of documentation

    bool x = vta::any_of(is_positive_int{})(1, 2, 3);    // x = false
    bool y = vta::any_of(is_positive_int{})(-4, 2, "s"); // y = false
    bool z = vta::any_of(is_positive_int{})('2', 0);     // z = true

---
#### `forward_after`
    template <typename Function, typename Transformation>
    constexpr /*VariadicFunctor*/ forward_after(Function&& f);

Returns a variadic functor that forwards all of it's arguments to `f` after applying transformation `Transformation`. Documentation of available transformation are below.

    auto minus = [](auto l, auto r){ return l - r; };

    // vta::id performs no transformation
    int a = vta::forward_after<vta::id>(minus)(4, 2);      // a = 2

    // vta::flip swaps the first two arguments
    int b = vta::forward_after<vta::flip>(minus)(4, 2); // b = -2

    // prints "4321" as vta::reverse reverses the arguments
    auto printer = [](auto const& x){ std::cout << x; };
    vta::forward_after<vta::reverse>(vta::map(printer))(1, 2, '3', "4");

<a name="transformation"></a>Transformations
---------------

Transformations are types that are passed to `forward_after` in order to permute and filters the parameters before fowarding them on. All transformations must have the following `static` function,

    struct Transformation {
        template <typename Function, typename... Args>
        static auto tranform(Function&&, Args&&...);
    };

The `tranform` function calls `f` with some or all of `args...` after applying some permutation and/or transformation.

All transformations in VTA have a `constexpr` `transform` function so are available to use in `constexpr` functions.

`#### id`
    struct id;

`id` forward all arguments without modification.

##### examples

    // prints "1234"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::id>(vta::map(printer))(1, 2u, '3', "4");

---
#### `call_if`
    template <bool Condition>
    struct call_if

`call_if` calls the function only if `Condition` is `true`, otherwise noop.

##### examples

    // prints "1234"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::forward_if<true>>(vta::map(printer))(1, 2u, '3', "4");

    // prints nothing
    std::forward_after<vta::forward_if<false>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `flip`
    struct flip;

`flip` swaps the first and second parameter.

##### examples

    // prints "2134"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::flip>(vta::map(printer))(1, 2u, '3', "4");

---
#### `left_shift`
    template <unsigned N>
    struct left_shift;

`left_shift` cyclic shifts the parameters `N` places to the left. `N` must be between 0 and the number of parameters passed to `forward_after` otherwise it will fail to compile.

##### examples

    // prints "2341"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::left_shift<1u>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `right_shift`
    template <unsigned N>
    struct right_shift;


`right_shift` cyclic shifts the parameters `N` places to the right. `N` must be between 0 and the number of parameters passed to `forward_after` otherwise it will fail to compile.

##### examples

    // prints "4123"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::right_shift<1u>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `shift`
    template <int N>
    struct shift;

If `N` is greater than 0, `shift<N>` is the same as `left_shift<N>`, if `N` is less than 0, it is the same as `right_shift<-N>`. If `N` equals 0, then the arguments are forwarded without permutation.

---
#### `left_shift_tail`
    template <unsigned N>
    struct left_shift_tail;

##### examples

    // prints "1342"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::left_shift_tail<1u>>(vta::map(printer))(1, 2u, '3', "4");

`left_shift_tail` cyclic shifts the all parameters but the first, `N` places to the left. `N` must be between 0 and 1 - the number of parameters passed to `forward_after` otherwise it will fail to compile.

---
#### `right_shift_tail`
    template <unsigned N>
    struct right_shift_tail;

`right_shift_tail` cyclic shifts the all parameters but the first, `N` places to the right. `N` must be between 0 and 1 - the number of parameters passed to `forward_after` otherwise it will fail to compile.

##### examples

    // prints "1423"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::right_shift_tail<1u>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `shift_tail`
    template <int N>
    struct shift_tail;

If `N` is greater than 0, `shift_tail<N>` is the same as `left_shift_tail<N>`, if `N` is less than 0, it is the same as `right_shift_tail<-N>`. If `N` equals 0, then the arguments are forwarded without permutation.

---
#### `drop`
    template <unsigned N>
    struct drop;

If `N` is positive,`drop` forwards all but the first `N` arguments. `N` must be less than the number of parameters passed otherwise it will fail to compile. If `N` is negative, `drop` forwards all but the first `-N` arguments.

##### examples

    // prints "34"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::drop<2>>(vta::map(printer))(1, 2u, '3', "4");

    // prints "234"
    std::forward_after<vta::drop<-1>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `take`
    template <int N>
    struct take;

If `N` is positive,`take` forwards only the first `N` arguments. `N` must be less than the number of parameters passed otherwise it will fail to compile. If `N` is negative, `take` forwards all but the last `-N` arguments.

##### examples

    // prints "123"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::take<3>>(vta::map(printer))(1, 2u, '3', "4");

    // prints "1"
    std::forward_after<vta::take<-3>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `slice`
    template <int N, int M>
    struct slice;

`slice` forwards the arguments with an index inclusively between `N` and `M`. If `N` is negative (or `M` is negative) it counts backwards from the end of the parameters, e.g. -1 is the index of the last parameter. If `N > M` (after accounting for negative indices) then `slice` will fail to compile.

##### examples

    // prints "23"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::slice<1, 2>>(vta::map(printer))(1, 2u, '3', "4");

    // prints "123"
    std::forward_after<vta::slice<0, -2>>(vta::map(printer))(1, 2u, '3', "4");

    // prints ""
    std::forward_after<vta::slice<3, -1>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `swap`
    template <int N, int M>
    struct swap;

`swap` exchanges the two parameters at position `N` and `M`. If this position is negative, it is counted backwards from the end of the parameters, e.g. -1 is the index of the last parameter.

##### examples

    // prints "3214"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::swap<0, 2>>(vta::map(printer))(1, 2u, '3', "4");

    // prints "4231"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::swap<0, -1>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `cycle`
    template <int... Ns>
    struct cycle;

`cycle` permutes the parameters at the positions `Ns...` using [cycle notation](http://en.wikipedia.org/wiki/Cycle_notation). Remember that indexing starts from 0, rather than starting from 1, which is more commonly used in mathematics literature. If any positions are negative, it is counted backwards from the end of the parameters, e.g. -1 is the index of the last parameter.

All indexes must be unique. `cycle<0, 1, 1>` will not compile. `cycle<-1, 3>` will not compile if there are 4 arguments passed to `forward_after` since -1 and 3 describe the same index.

##### examples

    // prints "4213"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::cycle<0, 2, 3>>(vta::map(printer))(1, 2u, '3', "4");

    // prints "4213"
    std::forward_after<vta::cycle<-1, 1, 0, 2>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `reverse`
    struct reverse;

`reverse` reverses the order of the arguments passed.

##### examples

    // prints "4321"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::reverse>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `filter`
    template <typename Filter>
    struct filter;

`filter` only forwards on a type `Arg` if `Filter<Arg>::value` is `true`. Most structs in `<type_traits>` are appropriate as a filter. An example filter:

    template <typename T>
    struct ExampleFilter {
        static bool const value = std::is_enum<T>::value &&
                                  std::is_class<T>::value;
    };

##### examples

    // prints "123"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::filter<std::is_integral>>(vta::map(printer))(1, 2u, '3', "4");

---
#### `compose`
    template <typename... Tranformations>
    struct compose;

`compose` creates a new transformation that would occur after applying all `Transformations...` in order.

##### examples

    // prints "2431"
    auto printer = [](auto const& x){ std::cout << x; };
    std::forward_after<vta::compose<vta::flip,     // would be "2134"
                                    vta::shift<1>, // would be "1342"
                                    vta::reverse   // prints "2431"
                                   >(vta::map(printer))(1, 2u, '3', "4");

<a name="macro"></a>Macros
------

#### `VTA_FN_TO_FUNCTOR`

    #define VTA_FN_TO_FUNCTOR(...) [](auto&&... args) \
      -> decltype(auto){ return __VA_ARGS__(std::forward<decltype(args)>(args)...); }

`VTA_FN_TO_FUNCTOR` creates a lambda that calls the function that was passed as an argument. This is a workaround for passing templated functions, such as `std::max` that would require specifying the type. This macro has been provided by [Florian Weber](http://florianjw.de/en/passing_overloaded_functions.html).

##### examples

    template <typename... Args>
    auto max(Args... args) {
        return vta::foldl(VTA_FN_TO_FUNCTOR(std::max))(args...);
    }
