#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Tuple.hh>


using namespace __XVI_STD_CORE_NS;



struct dummy_allocator {};

struct custom_tuple_like
{
    int x, y;
};

struct nonimplicit_custom_tuple_like
{
    struct nonimplicit { int i; explicit operator int() const { return i; } };

    nonimplicit x, y;
};


template <size_t I> 
constexpr auto get(const custom_tuple_like& t)
{
    if constexpr (I == 0)
        return t.x;
    else if constexpr (I == 1)
        return t.y;
}

template <size_t I>
constexpr auto get(const nonimplicit_custom_tuple_like& t)
{
    if constexpr (I == 0)
        return t.x;
    else if constexpr (I == 1)
        return t.y;
}


namespace __XVI_STD_CORE_NS_DECL
{

namespace __detail
{

template <>
struct __is_tuple_like<custom_tuple_like> : true_type {};

template <>
struct __is_tuple_like<nonimplicit_custom_tuple_like> : true_type {};

}

template <>
struct tuple_size<custom_tuple_like> : integral_constant<size_t, 2> {};

template <size_t I>
    requires (I <= 1)
struct tuple_element<I, custom_tuple_like> { using type = int; };

template <>
struct tuple_size<nonimplicit_custom_tuple_like> : integral_constant<size_t, 2> {};

template <size_t I>
    requires (I <= 1)
struct tuple_element<I, nonimplicit_custom_tuple_like> : integral_constant<size_t, 2> {};

}


TEST(Tuple, CommonReference)
{
    struct A {};
    struct B : A {};
    struct C {};

    using T = tuple<A&, B&, C&>;
    using U = tuple<B&, A&, C&>;
    using V = tuple<A&, A&, C&>;

    using Common = common_reference_t<T, U>;

    EXPECT_SAME_TYPE(Common, V);
}

TEST(Tuple, CommonType)
{
    using T = tuple<int, long, short>;
    using U = tuple<int, int, int>;
    using V = tuple<int, long, int>;

    using Common = common_type_t<T, U>;

    EXPECT_SAME_TYPE(Common, V);
}

TEST(Tuple, TupleProtocol)
{
    using T = tuple<int, long, short>;

    EXPECT_EQ(tuple_size_v<T>, 3);

    using T0 = tuple_element_t<0, T>;
    using T1 = tuple_element_t<1, T>;
    using T2 = tuple_element_t<2, T>;
    EXPECT_SAME_TYPE(T0, int);
    EXPECT_SAME_TYPE(T1, long);
    EXPECT_SAME_TYPE(T2, short);
}

TEST(Tuple, Layout)
{
    using T = tuple<int, long, short>;
    struct U { int i; long l; short s; };

    EXPECT_EQ(sizeof(T), sizeof(U));
}

TEST(Tuple, EmptyElementOptimisation)
{
    struct empty {};

    using T = tuple<int>;
    using U = tuple<int, empty>;
    using V = tuple<int, empty, empty>;
    //using W = tuple<empty, int, empty>;
    using X = tuple<empty, int>;
    //using Y = tuple<empty, empty, int>;
    using Z = tuple<int, empty, empty, empty>;

    EXPECT_EQ(sizeof(T), sizeof(int));
    EXPECT_EQ(sizeof(U), sizeof(int));
    EXPECT_EQ(sizeof(V), sizeof(int));
    //EXPECT_EQ(sizeof(W), sizeof(int));
    EXPECT_EQ(sizeof(X), sizeof(int));
    //EXPECT_EQ(sizeof(Y), sizeof(int));
    EXPECT_EQ(sizeof(Z), sizeof(int));
}

TEST(Tuple, References)
{
    using T = tuple<int&>;

    // Constness is shallow.
    using R1 = decltype(get<0>(declval<const T&>()));
    EXPECT_SAME_TYPE(R1, int&);

    // Lvalues shouldn't become rvalues.
    using R2 = decltype(get<0>(declval<T&&>()));
    EXPECT_SAME_TYPE(R2, int&);
}

