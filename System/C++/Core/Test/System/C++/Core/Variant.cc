#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Variant.hh>


using namespace __XVI_STD_CORE_NS;


//! @todo exception safety tests.
//! @todo noexcept tests.
//! @todo relational tests.
//! @todo visit tests.


TEST(Variant, DefaultConstructor)
{
    struct nondefault { nondefault() = delete; };
    using T = variant<int, nondefault>;
    using U = variant<nondefault, int>;

    T t;

    EXPECT_EQ(t.index(), 0);
    EXPECT_EQ(get<0>(t), 0);

    EXPECT(!std::is_default_constructible_v<U>);
}

TEST(Variant, CopyConstructor)
{
    struct nontrivial { nontrivial(const nontrivial&); };
    struct noncopy { noncopy(const noncopy&) = delete; };
    using T = variant<char, int, short>;
    using U = variant<nontrivial, int>;
    using V = variant<int, noncopy>;

    const T from(42);
    T to(from);

    EXPECT_EQ(to.index(), 1);
    EXPECT_EQ(get<1>(to), 42);

    EXPECT(std::is_trivially_copy_constructible_v<T>);
    EXPECT(!std::is_trivially_copy_constructible_v<U>);
    EXPECT(std::is_copy_constructible_v<U>);
    EXPECT(!std::is_copy_constructible_v<V>);
}

TEST(Variant, MoveConstructor)
{
    struct nontrivial { nontrivial(nontrivial&&); };
    struct nonmove { nonmove(nonmove&&) = delete; };
    using T = variant<short, char, int>;
    using U = variant<nontrivial, int>;
    using V = variant<int, nonmove>;

    T from(42);
    T to(std::move(from));

    EXPECT_EQ(to.index(), 2);
    EXPECT_EQ(get<2>(to), 42);

    EXPECT(std::is_trivially_move_constructible_v<T>);
    EXPECT(!std::is_trivially_move_constructible_v<U>);
    EXPECT(std::is_move_constructible_v<U>);
    EXPECT(!std::is_move_constructible_v<V>);
}

TEST(Variant, ConvertingConstructor)
{
    using T = variant<char, int, unsigned long long>;

    T a('1');
    T b(2);
    T c(3ULL);
    T d(short(5));

    EXPECT_EQ(a.index(), 0);
    EXPECT_EQ(b.index(), 1);
    EXPECT_EQ(c.index(), 2);
    EXPECT_EQ(d.index(), 1);

    // Would be ambiguous.
    EXPECT(!(std::is_constructible_v<T, long>));
}

TEST(Variant, InPlaceConstructor)
{
    struct data { int x; int y; int z; constexpr bool operator==(const data&) const = default; };
    using T = variant<int, data>;

    T a(in_place_type<int>, 42);
    T b(in_place_type<data>, 1, 2, 3);
    T c(in_place_index<0>, 7);
    T d(in_place_index<1>, -3, -2, -1);

    EXPECT_EQ(a.index(), 0);
    EXPECT_EQ(b.index(), 1);
    EXPECT_EQ(c.index(), 0);
    EXPECT_EQ(d.index(), 1);

    EXPECT_EQ(get<0>(a), 42);
    EXPECT_EQ(get<1>(b), (data{1, 2, 3}));
    EXPECT_EQ(get<0>(c), 7);
    EXPECT_EQ(get<1>(d), (data{-3, -2, -1}));
}

TEST(Variant, Destructor)
{
    struct nontrivial { ~nontrivial(); };
    using T = variant<int, char>;
    using U = variant<int, nontrivial>;

    EXPECT(std::is_trivially_destructible_v<T>);
    EXPECT(!std::is_trivially_destructible_v<U>);
    EXPECT(std::is_destructible_v<U>);
}

TEST(Variant, CopyAssignment)
{
    struct nontrivial { ~nontrivial(); };
    struct noncopy { noncopy(const noncopy&) = delete; };
    using T = variant<int, char>;
    using U = variant<nontrivial, int>;
    using V = variant<noncopy, int>;

    const T from(42);
    T to;

    to = from;

    EXPECT_EQ(to.index(), 0);
    EXPECT_EQ(get<0>(to), 42);

    EXPECT(std::is_trivially_copy_assignable_v<T>);
    EXPECT(!std::is_trivially_copy_assignable_v<U>);
    EXPECT(std::is_copy_assignable_v<U>);
    EXPECT(!std::is_copy_assignable_v<V>);
}

