#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Expected.hh>


using namespace __XVI_STD_CORE_NS;


//! @todo exception tess
//! @todo expected<void, T> monadic method tests
//! @todo relational operator tests


TEST(Expected, UnexpectedDefaultConstructor)
{
    EXPECT(!std::is_default_constructible_v<unexpected<int>>);
}

TEST(Expected, UnexpectedCopyConstructor)
{
    struct noncopy { noncopy(const noncopy&) = delete; };
    struct nontrivial { nontrivial(const nontrivial&); };
    using T = unexpected<int>;
    using U = unexpected<nontrivial>;
    using V = unexpected<noncopy>;

    const T from(42);
    T to(from);

    EXPECT_EQ(to.error(), 42);

    EXPECT(std::is_trivially_copy_constructible_v<T>);
    EXPECT(!std::is_trivially_copy_constructible_v<U>);
    EXPECT(std::is_copy_constructible_v<U>);
    EXPECT(!std::is_copy_constructible_v<V>);
}

TEST(Expected, UnexpectedMoveConstructor)
{
    struct nonmove { nonmove(nonmove&&) = delete; };
    struct nontrivial { nontrivial(nontrivial&&); };
    using T = unexpected<int>;
    using U = unexpected<nontrivial>;
    using V = unexpected<nonmove>;

    T from(42);
    T to(std::move(from));

    EXPECT_EQ(to.error(), 42);
    EXPECT_EQ(from.error(), 42);

    EXPECT(std::is_trivially_move_constructible_v<T>);
    EXPECT(!std::is_trivially_move_constructible_v<U>);
    EXPECT(std::is_move_constructible_v<U>);
    EXPECT(!std::is_move_constructible_v<V>);
}

TEST(Expected, UnexpectedConvertingConstructor)
{
    using T = unexpected<int>;

    T t(42UL);

    EXPECT_EQ(t.error(), 42);
}

TEST(Expected, UnexpectedInPlaceConstructor)
{
    struct data { int x; int y; int z; constexpr bool operator==(const data&) const = default; };
    using T = unexpected<data>;

    T t(in_place, 1, 2, 3);

    EXPECT_EQ(t.error(), (data{1, 2, 3}));
}

TEST(Expected, UnexpectedCopyAssignment)
{
    struct noncopy { void operator=(const noncopy&) = delete; };
    struct nontrivial { nontrivial& operator=(const nontrivial&); };
    using T = unexpected<int>;
    using U = unexpected<nontrivial>;
    using V = unexpected<noncopy>;

    const T from(42);
    T to(0);

    to = from;

    EXPECT_EQ(to.error(), 42);

    EXPECT(std::is_trivially_copy_assignable_v<T>);
    EXPECT(!std::is_trivially_copy_assignable_v<U>);
    EXPECT(std::is_copy_assignable_v<U>);
    EXPECT(!std::is_copy_assignable_v<V>);
}

TEST(Expected, UnexpectedMoveAssignment)
{
    struct nonmove { void operator=(nonmove&&) = delete; };
    struct nontrivial { nontrivial& operator=(nontrivial&&); };
    using T = unexpected<int>;
    using U = unexpected<nontrivial>;
    using V = unexpected<nonmove>;

    T from(42);
    T to(0);

    to = std::move(from);

    EXPECT_EQ(to.error(), 42);
    EXPECT_EQ(from.error(), 42);

    EXPECT(std::is_trivially_move_assignable_v<T>);
    EXPECT(!std::is_trivially_move_assignable_v<U>);
    EXPECT(std::is_move_assignable_v<U>);
    EXPECT(!std::is_move_assignable_v<V>);
}

TEST(Expected, UnexpectedSwap)
{
    using T = unexpected<int>;

    T a(1);
    T b(2);

    a.swap(b);

    EXPECT_EQ(a.error(), 2);
    EXPECT_EQ(b.error(), 1);
}

TEST(Expected, UnexpectedEquality)
{
    using T = unexpected<int>;

    const T a(1);
    const T b(2);
    const T c(1);

    EXPECT_EQ(a, c);
    EXPECT_NE(a, b);
}

TEST(Expected, UnexpectedDeductionGuide)
{
    unexpected u(42);

    EXPECT_SAME_TYPE(decltype(u), unexpected<int>);
}

