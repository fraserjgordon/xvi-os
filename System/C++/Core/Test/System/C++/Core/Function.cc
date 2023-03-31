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

TEST(Function, CopyAssignment)
{
    using F = function<void()>;

    const F a;
    const F b([]{});

    F c;
    F d([]{});
    F e;
    F f([]{});

    c = a;
    d = a;
    e = b;
    f = b;

    EXPECT(!c);
    EXPECT(!d);
    EXPECT(e);
    EXPECT(f);
}

TEST(Function, MoveAssignment)
{
    using F = function<void()>;

    F a;
    F b([]{});

    F c;
    F d([]{});
    F e;
    F f([]{});

    c = std::move(a);
    d = std::move(a);
    e = std::move(b);
    f = std::move(b);

    EXPECT(!c);
    EXPECT(!d);
    EXPECT(e);
    EXPECT(f);
}

TEST(Function, NullptrAssignment)
{
    using F = function<void()>;

    F a;
    F b([]{});

    a = nullptr;
    b = nullptr;

    EXPECT(!a);
    EXPECT(!b);
}

TEST(Function, ConveringAssignment)
{
    struct callable { static void operator()() {}; };
    using F = function<void()>;

    F f1;
    F f2;
    F f3;

    f1 = []{};
    f2 = callable{};
    f3 = &callable::operator();

    EXPECT(f1);
    EXPECT(f2);
    EXPECT(f3);
}

TEST(Function, ReferenceWrapperAssignment)
{
    struct callable { static void operator()() {}; };
    using F = function<void()>;

    const F empty;

    F f1;
    F f2;
    F f3;
    F f4;
    F f5([]{});

    auto lambda = []{};
    auto fn = callable{};
    auto fnptr = &callable::operator();

    // Note: the rules of std::function mean that f5 is non-empty even though we're passing a wrapped reference to an
    //       empty function. It does not get unwrapped and turned into a copy assignment!
    f1 = ref(lambda);
    f2 = ref(fn);
    f3 = ref(fnptr);
    f4 = ref(f5);
    f5 = ref(empty);

    EXPECT(f1);
    EXPECT(f2);
    EXPECT(f3);
    EXPECT(f4);
    EXPECT(f5);
}

TEST(Function, Swap)
{
    using F = function<void()>;

    F f1;
    F f2([]{});

    f1.swap(f2);

    EXPECT(f1);
    EXPECT(!f2);
}

TEST(Function, Invocation)
{
    using F = function<int()>;

    EXPECT_THROW(bad_function_call, F{}());

    EXPECT_EQ(F{[]{ return 42; }}(), 42);
}

TEST(Function, TargetAccess)
{
    using F = function<void()>;

    auto lambda = []{};

    F f1;
    F f2(lambda);

    EXPECT_EQ(f1.target_type(), typeid(void));
    EXPECT_EQ(f2.target_type(), typeid(lambda));

    EXPECT_EQ(f1.target<decltype(lambda)>(), nullptr);
    EXPECT_NE(f2.target<decltype(lambda)>(), nullptr);
}

TEST(Function, Comparison)
{
    using F = function<void()>;

    const F f1;
    const F f2([]{});

    EXPECT_EQ(f1, nullptr);
    EXPECT_NE(f2, nullptr);
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

    //! @todo deduction guide test with an explicit-this non-static member function.
}
