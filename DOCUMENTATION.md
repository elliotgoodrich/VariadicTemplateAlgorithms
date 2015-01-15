VTA Documentation
================

Predicates
-----------
#### is_homogenous
    template <typename... Args>
    struct is_homogenous {
        static bool const value;
    };

#### is_unique
    template <int... Ns>
    struct is_unique {
        static bool const value;
    };

Functions
---------
   
#### head
    template <typename Head, typename... Args>
    constexpr Head&& head(Head&&, Args&&...) noexcept;

#### last
    template <typename... Args, typename Last>
    constexpr Last&& head(Args&&..., Last&&) noexcept;

#### at
    template <int N, typename... Before, typename Arg, typename... After>
    constexpr Arg&& at(Before&&..., Arg&&, After&&...) noexcept;

#### add_const
    template <typename T>
    constexpr T const& add_const(T&&) noexcept;

#### VariadicFunctor
The following functions in the Variadic Template Algorithm library create a variadic functor. These functors have an unspecified type, but have the following member function signatures.

    struct /*unspecified*/ {
        template <typename... Args>
        constexpr /*hard to know*/ operator()(Args&&... args) const;

        template <typename... Args>
        /*hard to know*/ operator()(Args&&... args);
    };

The return type of `operator()` differs and will be specified for each function.

#### map
    template <typename Function>
    constexpr /*unspecified VariadicFunctor*/ map(Function&& f);
 
    
#### foldl
    template <typename Function>
    constexpr /*unspecified VariadicFunctor*/ foldl(Function&& f);

#### foldr
    template <typename Function>
    constexpr /*unspecified VariadicFunctor*/ foldr(Function&& f);

#### all_of
    template <typename Function>
    constexpr /*unspecified VariadicFunctor*/ all_of(Function&& f);

#### any_of
    template <typename Function>
    constexpr /*unspecified VariadicFunctor*/ any_of(Function&& f);

#### none_of
    template <typename Function>
    constexpr /*unspecified VariadicFunctor*/ none_of(Function&& f);

#### forward_after
    template <typename Function, typename Transformation>
    constexpr /*unspecified VariadicFunctor*/ forward_after(Function&& f);


Transformations
-------------

#### id
    struct id;

#### forward_if
    template <bool Condition>
    struct forward_if;

#### flip
    struct flip;

#### left_shift
    template <unsigned N>
    struct left_shift;
    
#### right_shift
    template <unsigned N>
    struct right_shift;
    
#### shift
    template <int N>
    struct shift;
    
#### left_shift_tail
    template <unsigned N>
    struct left_shift_tail;
    
#### right_shift_tail
    template <unsigned N>
    struct right_shift_tail;
    
#### shift_tail
    template <int N>
    struct shift_tail;
    
#### drop
    template <unsigned N>
    struct drop;

#### take
    template <unsigned N>
    struct drop;
    
#### slice
    template <int N, int M>
    struct slice;
    
#### swap
    template <int N, int M>
    struct swap;
    
#### cycle
    template <int... Ns>
    struct cycle;
    
#### reverse
    struct reverse;

#### filter
    template <typename Filter>
    struct filter;
    
#### compose
    template <typename... Tranformations>
    struct compose;