TEST(Expected, BadExpectedAccessConstructor)
{
    using T = bad_expected_access<int>;

    T err(42);

    EXPECT_EQ(err.error(), 42);
}

TEST(Expected, ExpectedTypes)
{
    using T = expected<int, char>;
    using U = expected<long, char>;

    EXPECT_SAME_TYPE(T::value_type, int);
    EXPECT_SAME_TYPE(T::error_type, char);
    EXPECT_SAME_TYPE(T::unexpected_type, unexpected<char>);

    EXPECT_SAME_TYPE(T::template rebind<long>, U);
}

TEST(Expected, ExpectedDefaultConstructor)
{
    struct nondefault { nondefault() = delete; };
    using T = expected<int, char>;
    using U = expected<nondefault, int>;

    T t;

    EXPECT(t.has_value());
    EXPECT_EQ(t.value(), 0);

    EXPECT(!(std::is_default_constructible_v<U>));
}

TEST(Expected, ExpectedCopyConstructor)
{
    struct nontrivial { nontrivial(const nontrivial&); };
    struct noncopy { noncopy(const noncopy&) = delete; };
    using T = expected<int, char>;
    using U = expected<nontrivial, int>;
    using V = expected<noncopy, int>;

    const T from(42);
    T to(from);

    EXPECT(to.has_value());
    EXPECT_EQ(to.value(), 42);

    EXPECT(std::is_trivially_copy_constructible_v<T>);
    EXPECT(!std::is_trivially_copy_constructible_v<U>);
    EXPECT(std::is_copy_constructible_v<U>);
    EXPECT(!std::is_copy_constructible_v<V>);
}

TEST(Expected, ExpectedMoveConstructor)
{
    struct nontrivial { nontrivial(nontrivial&&); };
    struct nonmove { nonmove(nonmove&&) = delete; };
    using T = expected<int, char>;
    using U = expected<nontrivial, int>;
    using V = expected<nonmove, int>;

    T from(42);
    T to(std::move(from));

    EXPECT(to.has_value());
    EXPECT_EQ(to.value(), 42);

    EXPECT(std::is_trivially_move_constructible_v<T>);
    EXPECT(!std::is_trivially_move_constructible_v<U>);
    EXPECT(std::is_move_constructible_v<U>);
    EXPECT(!std::is_move_constructible_v<V>);
}

TEST(Expected, ExpectedConvertingCopyConstructor)
{
    using T = expected<int, char>;
    using U = expected<char, int>;

    const T a(42);
    const T b(unexpected('!'));

    U c(a);
    U d(b);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.value(), 42);
    EXPECT_EQ(d.error(), '!');
}

TEST(Expected, ExpectedConvertingMoveConstructor)
{
    using T = expected<int, char>;
    using U = expected<char, int>;

    T a(42);
    T b(unexpected('!'));

    U c(std::move(a));
    U d(std::move(b));

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.value(), 42);
    EXPECT_EQ(d.error(), '!');
}

TEST(Expected, ExpectedConvertingConstructor)
{
    using T = expected<int, char>;

    T t(42ULL);

    EXPECT(t.has_value());
    EXPECT_EQ(t.value(), 42);
}

TEST(Expected, ExpectedUnexpectedCopyConstructor)
{
    using T = expected<int, char>;

    const auto u = unexpected<int>(42);
    const auto v = unexpected<unsigned long>(7);

    T a(u);
    T b(v);

    EXPECT(!a.has_value());
    EXPECT(!b.has_value());

    EXPECT_EQ(a.error(), 42);
    EXPECT_EQ(b.error(), 7);
}

TEST(Expected, ExpectedUnexpectedMoveConstructor)
{
    using T = expected<int, char>;

    auto u = unexpected<int>(42);
    auto v = unexpected<unsigned long>(7);

    T a(std::move(u));
    T b(std::move(v));

    EXPECT(!a.has_value());
    EXPECT(!b.has_value());

    EXPECT_EQ(a.error(), 42);
    EXPECT_EQ(b.error(), 7);
}

TEST(Expected, ExpectedInPlaceConstructor)
{
    struct data { int x; int y; int z; constexpr bool operator==(const data&) const = default; };
    using T = expected<data, int>;

    T t(in_place, 1, 2, 3);

    EXPECT(t.has_value());
    EXPECT_EQ(t.value(), (data{1, 2, 3}));
}