TEST(Tuple, TrivialDestructibility)
{
    struct nontrivial { ~nontrivial(); };
    using T = tuple<>;
    using U = tuple<int, int, int>;
    using V = tuple<int, nontrivial>;

    EXPECT(std::is_trivially_destructible_v<T>);
    EXPECT(std::is_trivially_destructible_v<U>);
    EXPECT(!std::is_trivially_destructible_v<V>);
}

TEST(Tuple, DefaultConstructor)
{
    using T = tuple<>;
    using U = tuple<int, int, int>;

    EXPECT(std::is_default_constructible_v<T>);
    EXPECT(!std::is_trivially_default_constructible_v<T>);
    EXPECT(!std::is_trivially_default_constructible_v<U>);

    U u;

    EXPECT_EQ(get<0>(u), 0);
    EXPECT_EQ(get<1>(u), 0);
    EXPECT_EQ(get<2>(u), 0);
}

TEST(Tuple, DeletedDefaultConstructor)
{
    struct nondefault { nondefault() = delete; nondefault(int) {} };
    using T = tuple<int, nondefault>;

    EXPECT(!std::is_default_constructible_v<T>);
}

TEST(Tuple, ImplicitDefaultConstructor)
{
    struct nonimplicit { explicit nonimplicit(); };
    using T = tuple<>;
    using U = tuple<int, int, int>;
    using V = tuple<int, nonimplicit>;

    EXPECT(__detail::__is_implicit_default_constructible_v<T>);
    EXPECT(__detail::__is_implicit_default_constructible_v<U>);
    EXPECT(!__detail::__is_implicit_default_constructible_v<V>);
}

TEST(Tuple, ElementCopyConstructor)
{
    struct noncopy { noncopy(const noncopy&) = delete; };
    using T = tuple<int, int>;
    using U = tuple<int, noncopy>;

    T t(1, 2);

    EXPECT_EQ(get<0>(t), 1);
    EXPECT_EQ(get<1>(t), 2);

    EXPECT(!(std::is_constructible_v<U, const int&, const noncopy&>));
}

TEST(Tuple, ImplicitElementCopyConstructor)
{
    struct nonimplicit { explicit nonimplicit(int); };
    using T = tuple<int, int>;
    using U = tuple<int, nonimplicit>;

    EXPECT((__detail::__is_implicit_constructible_v<T>));
    EXPECT((std::is_constructible_v<U, int, int>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, int, int>));
}

TEST(Tuple, ElementConvertingCopyConstructor)
{
    using T = tuple<int, int, int>;

    T t(1, 42UL, '!');

    EXPECT_EQ(get<0>(t), 1);
    EXPECT_EQ(get<1>(t), 42);
    EXPECT_EQ(get<2>(t), '!');
}

TEST(Tuple, ImplicitElementConvertingCopyConstructor)
{
    struct nonimplicit { explicit nonimplicit(int); };
    using T = tuple<int, int>;
    using U = tuple<int, nonimplicit>;

    EXPECT((__detail::__is_implicit_constructible_v<T, long, char>));
    EXPECT((std::is_constructible_v<U, char, long>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, char, long>));
}

TEST(Tuple, CopyConstructor)
{
    struct noncopy { noncopy(const noncopy&) = delete; };
    struct nontrivial { nontrivial(const nontrivial&); };
    using T = tuple<>;
    using U = tuple<int, int>;
    using V = tuple<noncopy, int>;
    using W = tuple<nontrivial>;

    EXPECT(std::is_trivially_copy_constructible_v<T>);
    EXPECT(std::is_trivially_copy_constructible_v<U>);

    EXPECT(!std::is_copy_constructible_v<V>);

    EXPECT(std::is_copy_constructible_v<W>);
    EXPECT(!std::is_trivially_copy_constructible_v<W>);

    const U from (42, 7);
    U to(from);

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), 7);
}

