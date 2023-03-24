#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Any.hh>


using namespace __XVI_STD_CORE_NS;


//! @todo exception safety tests.


TEST(Any, DefaultConstructor)
{
    any a;

    EXPECT(!a.has_value());
    EXPECT_EQ(a.type(), typeid(void));
}

TEST(Any, CopyConstructor)
{
    any a;
    any b(42);

    any c(a);
    any d(b);

    EXPECT(!c.has_value());
    EXPECT(d.has_value());
    EXPECT_EQ(d.type(), typeid(int));
    EXPECT_EQ(any_cast<int>(d), 42);
}

TEST(Any, MoveConstructor)
{
    any a;
    any b(42);

    any c(std::move(a));
    any d(std::move(b));

    EXPECT(!c.has_value());
    EXPECT(d.has_value());
    EXPECT(b.has_value());
    EXPECT_EQ(d.type(), typeid(int));
    EXPECT_EQ(any_cast<int>(d), 42);
    EXPECT_EQ(any_cast<int>(b), 42);
}

TEST(Any, ConvertingConstructor)
{
    struct noncopyctor { noncopyctor(const noncopyctor&) = delete; };
    struct noncopyassign { void operator=(const noncopyassign&) = delete; };

    any a('1');
    any b(2);
    any c(3UL);
    any d(nullptr);

    EXPECT(a.has_value());
    EXPECT(b.has_value());
    EXPECT(c.has_value());
    EXPECT(d.has_value());

    EXPECT_EQ(a.type(), typeid(char));
    EXPECT_EQ(b.type(), typeid(int));
    EXPECT_EQ(c.type(), typeid(unsigned long));
    EXPECT_EQ(d.type(), typeid(nullptr_t));

    EXPECT_EQ(any_cast<char>(a), '1');
    EXPECT_EQ(any_cast<int>(b), 2);
    EXPECT_EQ(any_cast<unsigned long>(c), 3UL);
    EXPECT_EQ(any_cast<nullptr_t>(d), nullptr);

    EXPECT(!(std::is_constructible_v<any, const noncopyctor&>));
    EXPECT((std::is_constructible_v<any, const noncopyassign&>));
}

TEST(Any, InPlaceTypeConstructor)
{
    any a(in_place_type<unsigned long>, 42U);
    any b(in_place_type<int>);

    EXPECT(a.has_value());
    EXPECT_EQ(a.type(), typeid(unsigned long));
    EXPECT_EQ(any_cast<unsigned long>(a), 42UL);

    EXPECT(b.has_value());
    EXPECT_EQ(b.type(), typeid(int));
    EXPECT_EQ(any_cast<int>(b), 0);
}

TEST(Any, CopyAssignment)
{
    const any a;
    const any b(42);

    any c;
    any d('!');
    any e;
    any f('!');

    c = a;
    d = b;
    e = b;
    f = a;

    EXPECT(!c.has_value());
    EXPECT(d.has_value());
    EXPECT(e.has_value());
    EXPECT(!f.has_value());

    EXPECT_EQ(d.type(), typeid(int));
    EXPECT_EQ(e.type(), typeid(int));

    EXPECT_EQ(any_cast<int>(d), 42);
    EXPECT_EQ(any_cast<int>(e), 42);
}

TEST(Any, MoveAssignment)
{
    any a;
    any b(42);

    any c;
    any d('!');
    any e;
    any f('!');

    c = std::move(a);
    d = std::move(b);
    e = std::move(b);
    f = std::move(a);

    EXPECT(!a.has_value());
    EXPECT(b.has_value());

    EXPECT(!c.has_value());
    EXPECT(d.has_value());
    EXPECT(e.has_value());
    EXPECT(!f.has_value());

    EXPECT_EQ(d.type(), typeid(int));
    EXPECT_EQ(e.type(), typeid(int));

    EXPECT_EQ(any_cast<int>(d), 42);
    EXPECT_EQ(any_cast<int>(e), 42);
}

TEST(Any, ConvertingAssignment)
{
    any a;
    any b;

    a = 42;
    b = '!';

    EXPECT(a.has_value());
    EXPECT(b.has_value());

    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(b.type(), typeid(char));

    EXPECT_EQ(any_cast<int>(a), 42);
    EXPECT_EQ(any_cast<char>(b), '!');
}

TEST(Any, Emplace)
{
    any a;
    any b('!');

    a.emplace<int>(42);
    b.emplace<int>(7);

    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(b.type(), typeid(int));

    EXPECT_EQ(any_cast<int>(a), 42);
    EXPECT_EQ(any_cast<int>(b), 7);
}

TEST(Any, Reset)
{
    any a;
    any b(42);

    a.reset();
    b.reset();

    EXPECT(!a.has_value());
    EXPECT(!b.has_value());

    EXPECT_EQ(a.type(), typeid(void));
    EXPECT_EQ(b.type(), typeid(void));
}

TEST(Any, Swap)
{
    any a;
    any b(42);

    a.swap(b);

    EXPECT(a.has_value());
    EXPECT(!b.has_value());

    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(b.type(), typeid(void));

    EXPECT_EQ(any_cast<int>(a), 42);
}

TEST(Any, MakeAny)
{
    auto a = make_any<int>(42);
    auto b = make_any<int>();

    EXPECT_SAME_TYPE(decltype(a), any);
    EXPECT_SAME_TYPE(decltype(b), any);

    EXPECT(a.has_value());
    EXPECT(b.has_value());

    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(b.type(), typeid(int));

    EXPECT_EQ(any_cast<int>(a), 42);
    EXPECT_EQ(any_cast<int>(b), 0);
}

TEST(Any, AnyCastRef)
{
    const any a;
    const any b(42);
    any c(42);

    EXPECT_THROW(bad_any_cast, any_cast<int>(a));
    EXPECT_THROW(bad_any_cast, any_cast<long>(b));
    EXPECT_NO_THROW(any_cast<int>(b));

    EXPECT_SAME_TYPE(decltype(any_cast<int>(b)), int);
    EXPECT_SAME_TYPE(decltype(any_cast<int>(c)), int);
    EXPECT_SAME_TYPE(decltype(any_cast<int>(std::move(c))), int);
}

TEST(Any, AnyCastPointer)
{
    const any a;
    const any b(42);
    any c(42);

    EXPECT_NO_THROW(any_cast<int>(&a));
    EXPECT_NO_THROW(any_cast<long>(&b));
    EXPECT_NO_THROW(any_cast<int>(&b));

    EXPECT_EQ(any_cast<int>(&a), nullptr);
    EXPECT_EQ(any_cast<long>(&b), nullptr);
    EXPECT_NE(any_cast<int>(&b), nullptr);

    EXPECT_SAME_TYPE(decltype(any_cast<int>(&b)), const int*);
    EXPECT_SAME_TYPE(decltype(any_cast<int>(&c)), int*);
}