TEST(Expected, ExpectedUnexpectConstructor)
{
    struct data { int x; int y; int z; constexpr bool operator==(const data&) const = default; };
    using T = expected<int, data>;

    T t(unexpect, 1, 2, 3);

    EXPECT(!t.has_value());
    EXPECT_EQ(t.error(), (data{1, 2, 3}));
}

TEST(Expected, ExpectedDestructor)
{
    struct nontrivial { ~nontrivial(); };
    using T = expected<int, char>;
    using U = expected<int, nontrivial>;
    using V = expected<nontrivial, int>;

    EXPECT(std::is_trivially_destructible_v<T>);
    EXPECT(!std::is_trivially_destructible_v<U>);
    EXPECT(!std::is_trivially_destructible_v<V>);
    EXPECT(std::is_destructible_v<U>);
    EXPECT(std::is_destructible_v<V>);
}

TEST(Expected, ExpectedCopyAssignmant)
{
    struct nontrivial { nontrivial& operator=(const nontrivial&); };
    struct noncopy { void operator=(const noncopy&) = delete; };
    using T = expected<int, char>;
    using U = expected<nontrivial, int>;
    using V = expected<noncopy, int>;

    const T a(42);
    const T b(unexpected('!'));

    T c(7);
    T d(7);
    T e(unexpected('?'));
    T f(unexpected('?'));

    c = a;
    d = b;
    e = a;
    f = b;

    EXPECT(c.has_value());
    EXPECT(!d.has_value());
    EXPECT(e.has_value());
    EXPECT(!f.has_value());

    EXPECT_EQ(c.value(), 42);
    EXPECT_EQ(d.error(), '!');
    EXPECT_EQ(e.value(), 42);
    EXPECT_EQ(f.error(), '!');

    EXPECT(std::is_trivially_copy_assignable_v<T>);
    EXPECT(!std::is_trivially_copy_assignable_v<U>);
    EXPECT(std::is_copy_assignable_v<U>);
    EXPECT(!std::is_copy_assignable_v<V>);
}

TEST(Expected, ExpectedMoveAssignmant)
{
    struct nontrivial { nontrivial(nontrivial&&); nontrivial& operator=(nontrivial&&); };
    struct nonmove { void operator=(nonmove&&) = delete; };
    using T = expected<int, char>;
    using U = expected<nontrivial, int>;
    using V = expected<nonmove, int>;

    T a(42);
    T b(unexpected('!'));

    T c(7);
    T d(7);
    T e(unexpected('?'));
    T f(unexpected('?'));

    c = std::move(a);
    d = std::move(b);
    e = std::move(a);
    f = std::move(b);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());
    EXPECT(e.has_value());
    EXPECT(!f.has_value());

    EXPECT_EQ(c.value(), 42);
    EXPECT_EQ(d.error(), '!');
    EXPECT_EQ(e.value(), 42);
    EXPECT_EQ(f.error(), '!');

    EXPECT(std::is_trivially_move_assignable_v<T>);
    EXPECT(!std::is_trivially_move_assignable_v<U>);
    EXPECT(std::is_move_assignable_v<U>);
    EXPECT(!std::is_move_assignable_v<V>);
}

TEST(Expected, ExpectedConvertingAssignment)
{
    using T = expected<int, char>;

    T a(1);
    T b(unexpected('?'));

    a = 42ULL;
    b = 7;

    EXPECT(a.has_value());
    EXPECT(b.has_value());
    EXPECT_EQ(a.value(), 42);
    EXPECT_EQ(b.value(), 7);
}

TEST(Expected, ExpectUnexpectedCopyAssignment)
{
    using T = expected<int, char>;

    T a(1);
    T b(unexpected('?'));

    const auto x = unexpected<int>(42);
    const auto y = unexpected<char>('!');

    a = x;
    b = y;

    EXPECT(!a.has_value());
    EXPECT(!b.has_value());
    EXPECT_EQ(a.error(), 42);
    EXPECT_EQ(b.error(), '!');
}

TEST(Expected, ExpectUnexpectedMoveAssignment)
{
    using T = expected<int, char>;

    T a(1);
    T b(unexpected('?'));

    auto x = unexpected<int>(42);
    auto y = unexpected<char>('!');

    a = std::move(x);
    b = std::move(y);

    EXPECT(!a.has_value());
    EXPECT(!b.has_value());
    EXPECT_EQ(a.error(), 42);
    EXPECT_EQ(b.error(), '!');
}

