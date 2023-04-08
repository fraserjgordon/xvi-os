#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Optional.hh>


using namespace __XVI_STD_CORE_NS;


#if defined(__clang__)
TEST(Optional, DefaultConstructor)
{
    EXPECT(!"Clang bug - no support for constexpr unions with non-literal members.");
}
#else
TEST(Optional, DefaultConstructor)
{
    struct nontrivial { nontrivial(); };
    struct nondefault { nondefault() = delete; };

    using T = optional<int>;
    using U = optional<nontrivial>;
    using V = optional<nondefault>;

    // Note: constructor is required to be constexpr.
    constexpr T t;
    constexpr U u;
    constexpr V v;

    EXPECT(!t.has_value());
    EXPECT(!u.has_value());
    EXPECT(!v.has_value());
}
#endif

TEST(Optional, NulloptConstructor)
{
    using T = optional<int>;

    T a(nullopt);

    EXPECT(!a.has_value());
}

TEST(Optional, CopyConstructor)
{
    struct nontrivial { nontrivial(const nontrivial&); };
    struct noncopy { noncopy(const noncopy&) = delete; };

    using T = optional<int>;
    using U = optional<nontrivial>;
    using V = optional<noncopy>;

    const T a(nullopt);
    const T b(42);

    T c(a);
    T d(b);

    EXPECT(!a.has_value());
    EXPECT(b.has_value());
    EXPECT(!c.has_value());
    EXPECT(d.has_value());

    EXPECT_EQ(b.value(), 42);
    EXPECT_EQ(d.value(), 42);

    EXPECT(std::is_trivially_copy_constructible_v<T>);
    EXPECT(!std::is_trivially_copy_constructible_v<U>);
    EXPECT(std::is_copy_constructible_v<U>);
    EXPECT(!std::is_copy_constructible_v<V>);
}

TEST(Optional, MoveConstructor)
{
    struct nontrivial { nontrivial(nontrivial&&); };
    struct nonmove { nonmove(nonmove&&) = delete; };

    using T = optional<int>;
    using U = optional<nontrivial>;
    using V = optional<nonmove>;

    T a(nullopt);
    T b(42);

    T c(std::move(a));
    T d(std::move(b));

    EXPECT(!a.has_value());
    EXPECT(b.has_value());
    EXPECT(!c.has_value());
    EXPECT(d.has_value());

    EXPECT_EQ(d.value(), 42);

    EXPECT(std::is_trivially_move_constructible_v<T>);
    EXPECT(!std::is_trivially_move_constructible_v<U>);
    EXPECT(std::is_move_constructible_v<U>);
    EXPECT(!std::is_move_constructible_v<V>);
}

TEST(Optional, InPlaceConstructor)
{
    struct data { int x; int y; int z; constexpr bool operator==(const data&) const = default; };

    using T = optional<int>;
    using U = optional<data>;

    constexpr T t(in_place, 42);
    constexpr U u(in_place, 1, 2, 3);

    EXPECT(t.has_value());
    EXPECT(u.has_value());

    EXPECT_EQ(t.value(), 42);
    EXPECT_EQ(u.value(), (data{1, 2, 3}));

    EXPECT((std::is_constructible_v<U, in_place_t, int, int>));
    EXPECT(!(std::is_constructible_v<T, in_place_t, std::nullptr_t>));
    EXPECT(!(std::is_constructible_v<U, in_place_t, int, int, int, int>));
}

TEST(Optional, InPlaceInitializerConstructor)
{
    struct complicated { constexpr complicated(std::initializer_list<int>, char, long) {} };
    using T = optional<complicated>;

    constexpr T t(in_place, {1, 2, 3, 4, 5}, '!', 42L);

    EXPECT(t.has_value());
}

TEST(Optional, ConvertingConstructor)
{
    struct nonimplicit { explicit nonimplicit(int); explicit operator int(); };
    using T = optional<int>;
    using U = optional<nonimplicit>;

    T t(42ULL);

    EXPECT(t.has_value());
    EXPECT_EQ(t.value(), 42);

    EXPECT((std::is_constructible_v<T, nonimplicit>));
    EXPECT((std::is_constructible_v<U, int>));
    EXPECT(!(__detail::__is_implicit_constructible_v<T, nonimplicit>));
    EXPECT(!(__detail::__is_implicit_constructible_v<U, int>));
}

