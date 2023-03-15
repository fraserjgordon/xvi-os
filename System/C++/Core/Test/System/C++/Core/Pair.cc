#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Pair.hh>

#include <System/C++/Core/Tuple.hh>


using namespace __XVI_STD_CORE_NS;


TEST(Pair, Layout)
{
    struct two_ints { int x, y; };
    using T = pair<int, int>;

    EXPECT(std::is_standard_layout_v<T>);
    EXPECT((std::is_layout_compatible_v<T, two_ints>));
}

TEST(Pair, NoTrivialDefaultConstruct)
{
    struct nontrivial { nontrivial(); };
    using T = pair<int, int>;
    using U = pair<int, nontrivial>;

    // Note: pair is never trivially default constructible, even for trivially default constructible types. The standard
    //       requires that the default constructor value-initialises the members.
    EXPECT(!std::is_trivially_default_constructible_v<T>);
    EXPECT(!std::is_trivially_default_constructible_v<U>);
}

TEST(Pair, ImplicitDefaultConstruct)
{
    struct nonimplicit { explicit nonimplicit(); };
    using T = pair<int, int>;
    using U = pair<int, nonimplicit>;

    EXPECT(__detail::__is_implicit_default_constructible_v<T>);
    EXPECT(!__detail::__is_implicit_default_constructible_v<U>);
}

TEST(Pair, ImplicitElementConstruct)
{
    struct nonimplicit { explicit nonimplicit(int); };
    using T = pair<int, int>;
    using U = pair<int, nonimplicit>;

    EXPECT((std::is_constructible_v<U, int, int>));

    EXPECT((__detail::__is_implicit_constructible_v<T, int, int>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, int, int>));
}

TEST(Pair, ImplicitElementConvertConstruct)
{
    struct nonimplicit { explicit nonimplicit(int); };

    using T = pair<int, int>;
    using U = pair<int, nonimplicit>;

    EXPECT((std::is_constructible_v<U, long, int>));

    EXPECT((__detail::__is_implicit_constructible_v<T, long, int>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, long, int>));
}

TEST(Pair, ImplicitConvertPairLvalueRef)
{
    struct nonimplicit { explicit nonimplicit(int); };

    using T = pair<int, int>;
    using U = pair<int, nonimplicit>;
    using V = pair<long, int>;

    EXPECT((std::is_constructible_v<U, V&>));

    EXPECT((__detail::__is_implicit_constructible_v<T, V&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, V&>));
}

TEST(Pair, ImplicitConvertPairLvalueConstRef)
{
    struct nonimplicit { explicit nonimplicit(int); };

    using T = pair<int, int>;
    using U = pair<int, nonimplicit>;
    using V = pair<long, int>;

    EXPECT((std::is_constructible_v<U, const V&>));

    EXPECT((__detail::__is_implicit_constructible_v<T, const V&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, const V&>));
}

TEST(Pair, ImplicitConvertPairRvalueRef)
{
    struct nonimplicit { explicit nonimplicit(int); };

    using T = pair<int, int>;
    using U = pair<int, nonimplicit>;
    using V = pair<long, int>;

    EXPECT((std::is_constructible_v<U, V&&>));

    EXPECT((__detail::__is_implicit_constructible_v<T, V&&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, V&&>));
}

TEST(Pair, ImplicitConvertPairRvalueConstRef)
{
    struct nonimplicit { explicit nonimplicit(int); };

    using T = pair<int, int>;
    using U = pair<int, nonimplicit>;
    using V = pair<long, int>;

    EXPECT((std::is_constructible_v<U, const V&&>));

    EXPECT((__detail::__is_implicit_constructible_v<T, const V&>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, const V&>));
}

TEST(Pair, TrivialCopyConstruct)
{
    struct nontrivial { nontrivial(const nontrivial&); };
    using T = pair<int, int>;
    using U = pair<int, nontrivial>;

    EXPECT(std::is_trivially_copy_constructible_v<T>);
    EXPECT(!std::is_trivially_copy_constructible_v<U>);
}

TEST(Pair, TrivialMoveConstruct)
{
    struct nontrivial { nontrivial(nontrivial&&); };
    using T = pair<int, int>;
    using U = pair<int, nontrivial>;

    EXPECT(std::is_trivially_move_constructible_v<T>);
    EXPECT(!std::is_trivially_move_constructible_v<U>);
}

TEST(Pair, DefaultConstruct)
{
    using T = pair<int, int>;

    T test;

    EXPECT_EQ(test.first, 0);
    EXPECT_EQ(test.second, 0);
}

TEST(Pair, ElementConstruct)
{
    using T = pair<int, int>;

    T test(0, 42);

    EXPECT_EQ(test.first, 0);
    EXPECT_EQ(test.second, 42);
}