TEST(Expected, ExpectedEmplace)
{
    using T = expected<int, char>;

    T a(1);
    T b(unexpected('?'));

    a.emplace(7);
    b.emplace(42);

    EXPECT(a.has_value());
    EXPECT(b.has_value());
    EXPECT_EQ(a.value(), 7);
    EXPECT_EQ(b.value(), 42);
}

TEST(Expected, ExpectedSwap)
{
    using T = expected<int, char>;

    T a(1);
    T b(unexpected('?'));

    a.swap(b);

    EXPECT(!a.has_value());
    EXPECT(b.has_value());
    EXPECT_EQ(a.error(), '?');
    EXPECT_EQ(b.value(), 1);

    a.swap(b);

    EXPECT(a.has_value());
    EXPECT(!b.has_value());
    EXPECT_EQ(a.value(), 1);
    EXPECT_EQ(b.error(), '?');
}

TEST(Expected, ExpectedOperatorArrow)
{
    using T = expected<int, char>;

    T t(42);
    const T u;

    EXPECT_NE(t.operator->(), nullptr);
    EXPECT_SAME_TYPE(decltype(t.operator->()), int*);
    EXPECT_SAME_TYPE(decltype(u.operator->()), const int*);
}

TEST(Expected, ExpectedOperatorStar)
{
    using T = expected<int, char>;

    T t(42);
    const T u;

    EXPECT_EQ(*t, 42);
    EXPECT_SAME_TYPE(decltype(*t), int&);
    EXPECT_SAME_TYPE(decltype(*u), const int&);
    EXPECT_SAME_TYPE(decltype(*std::move(t)), int&&);
    EXPECT_SAME_TYPE(decltype(*std::move(u)), const int&&);
}

TEST(Expected, ExpectedOperatorBool)
{
    using T = expected<int, char>;

    const T a(42);
    const T b(unexpected('!'));

    EXPECT(bool(a));
    EXPECT(!bool(b));
}

TEST(Expected, ExpectedValueOr)
{
    using T = expected<int, char>;

    const T a(42);
    const T b(unexpected('!'));

    EXPECT_EQ(a.value_or(7), 42);
    EXPECT_EQ(b.value_or(7), 7);
}

TEST(Expected, ExpectedErrorOr)
{
    using T = expected<int, char>;

    const T a(42);
    const T b(unexpected('!'));

    EXPECT_EQ(a.error_or('?'), '?');
    EXPECT_EQ(b.error_or('?'), '!');
}

TEST(Expected, ExpectedAndThen)
{
    using T = expected<int, char>;

    const T a(42);
    const T b(unexpected('!'));

    auto fn = [](int x) -> expected<int, char> { return x + 1; };

    auto c = a.and_then(fn);
    auto d = b.and_then(fn);

    EXPECT_SAME_TYPE(decltype(c), T);
    EXPECT_SAME_TYPE(decltype(d), T);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.value(), 43);
    EXPECT_EQ(d.error(), '!');
}

TEST(Expected, ExpectedOrElse)
{
    using T = expected<int, char>;

    const T a(42);
    const T b(unexpected('A'));

    auto fn = [](char x) -> expected<int, char> { return unexpected(x + 0x20); };

    auto c = a.or_else(fn);
    auto d = b.or_else(fn);

    EXPECT_SAME_TYPE(decltype(c), T);
    EXPECT_SAME_TYPE(decltype(d), T);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.value(), 42);
    EXPECT_EQ(d.error(), 'a');
}

TEST(Expected, ExpectedTransform)
{
    using T = expected<int, char>;
    using U = expected<long, char>;

    const T a(42);
    const T b(unexpected('!'));

    auto fn = [](int x) -> long { return x + 1L; };

    auto c = a.transform(fn);
    auto d = b.transform(fn);

    EXPECT_SAME_TYPE(decltype(c), U);
    EXPECT_SAME_TYPE(decltype(d), U);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.value(), 43L);
    EXPECT_EQ(d.error(), '!');
}

