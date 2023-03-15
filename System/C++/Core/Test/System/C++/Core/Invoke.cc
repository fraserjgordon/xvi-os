#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Invoke.hh>

#include <System/C++/Core/ReferenceWrapper.hh>


using namespace __XVI_STD_CORE_NS;


TEST(Invoke, MemberFunctionPtr)
{
    class Test
    {
    public:

        int func(int x) { return x; }
    };

    Test test;

    EXPECT_EQ(invoke(&Test::func, &test, 42), 42);
}

TEST(Invoke, MemberFunctionPtrReferenceWrapper)
{
    class Test
    {
    public:

        int func(int x) { return x; }
    };

    Test test;

    EXPECT_EQ(invoke(&Test::func, ref(test), 42), 42);
}

TEST(Invoke, MemberFunctionPtrSmartPointer)
{
    class Test
    {
    public:

        int func(int x) { return x; }
    };

    struct SmartPointer
    {
        Test* m_ptr;

        Test& operator*() { return *m_ptr; };
    };

    Test test;

    EXPECT_EQ(invoke(&Test::func, SmartPointer{&test}, 42), 42);
}

TEST(Invoke, MemberDataPtr)
{
    struct Test
    {
        int x;
    };

    Test test {42};

    EXPECT_EQ(invoke(&Test::x, &test), 42);
}

TEST(Invoke, MemberDataPtrReferenceWrapper)
{
    struct Test
    {
        int x;
    };

    Test test {42};

    EXPECT_EQ(invoke(&Test::x, ref(test)), 42);
}

TEST(Invoke, MemberDataPtrSmartPointer)
{
    struct Test
    {
        int x;
    };

    struct SmartPointer
    {
        Test* m_ptr;

        Test& operator*() { return *m_ptr; }
    };

    Test test {42};

    EXPECT_EQ(invoke(&Test::x, SmartPointer{&test}), 42);
}

TEST(Invoke, FunctionObject)
{
    auto lambda0 = [] { return 7; };
    auto lambda1 = [](int x) { return x; };

    EXPECT_EQ(invoke(lambda0), 7);
    EXPECT_EQ(invoke(lambda1, 42), 42);
}