TEST(Optional, ConvertingOptionalCopyConstructor)
{
    struct fromopt { fromopt(const optional<int>&) {} };
    using T = optional<fromopt>;
    using U = optional<int>;
    using V = optional<short>;

    // This should use an implicit conversion and not the templated optional<U> constructor.
    const U from{};
    T t(from);
    EXPECT(t.has_value());

    V v(from);
    EXPECT(!v.has_value());
}

TEST(Optional, ConvertingOptionalMoveConstructor)
{
    struct fromopt { fromopt(optional<int>) {} };
    using T = optional<fromopt>;
    using U = optional<int>;
    using V = optional<short>;

    // This should use an implicit conversion and not the templated optional<U> constructor.
    T t(U{});
    EXPECT(t.has_value());

    V v(U{});
    EXPECT(!v.has_value());
}

TEST(Optional, Destructor)
{
    struct nontrivial { ~nontrivial(); };
    using T = optional<int>;
    using U = optional<nontrivial>;

    EXPECT(std::is_trivially_destructible_v<T>);
    EXPECT(!std::is_trivially_destructible_v<U>);
    EXPECT(std::is_destructible_v<U>);
}

TEST(Optional, NulloptAssignment)
{
    using T = optional<int>;

    T t(42);
    t = nullopt;

    EXPECT(!t.has_value());
}

TEST(Optional, CopyAssignment)
{
    struct nocopyctor { nocopyctor(const nocopyctor&) = delete; };
    struct nocopyassign { void operator=(const nocopyassign&) = delete; };

    struct nontrivialcopyctor { nontrivialcopyctor(const nontrivialcopyctor&); };
    struct nontrivialcopyassign { nontrivialcopyassign& operator=(const nontrivialcopyassign&); };
    struct nontrivialdestruct { ~nontrivialdestruct(); };

    using T = optional<int>;
    using U = optional<nocopyctor>;
    using V = optional<nocopyassign>;
    using W = optional<nontrivialcopyctor>;
    using X = optional<nontrivialcopyassign>;
    using Y = optional<nontrivialdestruct>;

    EXPECT(!std::is_copy_assignable_v<U>);
    EXPECT(!std::is_copy_assignable_v<V>);

    EXPECT(std::is_copy_assignable_v<W>);
    EXPECT(std::is_copy_assignable_v<X>);
    EXPECT(std::is_copy_assignable_v<Y>);

    EXPECT(!std::is_trivially_copy_assignable_v<W>);
    EXPECT(!std::is_trivially_copy_assignable_v<X>);
    EXPECT(!std::is_trivially_copy_assignable_v<Y>);

    const T from(42);
    T to;

    to = from;

    EXPECT(to.has_value());
    EXPECT(from.has_value());
    EXPECT_EQ(to.value(), 42);
}

TEST(Optional, CopyAssignmentExceptions)
{
    struct throwing
    {
        int x;
        bool ctorthrow;
        bool assignthrow;

        throwing(int a, bool b, bool c)
            : x(a), ctorthrow(b), assignthrow(c)
        {
        }

        throwing(const throwing& from)
            : x(from.x), ctorthrow(from.ctorthrow), assignthrow(from.assignthrow)
        {
            if (ctorthrow)
                throw x;
        }

        throwing& operator=(const throwing& from)
        {
            x = from.x;
            ctorthrow = from.ctorthrow;
            assignthrow = from.assignthrow;

            if (assignthrow)
                throw x;

            return *this;
        }
    };

    using T = optional<throwing>;

    T a;
    T b(in_place, 42, true, true);

    EXPECT_THROW_ANY(a = b);
    EXPECT(!a.has_value());

    T c(in_place, 7, false, false);

    // Note: value of c->x depends on exception safety guarantee of struct throwing.
    EXPECT_THROW_ANY(c = b);
    EXPECT(c.has_value());
    EXPECT_EQ(c->x, 42);
}