TEST(Expected, ExpectedTransformError)
{
    using T = expected<int, char>;
    using U = expected<int, int>;

    const T a(42);
    const T b(unexpected('!'));

    auto fn = [](char x) -> int { return x + 2; };

    auto c = a.transform_error(fn);
    auto d = b.transform_error(fn);

    EXPECT_SAME_TYPE(decltype(c), U);
    EXPECT_SAME_TYPE(decltype(d), U);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.value(), 42);
    EXPECT_EQ(d.error(), '#');
}

TEST(Expected, ExpectedVoidTypes)
{
    using T = expected<void, char>;
    using U = expected<int, char>;

    EXPECT_SAME_TYPE(T::value_type, void);
    EXPECT_SAME_TYPE(T::error_type, char);
    EXPECT_SAME_TYPE(T::unexpected_type, unexpected<char>);

    EXPECT_SAME_TYPE(typename T::template rebind<int>, U);
}

TEST(Expected, ExpectedVoidDefaultConstructor)
{
    using T = expected<void, int>;

    T t;

    EXPECT(t.has_value());
}

TEST(Expected, ExpectedVoidCopyConstructor)
{
    struct noncopy { noncopy(const noncopy&) = delete; };
    struct nontrivial { nontrivial(const nontrivial&); };

    using T = expected<void, int>;
    using U = expected<void, nontrivial>;
    using V = expected<void, noncopy>;

    const T a;
    const T b(unexpected(7));

    T c(a);
    T d(b);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(d.error(), 7);

    EXPECT(std::is_trivially_copy_constructible_v<T>);
    EXPECT(!std::is_trivially_copy_constructible_v<U>);
    EXPECT(std::is_copy_constructible_v<U>);
    EXPECT(!std::is_copy_constructible_v<V>);
}

TEST(Expected, ExpectedVoidMoveConstructor)
{
    struct nonmove { nonmove(nonmove&&) = delete; };
    struct nontrivial { nontrivial(nontrivial&&); };

    using T = expected<void, int>;
    using U = expected<void, nontrivial>;
    using V = expected<void, nonmove>;

    T a;
    T b(unexpected(7));

    T c(std::move(a));
    T d(std::move(b));

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(d.error(), 7);

    EXPECT(std::is_trivially_move_constructible_v<T>);
    EXPECT(!std::is_trivially_move_constructible_v<U>);
    EXPECT(std::is_move_constructible_v<U>);
    EXPECT(!std::is_move_constructible_v<V>);
}

TEST(Expected, ExpectedVoidConvertingCopyConstructor)
{
    using T = expected<void, int>;
    using U = expected<void, long>;
    using V = expected<int, char>;

    const T a;
    const T b(unexpected('!'));

    U c(a);
    U d(b);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(d.error(), '!');

    EXPECT(!(std::is_constructible_v<T, const V&>));
}