TEST(Tuple, MoveConstructor)
{
    struct nonmove { nonmove(nonmove&) = delete; };
    struct nontrivial { nontrivial(nontrivial&&); };
    using T = tuple<>;
    using U = tuple<int, int>;
    using V = tuple<nonmove, int>;
    using W = tuple<nontrivial>;

    EXPECT(std::is_trivially_move_constructible_v<T>);
    EXPECT(std::is_trivially_move_constructible_v<U>);

    EXPECT(!std::is_move_constructible_v<V>);

    EXPECT(std::is_move_constructible_v<W>);
    EXPECT(!std::is_trivially_move_constructible_v<W>);

    U from (42, 7);
    U to(std::move(from));

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), 7);
}

TEST(Tuple, ConvertTupleLvalueRefConstructor)
{
    using T = tuple<int, int, int>;
    using U = tuple<long, char, bool>;

    U from(42L, '!', true);
    T to(from);

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), '!');
    EXPECT_EQ(get<2>(to), 1);
}

TEST(Tuple, ImplicitConvertTupleLvalueRefConstructor)
{
    struct nonimplicit { explicit operator int(); operator int() const = delete; };
    using T = tuple<int, int, int>;
    using U = tuple<long, char, nonimplicit>;
    using V = tuple<long, char, int>;

    EXPECT((__detail::__is_implicit_constructible_v<T, V&>));
    EXPECT((std::is_constructible_v<T, U&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, U&>));
}

TEST(Tuple, ConvertTupleLvalueConstRefConstructor)
{
    using T = tuple<int, int, int>;
    using U = tuple<long, char, bool>;

    const U from(42L, '!', true);
    T to(from);

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), '!');
    EXPECT_EQ(get<2>(to), 1);
}

TEST(Tuple, ImplicitConvertTupleLvalueConstRefConstructor)
{
    struct nonimplicit { explicit operator int() const; };
    using T = tuple<int, int, int>;
    using U = tuple<long, char, nonimplicit>;
    using V = tuple<long, char, int>;

    EXPECT((__detail::__is_implicit_constructible_v<T, const V&>));
    EXPECT((std::is_constructible_v<T, const U&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, const U&>));
}

TEST(Tuple, ConvertTupleRvalueRefConstructor)
{
    struct movable { int i; operator int() && { return i; } };
    using T = tuple<int, int, int>;
    using U = tuple<long, char, movable>;

    U from(42L, '!', 7);
    T to(std::move(from));

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), '!');
    EXPECT_EQ(get<2>(to), 7);
}

TEST(Tuple, ImplicitConvertTupleRvalueRefConstructor)
{
    struct nonimplicit_movable { int i; explicit operator int() &&; operator int() const && = delete; };
    using T = tuple<int, int, int>;
    using U = tuple<long, char, nonimplicit_movable>;
    using V = tuple<long, char, int>;

    EXPECT((__detail::__is_implicit_constructible_v<T, V&&>));
    EXPECT((std::is_constructible_v<T, U&&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, U&&>));
}

TEST(Tuple, ConvertTupleRvalueConstRefConstructor)
{
    struct movable { int i; operator int() const && { return i; } };
    using T = tuple<int, int, int>;
    using U = tuple<long, char, movable>;

    const U from(42L, '!', 7);
    T to(std::move(from));

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), '!');
    EXPECT_EQ(get<2>(to), 7);
}

TEST(Tuple, ImplicitConvertTupleRvalueConstRefConstructor)
{
    struct nonimplicit_movable { int i; explicit operator int() const &&; };
    using T = tuple<int, int, int>;
    using U = tuple<long, char, nonimplicit_movable>;
    using V = tuple<long, char, int>;

    EXPECT((__detail::__is_implicit_constructible_v<T, const V&&>));
    EXPECT((std::is_constructible_v<T, const U&&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, const U&&>));
}

TEST(Tuple, PairLvalueRefConstructor)
{
    using T = tuple<int, int>;
    using U = pair<long, char>;

    U from(42L, '!');
    T to(from);

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), '!');
}