TEST(Variant, MoveAssignment)
{
    struct nontrivial { ~nontrivial(); };
    struct nonmove { nonmove(nonmove&&) = delete; };
    using T = variant<int, char>;
    using U = variant<nontrivial, int>;
    using V = variant<nonmove, int>;

    T from(42);
    T to;

    to = std::move(from);

    EXPECT_EQ(to.index(), 0);
    EXPECT_EQ(get<0>(to), 42);

    EXPECT(std::is_trivially_copy_assignable_v<T>);
    EXPECT(!std::is_trivially_copy_assignable_v<U>);
    EXPECT(std::is_copy_assignable_v<U>);
    EXPECT(!std::is_copy_assignable_v<V>);
}

TEST(Variant, ConvertingAssignment)
{
    using T = variant<char, int, unsigned long long>;

    T a;
    T b;
    T c;
    T d;

    a = '1';
    b = 2;
    c = 3ULL;
    d = short(4);

    EXPECT_EQ(a.index(), 0);
    EXPECT_EQ(b.index(), 1);
    EXPECT_EQ(c.index(), 2);
    EXPECT_EQ(d.index(), 1);

    // Would be ambiguous.
    EXPECT(!(std::is_assignable_v<T, long>));
}

TEST(Variant, Emplace)
{
    using T = variant<char, int, unsigned long>;

    T a;
    T b;
    T c;
    T d;
    T e;
    T f;

    a.emplace<char>('1');
    b.emplace<int>(2);
    c.emplace<unsigned long>(3UL);
    d.emplace<0>('4');
    e.emplace<1>(5);
    f.emplace<2>(6UL);

    EXPECT_EQ(a.index(), 0);
    EXPECT_EQ(b.index(), 1);
    EXPECT_EQ(c.index(), 2);
    EXPECT_EQ(d.index(), 0);
    EXPECT_EQ(e.index(), 1);
    EXPECT_EQ(f.index(), 2);

    EXPECT_EQ(get<0>(a), '1');
    EXPECT_EQ(get<1>(b), 2);
    EXPECT_EQ(get<2>(c), 3);
    EXPECT_EQ(get<0>(d), '4');
    EXPECT_EQ(get<1>(e), 5);
    EXPECT_EQ(get<2>(f), 6);
}

TEST(Variant, Valueless)
{
    struct make_valueless { operator int() { throw 0; } };
    using T = variant<char, int>;

    T valueless;
    
    EXPECT_THROW_ANY(valueless.emplace<1>(make_valueless{}));
    EXPECT(valueless.valueless_by_exception());
    EXPECT_EQ(valueless.index(), variant_npos);

    T valuelesscopy(valueless);
    EXPECT(valuelesscopy.valueless_by_exception());

    T valuelessmove(std::move(valueless));
    EXPECT(valuelessmove.valueless_by_exception());

    valuelesscopy = 1;
    valuelessmove = 1;

    EXPECT(!valuelesscopy.valueless_by_exception());
    EXPECT(!valuelessmove.valueless_by_exception());

    valuelesscopy = valueless;
    valuelessmove = std::move(valueless);

    EXPECT(valuelesscopy.valueless_by_exception());
    EXPECT(valuelessmove.valueless_by_exception());
}

TEST(Variant, Swap)
{
    using T = variant<int, char>;

    T a(1);
    T b('2');

    a.swap(b);

    EXPECT_EQ(a.index(), 1);
    EXPECT_EQ(b.index(), 0);

    EXPECT_EQ(get<1>(a), '2');
    EXPECT_EQ(get<0>(b), 1);
}

TEST(Variant, VariantProtocol)
{
    using T = variant<int, short, char>;

    EXPECT_EQ(variant_size_v<T>, 3);

    EXPECT_SAME_TYPE(variant_alternative_t<0, T>, int);
    EXPECT_SAME_TYPE(variant_alternative_t<2, const T>, const char);
}

TEST(Variant, HoldsAlternative)
{
    using T = variant<int, char, long>;

    T a(1);
    T b('2');
    T c(3L);

    EXPECT(holds_alternative<int>(a));
    EXPECT(holds_alternative<char>(b));
    EXPECT(holds_alternative<long>(c));
}

TEST(Variant, GetByType)
{
    using T = variant<int, char, long>;

    T a(1);
    T b('2');
    T c(3L);

    EXPECT_EQ(get<int>(a), 1);
    EXPECT_EQ(get<char>(b), '2');
    EXPECT_EQ(get<long>(c), 3);
}

TEST(Variant, GetIfByIndex)
{
    using T = variant<int, char, long>;

    const T t('t');

    EXPECT_EQ(get_if<0>(&t), nullptr);
    EXPECT_NE(get_if<1>(&t), nullptr);
    EXPECT_EQ(get_if<2>(&t), nullptr);
}

TEST(Variant, GetIfByType)
{
    using T = variant<int, char, long>;

    const T t('t');

    EXPECT_EQ(get_if<int>(&t), nullptr);
    EXPECT_NE(get_if<char>(&t), nullptr);
    EXPECT_EQ(get_if<long>(&t), nullptr);
}