TEST(Expected, ExpectedVoidConvertingMoveConstructor)
{
    using T = expected<void, int>;
    using U = expected<void, long>;
    using V = expected<int, char>;

    T a;
    T b(unexpected('!'));

    U c(std::move(a));
    U d(std::move(b));

    EXPECT(c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(d.error(), '!');

    EXPECT(!(std::is_constructible_v<T, V>));
}

TEST(Expected, ExpectedVoidUnexpectedCopyConstructor)
{
    using T = expected<void, int>;

    const auto a = unexpected('7');
    const auto b = unexpected(42ULL);

    T c(a);
    T d(b);

    EXPECT(!c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.error(), '7');
    EXPECT_EQ(d.error(), 42);
}

TEST(Expected, ExpectedVoidUnexpectedMoveConstructor)
{
    using T = expected<void, int>;

    auto a = unexpected('7');
    auto b = unexpected(42ULL);

    T c(std::move(a));
    T d(std::move(b));

    EXPECT(!c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.error(), '7');
    EXPECT_EQ(d.error(), 42);
}

TEST(Expected, ExpectedVoidInPlaceConstructor)
{
    using T = expected<void, int>;

    T t(in_place);

    EXPECT(t.has_value());
}

TEST(Expected, ExpectedVoidUnexpectConstructor)
{
    struct data { int x; int y; int z; constexpr bool operator==(const data&) const = default; };
    using T = expected<void, data>;

    T t(unexpect, 1, 2, 3);

    EXPECT(!t.has_value());

    EXPECT_EQ(t.error(), (data{1, 2, 3}));
}

TEST(Expected, ExpectedVoidDestructor)
{
    struct nontrivial { ~nontrivial(); };
    struct nondestruct { ~nondestruct() = delete; };
    using T = expected<void, int>;
    using U = expected<void, nontrivial>;

    EXPECT(std::is_trivially_destructible_v<T>);
    EXPECT(!std::is_trivially_destructible_v<U>);
    EXPECT(std::is_destructible_v<U>);
}

TEST(Expected, ExpectedVoidCopyAssignment)
{
    struct nontrivial { nontrivial& operator=(const nontrivial&); };
    struct noncopy { void operator=(const noncopy&) = delete; };

    using T = expected<void, int>;
    using U = expected<void, nontrivial>;
    using V = expected<void, noncopy>;

    const T a;
    const T b(unexpected('!'));

    T c;
    T d;
    T e(unexpected('?'));
    T f(unexpected('?'));

    c = a;
    d = b;
    e = a;
    f = b;

    EXPECT(c.has_value());
    EXPECT(!d.has_value());
    EXPECT(e.has_value());
    EXPECT(!f.has_value());

    EXPECT_EQ(d.error(), '!');
    EXPECT_EQ(f.error(), '!');

    EXPECT(std::is_trivially_copy_assignable_v<T>);
    EXPECT(!std::is_trivially_copy_assignable_v<U>);
    EXPECT(std::is_copy_assignable_v<U>);
    EXPECT(!std::is_copy_assignable_v<V>);
}

TEST(Expected, ExpectedVoidMoveAssignment)
{
    struct nontrivial { nontrivial& operator=(const nontrivial&); };
    struct nonmove { void operator=(nonmove&&) = delete; };

    using T = expected<void, int>;
    using U = expected<void, nontrivial>;
    using V = expected<void, nonmove>;

    T a;
    T b(unexpected('!'));

    T c;
    T d;
    T e(unexpected('?'));
    T f(unexpected('?'));

    c = std::move(a);
    d = std::move(b);
    e = std::move(a);
    f = std::move(b);

    EXPECT(c.has_value());
    EXPECT(!d.has_value());
    EXPECT(e.has_value());
    EXPECT(!f.has_value());

    EXPECT_EQ(d.error(), '!');
    EXPECT_EQ(f.error(), '!');

    EXPECT(std::is_trivially_move_assignable_v<T>);
    EXPECT(!std::is_trivially_move_assignable_v<U>);
    EXPECT(std::is_move_assignable_v<U>);
    EXPECT(!std::is_move_assignable_v<V>);
}

TEST(Expected, ExpectedVoidUnexpectedCopyAssignment)
{
    using T = expected<void, int>;

    const auto a = unexpected('7');
    const auto b = unexpected(42ULL);

    T c;
    T d(unexpected(3));

    c = a;
    d = b;

    EXPECT(!c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.error(), '7');
    EXPECT_EQ(d.error(), 42);
}

TEST(Expected, ExpectedVoidUnexpectedMoveAssignment)
{
    using T = expected<void, int>;

    auto a = unexpected('7');
    auto b = unexpected(42ULL);

    T c;
    T d(unexpected(3));

    c = std::move(a);
    d = std::move(b);

    EXPECT(!c.has_value());
    EXPECT(!d.has_value());

    EXPECT_EQ(c.error(), '7');
    EXPECT_EQ(d.error(), 42);
}

TEST(Expected, ExpectedVoidEmplace)
{
    using T = expected<void, int>;

    T a;
    T b(unexpected('?'));

    a.emplace();
    b.emplace();

    EXPECT(a.has_value());
    EXPECT(b.has_value());
}

TEST(Expected, ExpectedVoidSwap)
{
    using T = expected<void, int>;

    T a;
    T b(unexpected('!'));

    a.swap(b);

    EXPECT(!a.has_value());
    EXPECT(b.has_value());

    EXPECT_EQ(a.error(), '!');

    a.swap(b);

    EXPECT(a.has_value());
    EXPECT(!b.has_value());

    EXPECT_EQ(b.error(), '!');
}

TEST(Expected, ExpectedVoidErrorOr)
{
    using T = expected<void, char>;

    const T a;
    const T b(unexpected('!'));

    EXPECT_EQ(a.error_or('?'), '?');
    EXPECT_EQ(b.error_or('?'), '!');
}