TEST(Optional, MoveAssignment)
{
    struct nomovector { nomovector(nomovector&&) = delete; };
    struct nomoveassign { void operator=(nomoveassign&&) = delete; };

    struct nontrivialmove { nontrivialmove(nontrivialmove&&); nontrivialmove& operator=(nontrivialmove&&); };
    struct nontrivialdestruct { ~nontrivialdestruct(); };

    using T = optional<int>;
    using U = optional<nomovector>;
    using V = optional<nomoveassign>;
    using X = optional<nontrivialmove>;
    using Y = optional<nontrivialdestruct>;

    EXPECT(!std::is_move_assignable_v<U>);
    EXPECT(!std::is_move_assignable_v<V>);

    EXPECT(std::is_move_assignable_v<X>);
    EXPECT(std::is_move_assignable_v<Y>);

    EXPECT(!std::is_trivially_move_assignable_v<X>);
    EXPECT(!std::is_trivially_move_assignable_v<Y>);

    T from(42);
    T to;

    to = std::move(from);

    EXPECT(to.has_value());
    EXPECT(from.has_value());
    EXPECT_EQ(to.value(), 42);
}

TEST(Optional, MoveAssignmentExceptions)
{
    struct throwing
    {
        int x;
        bool ctorthrow;
        bool assignthrow;

        throwing(int a, bool b, bool c)
            : x(a), ctorthrow(b), assignthrow(c)
        {
        }

        throwing(throwing&& from)
            : x(from.x), ctorthrow(from.ctorthrow), assignthrow(from.assignthrow)
        {
            if (ctorthrow)
                throw x;
        }

        throwing& operator=(throwing&& from)
        {
            x = from.x;
            ctorthrow = from.ctorthrow;
            assignthrow = from.assignthrow;

            if (assignthrow)
                throw x;

            return *this;
        }
    };

    using T = optional<throwing>;

    T a;
    T b(in_place, 42, true, true);

    EXPECT_THROW_ANY(a = std::move(b));
    EXPECT(!a.has_value());

    T c(in_place, 7, false, false);

    // Note: value of c->x depends on exception safety guarantee of struct throwing.
    EXPECT_THROW_ANY(c = std::move(b));
    EXPECT(c.has_value());
    EXPECT_EQ(c->x, 42);
}

TEST(Optional, ConvertingAssignment)
{
    using T = optional<int>;

    T t;
    t = 42ULL;

    EXPECT(t.has_value());
    EXPECT_EQ(t.value(), 42);
}

TEST(Optional, ConvertingOptionalCopyAssignment)
{
    struct converting { operator int() const; };
    using T = optional<int>;
    using U = optional<unsigned long long>;
    using V = optional<converting>;

    const U from(42ULL);
    T to;

    to = from;

    EXPECT(to.has_value());
    EXPECT_EQ(to.value(), 42);

    EXPECT((std::is_assignable_v<T&, const V&>));
}

TEST(Optional, ConveringOptionMoveAssignment)
{
    struct converting { operator int() &&; };
    using T = optional<int>;
    using U = optional<unsigned long long>;
    using V = optional<converting>;

    U from(42ULL);
    T to;

    to = std::move(from);

    EXPECT(to.has_value());
    EXPECT_EQ(to.value(), 42);

    EXPECT((std::is_assignable_v<T&, V>)); 
}

TEST(Optional, Emplace)
{
    struct data { int x; int y; int z; constexpr bool operator==(const data&) const = default; };
    using T = optional<data>;

    T t;
    t.emplace(1, 2, 3);

    EXPECT(t.has_value());
    EXPECT_EQ(t.value(), (data{1, 2, 3}));
}

TEST(Optional, Swap)
{
    using T = optional<int>;

    T one(42);
    T two;

    two.swap(one);

    EXPECT(two.has_value());
    EXPECT(!one.has_value());
    EXPECT_EQ(two.value(), 42);
}

TEST(Optional, OperatorArrow)
{
    struct data { int x; };
    using T = optional<data>;

    T t(in_place, 42);

    EXPECT_EQ(t->x, 42);
}

TEST(Optional, OperatorStar)
{
    using T = optional<int>;

    T t(42);

    EXPECT_EQ(*t, 42);
}

TEST(Optional, OperatorBool)
{
    using T = optional<int>;

    T empty;
    T full(42);

    EXPECT(!bool(empty));
    EXPECT(bool(full));
}

TEST(Optional, ValueOr)
{
    using T = optional<int>;

    EXPECT_EQ(T{}.value_or(42), 42);
}

TEST(Optional, Reset)
{
    using T = optional<int>;

    T t(42);

    t.reset();

    EXPECT(!t.has_value());
}

//! @todo: monadic tests
//! @todo: comparison tests
//! @todo: make_optional tests
//! @todo: exception safety tests
//! @todo: noexcept tests