TEST(Tuple, ImplicitPairLvalueRefConstructor)
{
    struct nonimplicit { explicit operator int(); operator int() const = delete; };
    using T = tuple<int, int>;
    using U = pair<int, nonimplicit>;
    using V = pair<long, char>;

    EXPECT((__detail::__is_implicit_constructible_v<T, V&>));
    EXPECT((std::is_constructible_v<T, U&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, U&>));
}

TEST(Tuple, PairLvalueConstRefConstructor)
{
    using T = tuple<int, int>;
    using U = pair<long, char>;

    const U from(42L, '!');
    T to(from);

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), '!');
}

TEST(Tuple, ImplicitPairLvalueConstRefConstructor)
{
    struct nonimplicit { explicit operator int() const; };
    using T = tuple<int, int>;
    using U = pair<int, nonimplicit>;
    using V = pair<long, char>;

    EXPECT((__detail::__is_implicit_constructible_v<T, const V&>));
    EXPECT((std::is_constructible_v<T, const U&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, const U&>));
}

TEST(Tuple, TupleLikeConstructor)
{
    using T = tuple<int, int>;

    const custom_tuple_like from{7, 42};
    T to(from);

    EXPECT_EQ(get<0>(to), 7);
    EXPECT_EQ(get<1>(to), 42);
}

TEST(Tuple, ImplicitTupleLikeConstructor)
{
    using T = tuple<int, int>;

    EXPECT((__detail::__is_implicit_constructible_v<T, const custom_tuple_like&>));
    EXPECT((std::is_constructible_v<T, const nonimplicit_custom_tuple_like&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, const nonimplicit_custom_tuple_like&>));
}

/*TEST(Tuple, AllocatorConstructor)
{
    using T = tuple<>;
    using U = tuple<int, int, int>;

    EXPECT(std::is_constructible_v<T, allocator_arg_t, dummy_allocator>);

    U u(allocator_arg, dummy_allocator{});

    EXPECT_EQ(get<0>(u), 0);
    EXPECT_EQ(get<1>(u), 0);
    EXPECT_EQ(get<2>(u), 0);
}*/

TEST(Tuple, CopyAssignment)
{
    struct noncopy { void operator=(const noncopy&) = delete; };
    struct nontrivial { void operator=(const nontrivial&); };
    using T = tuple<>;
    using U = tuple<int, int>;
    using V = tuple<noncopy, int>;
    using W = tuple<nontrivial>;

    // GCC bug.
    EXPECT(std::is_trivially_copy_assignable_v<T>);
    
    EXPECT(std::is_trivially_copy_assignable_v<U>);
    EXPECT(!std::is_copy_assignable_v<V>);
    EXPECT(std::is_copy_assignable_v<W>);
    EXPECT(!std::is_trivially_copy_assignable_v<W>);

    const U from(7, 42);
    U to;

    to = from;

    EXPECT_EQ(get<0>(to), 7);
    EXPECT_EQ(get<1>(to), 42);
}

TEST(Tuple, CopyAssignmentConst)
{
    using T = tuple<int&, int&>;
    using U = tuple<int, int>;
    using V = tuple<>;

    EXPECT(!std::is_copy_assignable_v<const U>);
    EXPECT(std::is_copy_assignable_v<const V>);

    int a = 42;
    int b = 7;
    int x = 0;
    int y = 0;

    const T from(a, b);
    const T to(x, y);

    to = from;

    EXPECT_EQ(x, 42);
    EXPECT_EQ(y, 7);
}