TEST(Pair, ElementConvertConstruct)
{
    using T = pair<int, int>;

    T test(42.0f, 7LL);

    EXPECT_EQ(test.first, 42);
    EXPECT_EQ(test.second, 7);
}

TEST(Pair, ConvertPairLvalueRef)
{
    using T = pair<int, int>;
    using U = pair<long, char>;

    U from(42L, '!');
    T test(from);

    EXPECT_EQ(test.first, 42);
    EXPECT_EQ(test.second, '!');
}

TEST(Pair, ConvertPairLvalueConstRef)
{
    using T = pair<int, int>;
    using U = pair<long, char>;

    const U from(42L, '!');
    T test(from);

    EXPECT_EQ(test.first, 42);
    EXPECT_EQ(test.second, '!');
}

TEST(Pair, ConvertPairRvalueRef)
{
    using T = pair<int, int>;
    using U = pair<long, char>;

    T test(U(42L, '!'));

    EXPECT_EQ(test.first, 42);
    EXPECT_EQ(test.second, '!');
}

TEST(Pair, ConvertPairRvalueConstRef)
{
    using T = pair<int, int>;
    using U = pair<long, char>;

    const U from(42L, '!');
    T test(std::move(from));

    EXPECT_EQ(test.first, 42);
    EXPECT_EQ(test.second, '!');
}

TEST(Pair, PairlikeConstruct)
{
    using T = pair<int, int>;
    using U = tuple<int, int>;

    const U from(7, 42);
    T test(from);

    EXPECT_EQ(test.first, 7);
    EXPECT_EQ(test.second, 42);
}

TEST(Pair, PiecewiseConstruct)
{
    using T = pair<int, int>;

    T test(piecewise_construct, make_tuple<int>(7), make_tuple<int>(42));

    EXPECT_EQ(test.first, 7);
    EXPECT_EQ(test.second, 42);
}

TEST(Pair, TrivialCopyAssign)
{
    struct nontrivial { nontrivial& operator=(const nontrivial&); };
    using T = pair<int, int>;
    using U = pair<int, nontrivial>;

    EXPECT(std::is_trivially_copy_assignable_v<T>);
    EXPECT(!std::is_trivially_copy_assignable_v<U>);
}

TEST(Pair, TrivialMoveAssign)
{
    struct nontrivial { nontrivial& operator=(nontrivial&&); };
    using T = pair<int, int>;
    using U = pair<int, nontrivial>;

    EXPECT(std::is_trivially_move_assignable_v<T>);
    EXPECT(!std::is_trivially_move_assignable_v<U>);
}

TEST(Pair, TrivialDestructor)
{
    struct nontrivial { ~nontrivial(); };
    using T = pair<int, int>;
    using U = pair<int, nontrivial>;

    EXPECT(std::is_trivially_destructible_v<T>);
    EXPECT(!std::is_trivially_destructible_v<U>);
}

TEST(Pair, DeletedCopyAssign)
{
    struct noncopy { void operator=(const noncopy&) = delete; };
    using T = pair<int, int>;
    using U = pair<int, noncopy>;

    EXPECT(std::is_copy_assignable_v<T>);
    EXPECT(!std::is_copy_assignable_v<U>);
}

TEST(Pair, DeletedMoveAssign)
{
    struct nonmove { void operator=(nonmove&&) = delete; };
    using T = pair<int, int>;
    using U = pair<int, nonmove>;

    EXPECT(std::is_move_assignable_v<T>);
    EXPECT(!std::is_move_assignable_v<U>);
}

TEST(Pair, CopyAssign)
{
    using T = pair<int, int>;

    T from(42, 7);
    T to;

    to = from;

    EXPECT_EQ(to.first, 42);
    EXPECT_EQ(to.second, 7);
}

TEST(Pair, MoveAssign)
{
    using T = pair<int, int>;

    T from(42, 7);
    T to;

    to = std::move(from);

    EXPECT_EQ(to.first, 42);
    EXPECT_EQ(to.second, 7);
}

TEST(Pair, ConstAssignLvalueConstRef)
{
    using T = pair<int&, int&>;

    int x = 42;
    int y = 7;
    int a = 0;
    int b = 0;

    const T from(x, y);
    const T to(a, b);

    to = from;

    EXPECT_EQ(a, 42);
    EXPECT_EQ(b, 7);
}

TEST(Pair, ConstAssignRvalueRef)
{
    using T = pair<int&, int&>;

    int x = 42;
    int y = 7;
    int a = 0;
    int b = 0;

    T from(x, y);
    const T to(a, b);

    to = std::move(from);

    EXPECT_EQ(a, 42);
    EXPECT_EQ(b, 7);
}

TEST(Pair, AssignConvertLvalueConstRef)
{
    using T = pair<int, int>;
    using U = pair<long, char>;

    const U from(42L, '!');
    T to;

    to = from;

    EXPECT_EQ(to.first, 42);
    EXPECT_EQ(to.second, '!');
}

