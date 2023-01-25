#include <Tools/Development/Test/Test.hh>

#include <stdexcept>


// Tests the basic EXPECT/ASSERT macros do something.
TEST(BasicMacroTests, SimplePredicates) {
    int foo = 42;
    EXPECT(true);
    EXPECT_EQ(foo, 42);
    EXPECT_NE(foo, 1337);
    EXPECT_GT(foo, 0);
    EXPECT_LT(0, foo);
    EXPECT_GE(foo, 42);
    EXPECT_LE(foo, 42);

    ASSERT(true);
    ASSERT_EQ(foo, foo);
    ASSERT_NE(0, foo);
    ASSERT_GT(foo, -1);
    ASSERT_LT(foo, 1024);
    ASSERT_GE(foo, foo);
    ASSERT_LE(foo, foo);
}

// Test the exception macros seem to work.
TEST(BasicMacroTests, Exceptions) {
    EXPECT_NO_THROW(int foo = 42);
    EXPECT_THROW(std::exception, throw std::runtime_error("test"));
    EXPECT_THROW_ANY(throw std::runtime_error("test"));

    ASSERT_NO_THROW(int foo = 42);
    ASSERT_THROW(std::exception, throw std::runtime_error("test"));
    ASSERT_THROW_ANY(throw std::runtime_error("test"));
}

TEST_COMPILE(BasicMacroTests, DoesCompileTest, (int foo), foo = 42)
TEST_NOT_COMPILE(BasicMacroTests, DoesNotCompileTest, (int foo), foo = nullptr)