TEST(Tuple, MoveAssignment)
{
    struct nonmove { void operator=(nonmove&&) = delete; };
    struct nontrivial { void operator=(nontrivial&&); };
    using T = tuple<>;
    using U = tuple<int, int>;
    using V = tuple<nonmove, int>;
    using W = tuple<nontrivial>;

    // GCC bug.
    EXPECT(std::is_trivially_move_assignable_v<T>);
    
    EXPECT(std::is_trivially_move_assignable_v<U>);
    EXPECT(!std::is_move_assignable_v<V>);
    EXPECT(std::is_move_assignable_v<W>);
    EXPECT(!std::is_trivially_move_assignable_v<W>);

    const U from(7, 42);
    U to;

    to = std::move(from);

    EXPECT_EQ(get<0>(to), 7);
    EXPECT_EQ(get<1>(to), 42);   
}

TEST(Tuple, MoveAssignmentConst)
{
    using T = tuple<int&, int&>;
    using U = tuple<int, int>;
    using V = tuple<>;

    EXPECT(!std::is_move_assignable_v<const U>);
    EXPECT(std::is_move_assignable_v<const V>);

    int a = 42;
    int b = 7;
    int x = 0;
    int y = 0;

    T from(a, b);
    const T to(x, y);

    to = std::move(from);

    EXPECT_EQ(x, 42);
    EXPECT_EQ(y, 7);
}

TEST(Tuple, ConvertTupleLvalueConstRefAssignment)
{
    using T = tuple<int, int, int>;
    using U = tuple<long, bool, char>;
    using V = tuple<void*, int, int>;

    EXPECT(!(std::is_assignable_v<T&, const V&>));

    const U from(42L, true, '!');
    T to;

    to = from;

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), 1);
    EXPECT_EQ(get<2>(to), '!');
}

TEST(Tuple, ConvertTupleLvalueConstRefAssignmentConst)
{
    using T = tuple<int&, int&>;
    using U = tuple<char, long>;
    using V = tuple<int, void*>;

    EXPECT(!(std::is_assignable_v<const T&, const V&>));
    EXPECT(!(std::is_assignable_v<const U&, const U&>));

    int x = 0;
    int y = 0;

    const U from('!', 42L);
    const T to(x, y);

    to = from;

    EXPECT_EQ(x, '!');
    EXPECT_EQ(y, 42);
}

TEST(Tuple, ConvertTupleRvalueRefAssignment)
{
    struct noncopy { void operator=(const noncopy&) = delete; operator int() const; };
    using T = tuple<int, int>;
    using U = tuple<long, char>;
    using V = tuple<int, void*>;
    using W = tuple<int, noncopy>;

    EXPECT((std::is_assignable_v<T&, T>));
    EXPECT(!(std::is_assignable_v<T&, V>));
    EXPECT((std::is_assignable_v<T&, W>));

    U from(42L, '!');
    T to;

    to = std::move(from);

    EXPECT_EQ(get<0>(to), 42);
    EXPECT_EQ(get<1>(to), '!');
}

TEST(Tuple, ConvertTupleRvalueRefAssignmentConst)
{
    using T = tuple<int&, int&>;
    using U = tuple<char, long>;
    using V = tuple<int, void*>;

    EXPECT(!(std::is_assignable_v<const T&, V>));
    EXPECT(!(std::is_assignable_v<const U&, U>));

    int x = 0;
    int y = 0;

    U from('!', 42L);
    const T to(x, y);

    to = std::move(from);

    EXPECT_EQ(x, '!');
    EXPECT_EQ(y, 42);

    EXPECT(!(std::is_assignable_v<const T&, tuple<int, int, int>>));
}

TEST(Tuple, ConvertPairLvalueConstRefAssignment)
{
    using T = tuple<int, int>;
    using U = pair<int, int>;
    using V = pair<long, char>;

    T t1;
    T t2;

    const U u(7, 42);
    const V v(42L, '!');

    t1 = u;
    t2 = v;

    EXPECT_EQ(get<0>(t1), 7);
    EXPECT_EQ(get<1>(t1), 42);
    EXPECT_EQ(get<0>(t2), 42);
    EXPECT_EQ(get<1>(t2), '!');
}