TEST(Pair, ConstAssignConvertLvalueConstRef)
{
    using T = pair<int&, int&>;
    using U = pair<long, char>;

    int a = 0;
    int b = 0;

    const T to(a, b);
    const U from(42L, '!');

    to = from;

    EXPECT_EQ(a, 42);
    EXPECT_EQ(b, '!');
}

TEST(Pair, AssignConvertRvalueRef)
{
    using T = pair<int, int>;
    using U = pair<long, char>;

    T to;
    U from(42L, '!');

    to = std::move(from);

    EXPECT_EQ(to.first, 42);
    EXPECT_EQ(to.second, '!');
}

TEST(Pair, ConstAssignConvertRvalueRef)
{
    using T = pair<int&, int&>;
    using U = pair<long, char>;

    int a = 0;
    int b = 0;

    const T to(a, b);
    U from(42L, '!');

    to = std::move(from);

    EXPECT_EQ(a, 42);
    EXPECT_EQ(b, '!');
}

TEST(Pair, AssignPairlike)
{
    using T = pair<int, int>;
    using U = tuple<int, int>;

    const U from(7, 42);
    T to;

    to = from;

    EXPECT_EQ(to.first, 7);
    EXPECT_EQ(to.second, 42);
}

TEST(Pair, ConstAssignPairlike)
{
    using T = pair<int&, int&>;
    using U = tuple<int, int>;

    int a = 0;
    int b = 0;

    const T to(a, b);
    const U from(7, 42);

    to = from;

    EXPECT_EQ(a, 7);
    EXPECT_EQ(b, 42);
}

TEST(Pair, Swap)
{
    struct nonmove { void operator=(nonmove&&) = delete; };
    using T = pair<int, int>;
    using U = pair<int, nonmove>;

    T one(1, 2);
    T two(3, 4);

    one.swap(two);

    EXPECT_EQ(one.first, 3);
    EXPECT_EQ(one.second, 4);
    EXPECT_EQ(two.first, 1);
    EXPECT_EQ(two.second, 2);

    EXPECT(!std::is_swappable_v<U>);
}

TEST(Pair, ConstSwap)
{
    using T = pair<int&, int&>;
    using U = pair<int, int>;

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

    EXPECT(!std::is_swappable_v<const U>);
}

TEST(Pair, DeductionGuide)
{
    using T = pair<int, int>;

    pair test(42, 7);

    EXPECT_SAME_TYPE(T, decltype(test));
}

TEST(Pair, Equality)
{
    using T = pair<int, int>;

    T one(1, 2);
    T two(1, 2);

    EXPECT(one == two);
}

TEST(Pair, Inequality)
{
    using T = pair<int, int>;

    T one(1, 2);
    T two(2, 1);

    EXPECT(one != two);
}

TEST(Pair, Comparison)
{
    using T = pair<int, int>;

    T one_zero(1, 0);
    T one_one(1, 1);
    T two_zero(2, 0);

    EXPECT(one_zero < one_one);
    EXPECT(one_zero < two_zero);
    EXPECT(one_one < two_zero);
}

TEST(Pair, MakePair)
{
    using T = pair<int, int>;

    auto test = make_pair(1, 2);

    EXPECT_SAME_TYPE(T, decltype(test));
    EXPECT_EQ(test.first, 1);
    EXPECT_EQ(test.second, 2);
}

TEST(Pair, TupleProtocol)
{
    using T = pair<int, long>;

    using T1 = tuple_element_t<0, T>;
    using T2 = tuple_element_t<1, T>;

    T t(1, 2);
    const T ct(3, 4);

    EXPECT_EQ(tuple_size_v<T>, 2);
    EXPECT_SAME_TYPE(T1, int);
    EXPECT_SAME_TYPE(T2, long);

    EXPECT_EQ(get<0>(t), 1);
    EXPECT_EQ(get<1>(t), 2);

    EXPECT_EQ(get<0>(ct), 3);
    EXPECT_EQ(get<1>(ct), 4);
    
    EXPECT_EQ(get<0>(std::move(t)), 1);
    EXPECT_EQ(get<1>(std::move(t)), 2);

    EXPECT_EQ(get<0>(std::move(ct)), 3);
    EXPECT_EQ(get<1>(std::move(ct)), 4);

    EXPECT_EQ(get<int>(t), 1);
    EXPECT_EQ(get<long>(t), 2);

    EXPECT_EQ(get<int>(ct), 3);
    EXPECT_EQ(get<long>(ct), 4);

    EXPECT_EQ(get<int>(std::move(t)), 1);
    EXPECT_EQ(get<long>(std::move(t)), 2);

    EXPECT_EQ(get<int>(std::move(ct)), 3);
    EXPECT_EQ(get<long>(std::move(ct)), 4);
}
