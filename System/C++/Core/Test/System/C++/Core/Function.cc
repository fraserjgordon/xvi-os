#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Function.hh>


using namespace __XVI_STD_CORE_NS;


TEST(Function, DefaultConstructor)
{
    using F = function<void()>;

    F f;

    EXPECT(!f);
    EXPECT_EQ(f.target_type(), typeid(void));
}

TEST(Function, NullptrConstructor)
{
    using F = function<void()>;

    F f(nullptr);

    EXPECT(!f);
    EXPECT_EQ(f.target_type(), typeid(void));
}

TEST(Function, CopyConstructor)
{
    using F = function<void()>;

    const F a;
    const F b([]{});

    F c(a);
    F d(b);

    EXPECT(!c);
    EXPECT(d);
}

TEST(Function, MoveConstructor)
{
    using F = function<void()>;

    F a;
    F b([]{});

    F c(std::move(a));
    F d(std::move(b));

    EXPECT(!c);
    EXPECT(d);
}

TEST(Function, ConvertingConstructor)
{
    struct callable { static void operator()() {}; };
    using F = function<void()>;

    F f1([]{});
    F f2(callable{});
    F f3(&callable::operator());

    EXPECT(f1);
    EXPECT(f2);
    EXPECT(f3);
}

TEST(Function, DeductionGuides)
{
    struct static_callable { static int operator()(long) { return 0; } };
    struct callable { long operator()(char) const { return 0; } };

    // Deduction from a function pointer.
    EXPECT_SAME_TYPE(decltype(function(&static_callable::operator())), function<int(long)>);

    // Deduction from an object with a non-static call operator.
    EXPECT_SAME_TYPE(decltype(function(callable{})), function<long(char)>);

    // Deduction from an object with a static call operator.
    EXPECT_SAME_TYPE(decltype(function(static_callable{})), function<int(long)>);

    //! @todo deduction guide test with a deduced-this non-static member function.
}