TEST(Tuple, ConvertPairLvalueConstRefAssignmentConst)
{
    using T = tuple<int&, int&>;
    using U = pair<int, int>;
    using V = pair<long, char>;

    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    const T t1(a, b);
    const T t2(c, d);

    const U u(7, 42);
    const V v(42, '!');

    t1 = u;
    t2 = v;

    EXPECT_EQ(a, 7);
    EXPECT_EQ(b, 42);
    EXPECT_EQ(c, 42);
    EXPECT_EQ(d, '!');
}

TEST(Tuple, ConvertPairRvalueRefAssignment)
{
    struct moveonly { int i; void operator=(const moveonly&) = delete; constexpr operator int() const { return i; }};
    using T = tuple<int, int>;
    using U = pair<int, int>;
    using V = pair<long, moveonly>;

    T t1;
    T t2;

    U u(7, 42);
    V v(42, 7);

    t1 = std::move(u);
    t2 = std::move(v);

    EXPECT_EQ(get<0>(t1), 7);
    EXPECT_EQ(get<1>(t1), 42);
    EXPECT_EQ(get<0>(t2), 42);
    EXPECT_EQ(get<1>(t2), 7);
}

TEST(Tuple, ConvertPairRvalueRefAssignmentConst)
{
    struct moveonly { int i; void operator=(const moveonly&) = delete; constexpr operator int() const { return i; }};
    using T = tuple<int&, int&>;
    using U = pair<int, int>;
    using V = pair<long, moveonly>;

    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;

    const T t1(a, b);
    const T t2(c, d);

    U u(7, 42);
    V v(42, 7);

    t1 = std::move(u);
    t2 = std::move(v);

    EXPECT_EQ(a, 7);
    EXPECT_EQ(b, 42);
    EXPECT_EQ(c, 42);
    EXPECT_EQ(d, 7);
}

TEST(Tuple, TupleLikeAssignment)
{
    using T = tuple<int, int>;

    T to;
    const custom_tuple_like from{7, 42};

    to = from;

    EXPECT_EQ(get<0>(to), 7);
    EXPECT_EQ(get<1>(to), 42);
}

TEST(Tuple, TupleLikeAssignmentConst)
{
    using T = tuple<int&, int&>;

    int a = 0;
    int b = 0;

    const T to(a, b);
    const custom_tuple_like from{7, 42};

    to = from;

    EXPECT_EQ(a, 7);
    EXPECT_EQ(b, 42);
}

TEST(Tuple, Swap)
{
    using T = tuple<int, int>;

    EXPECT(std::is_nothrow_swappable_v<T>);

    T one(1, 2);
    T two(3, 4);

    one.swap(two);

    EXPECT_EQ(get<0>(one), 3);
    EXPECT_EQ(get<1>(one), 4);
    EXPECT_EQ(get<0>(two), 1);
    EXPECT_EQ(get<1>(two), 2);
}

TEST(Tuple, SwapConst)
{
    using T = tuple<int&, int&>;

    EXPECT(std::is_nothrow_swappable_v<T>);

    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    const T one(a, b);
    const T two(c, d);

    one.swap(two);

    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, 4);
    EXPECT_EQ(c, 1);
    EXPECT_EQ(d, 2);
}

TEST(Tuple, MakeTuple)
{
    using T = tuple<int, int, int>;
    using U = tuple<int&, const char&>;

    int x = 0;
    char c = 'x';

    EXPECT_SAME_TYPE(T, decltype(make_tuple(1, 2, 3)));
    EXPECT_SAME_TYPE(U, decltype(make_tuple(ref(x), cref(c))));

    auto t = make_tuple(1, 2, 3);

    EXPECT_EQ(get<0>(t), 1);
    EXPECT_EQ(get<1>(t), 2);
    EXPECT_EQ(get<2>(t), 3);
}

TEST(Tuple, ForwardAsTuple)
{
    using T = tuple<int&&, int&&>;
    using U = tuple<int&, const int&>;

    int x = 0;
    const int y = 0;

    EXPECT_SAME_TYPE(T, decltype(forward_as_tuple(1, 2)));
    EXPECT_SAME_TYPE(U, decltype(forward_as_tuple(x, y)));
}

TEST(Tuple, Tie)
{
    using T = tuple<int, int, int>;

    int x = 0;
    int y = 0;

    tie(x, ignore, y) = T(1, 2, 3);

    EXPECT_EQ(x, 1);
    EXPECT_EQ(y, 3);
}

TEST(Tuple, Cat)
{
    using T = tuple<int>;
    using U = tuple<char, long>;
    using V = tuple<int, int>;
    using W = tuple<>;
    using Z = tuple<int, char, long, int, int>;

    auto test = tuple_cat(T(1), U(2, 3), W(), V(4, 5));

    EXPECT_SAME_TYPE(Z, decltype(test));

    EXPECT_EQ(get<0>(test), 1);
    EXPECT_EQ(get<1>(test), 2);
    EXPECT_EQ(get<2>(test), 3);
    EXPECT_EQ(get<3>(test), 4);
    EXPECT_EQ(get<4>(test), 5);
}

TEST(Tuple, Apply)
{
    auto fn = [](int x, int y) { return x + y; };

    auto result = apply(fn, make_tuple(40, 2));

    EXPECT_EQ(result, 42);
}

TEST(Tuple, MakeFromTuple)
{
    struct data { int x; int y; };

    auto result = make_from_tuple<data>(make_tuple(42, 7));

    EXPECT_SAME_TYPE(decltype(result), data);

    EXPECT_EQ(result.x, 42);
    EXPECT_EQ(result.y, 7);
}

TEST(Tuple, Equality)
{
    using E = tuple<>;
    using T = tuple<int, int>;
    using U = tuple<long, char>;
    using V = pair<int, int>;

    E e1;
    E e2;

    EXPECT_EQ(e1, e2);

    T a(0, 0);
    T b(0, 1);
    T c(1, 0);
    T d(1, 1);

    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(a, d);
    EXPECT_NE(b, c);
    EXPECT_NE(b, d);
    EXPECT_NE(c, d);

    EXPECT_EQ(a, a);
    EXPECT_EQ(b, b);
    EXPECT_EQ(c, c);
    EXPECT_EQ(d, d);

    U u(1L, 1);
    V v(1, 1);

    EXPECT_EQ(u, d);
    EXPECT_EQ(v, d);
}

TEST(Tuple, Comparison)
{
    using E = tuple<>;
    using T = tuple<int, int>;
    using U = tuple<long, char>;
    using V = pair<int, int>;

    E e1;
    E e2;

    EXPECT_EQ(e1, e2);

    T a(0, 0);
    T b(0, 1);
    T c(1, 0);
    T d(1, 1);

    EXPECT_GT(d, c);
    EXPECT_GT(c, b);
    EXPECT_GT(b, a);

    EXPECT_LT(a, d);

    U u(42L, 0);
    V v(1, 1);

    EXPECT_GT(u, d);
    EXPECT_GT(v, c);
}

TEST(Tuple, UsesAllocatorTrait)
{
    class Alloc {};
    using T = tuple<>;

    EXPECT_EQ((uses_allocator<T, Alloc>::value), true);
}

TEST(Tuple, NonMemberSwap)
{
    using T = tuple<int, int>;

    T one(1, 2);
    T two(3, 4);

    swap(one, two);

    EXPECT_EQ(get<0>(one), 3);
    EXPECT_EQ(get<1>(one), 4);
    EXPECT_EQ(get<0>(two), 1);
    EXPECT_EQ(get<1>(two), 2);
}

TEST(Tuple, DeductionGuides)
{
    using T = tuple<int, char, long>;
    using U = tuple<int, int>;

    tuple t(1, '2', 3L);
    tuple u(make_pair(7, 42));

    EXPECT_SAME_TYPE(T, decltype(t));
    EXPECT_SAME_TYPE(U, decltype(u));
}
