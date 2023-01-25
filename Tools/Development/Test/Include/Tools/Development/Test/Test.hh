#ifndef TOOLS_DEVELOPMENT_TEST_TEST_H
#define TOOLS_DEVELOPMENT_TEST_TEST_H


#include <concepts>
#include <exception>
#include <iosfwd>
#include <memory>


namespace Tools::Development::Test
{


template <class T>
concept OStreamable = requires(std::ostream& os, const T& t)
{
    os << t;
};


class TestContext;


class Expectation
{
public:

    Expectation(TestContext& context, const char* expr, const char* file, unsigned int line);

    ~Expectation() noexcept(false);

    Expectation& expectTrue(bool);

    Expectation& assertTrue(bool cond)
    {
        setIsAssertion();
        return expectTrue(cond);
    }

    template <class Fn>
    Expectation& expectNoThrow(Fn&& fn)
    {
        return doExpectNoThrow([](void* ctxt)
        {
            (*static_cast<Fn*>(ctxt))();
        }, &fn);
    }

    template <class Fn>
    Expectation& assertNoThrow(Fn&& fn)
    {
        setIsAssertion();
        return expectNoThrow(std::forward<Fn>(fn));
    }

    template <class Exception, class Fn>
    Expectation& expectThrow(Fn&& fn) try
    {
        return doExpectThrow([](void* ctxt)
        {
            (*static_cast<Fn*>(ctxt))();
        }, &fn);
    }
    catch (Exception& ex)
    {
        return *this;
    }
    catch (...)
    {
        return unexpectedException(std::current_exception());
    }

    template <class Exception, class Fn>
    Expectation& assertThrow(Fn&& fn)
    {
        setIsAssertion();
        return expectThrow<Exception>(std::forward<Fn>(fn));
    }

    template <class Fn>
    Expectation& expectThrowAnything(Fn&& fn) try
    {
        return doExpectThrow([](void* ctxt)
        {
            (*static_cast<Fn*>(ctxt))();
        }, &fn);
    }
    catch (...)
    {
        return *this;
    }

    template <class Fn>
    Expectation& assertThrowAnything(Fn&& fn)
    {
        setIsAssertion();
        return expectThrowAnything(std::forward<Fn>(fn));
    }

    template <OStreamable OS>
    Expectation& operator<<(const OS& obj);

private:

    struct data_t;


    std::unique_ptr<data_t> m_data;


    void setIsAssertion();

    Expectation& doExpectNoThrow(void (*fn)(void*), void* ctxt);
    Expectation& doExpectThrow(void (*fn)(void*), void* ctxt);

    Expectation& unexpectedException(std::exception_ptr);
};


using Assertion = Expectation;


struct Compile {};
struct NotCompile {};


class TestCaseAutoRegister
{
public:

    using TestCaseFunction = void(TestContext&);

    // Explicitly not constexpr - we need the constructor to run at init time.
    TestCaseAutoRegister(const char* suite, const char* name, TestCaseFunction* test_case);
    TestCaseAutoRegister(Compile, const char* suite, const char* name, const char* expr, bool compiles, const char* file, unsigned int line);
    TestCaseAutoRegister(NotCompile, const char* suite, const char* name, const char* expr, bool compiles, const char* file, unsigned int line);

    ~TestCaseAutoRegister() = default;
};


int runAllTests();


} // namespace Tools::Development::Test


#if defined(TEST_LIB_NAME)
[[gnu::used, gnu::visibility("default")]] extern inline const char* const _test_testlibname = TEST_LIB_NAME;
#else
[[gnu::weak, gnu::visibility("default")]] extern const char* const _test_testlibname;
#  define TESTLIBNAME(name) extern const char* const _test_testlibname = name;
#endif

#define TEST(suite, name) TEST_(suite, name)
#define TEST_(suite, name) TEST_DEFINE_TEST(suite, #suite, name, #name)
#define TEST_DEFINE_TEST(suite, suitestr, name, namestr) \
    auto _test_testcasefn_ ## suite ## _ ## name (Tools::Development::Test::TestContext&) -> void; \
    const Tools::Development::Test::TestCaseAutoRegister _test_testcase_ ## name { suitestr, namestr, &_test_testcasefn_ ## suite ## _ ## name }; \
    auto _test_testcasefn_ ## suite ## _ ## name (Tools::Development::Test::TestContext& _test_context_) -> void

#define TEST_EXPECTATION(expr) TEST_EXPECTATION_(expr, #expr)
#define TEST_EXPECTATION_(expr, exprstr) \
    Tools::Development::Test::Expectation(_test_context_, exprstr, __FILE__, __LINE__).expectTrue(static_cast<bool>(expr))

#define EXPECT(condition) TEST_EXPECTATION(condition)
#define EXPECT_EQ(left, right) TEST_EXPECTATION((left == right))
#define EXPECT_NE(left, right) TEST_EXPECTATION((left != right))
#define EXPECT_LT(left, right) TEST_EXPECTATION((left <  right))
#define EXPECT_GT(left, right) TEST_EXPECTATION((left >  right))
#define EXPECT_LE(left, right) TEST_EXPECTATION((left <= right))
#define EXPECT_GE(left, right) TEST_EXPECTATION((left >= right))

#define EXPECT_NO_THROW(...) \
    Tools::Development::Test::Expectation(_test_context_, nullptr, __FILE__, __LINE__).expectNoThrow([&](){ __VA_ARGS__ ; })

#define EXPECT_THROW(exception, ...) \
    Tools::Development::Test::Expectation(_test_context_, nullptr, __FILE__, __LINE__).expectThrow<exception>([&](){ __VA_ARGS__ ; })

#define EXPECT_THROW_ANY(...) \
    Tools::Development::Test::Expectation(_test_context_, nullptr, __FILE__, __LINE__).expectThrowAnything([&](){ __VA_ARGS__ ; })

#define TEST_ASSERTION(expr) TEST_ASSERTION_(expr, #expr)
#define TEST_ASSERTION_(expr, exprstr) \
    Tools::Development::Test::Assertion(_test_context_, exprstr, __FILE__, __LINE__).assertTrue(static_cast<bool>(expr))

#define ASSERT(condition) TEST_ASSERTION(condition)
#define ASSERT_EQ(left, right) TEST_ASSERTION((left == right))
#define ASSERT_NE(left, right) TEST_ASSERTION((left != right))
#define ASSERT_LT(left, right) TEST_ASSERTION((left <  right))
#define ASSERT_GT(left, right) TEST_ASSERTION((left >  right))
#define ASSERT_LE(left, right) TEST_ASSERTION((left <= right))
#define ASSERT_GE(left, right) TEST_ASSERTION((left >= right))

#define ASSERT_NO_THROW(...) \
    Tools::Development::Test::Assertion(_test_context_, nullptr, __FILE__, __LINE__).assertNoThrow([&](){ __VA_ARGS__ ; })

#define ASSERT_THROW(exception, ...) \
    Tools::Development::Test::Assertion(_test_context_, nullptr, __FILE__, __LINE__).assertThrow<exception>([&](){ __VA_ARGS__ ; })

#define ASSERT_THROW_ANY(...) \
    Tools::Development::Test::Assertion(_test_context_, nullptr, __FILE__, __LINE__).assertThrowAnything([&](){ __VA_ARGS__ ; })

#define TEST_COMPILE(suite, name, signature, ...) TEST_COMPILE_(suite, name, signature, (__VA_ARGS__))
#define TEST_COMPILE_(suite, name, signature, expr) TEST_DEFINE_COMPILE(Tools::Development::Test::Compile, suite, #suite, name, #name, signature, expr, #expr)

#define TEST_NOT_COMPILE(suite, name, signature, ...) TEST_NOT_COMPILE_(suite, name, signature, (__VA_ARGS__))
#define TEST_NOT_COMPILE_(suite, name, signature, expr) TEST_DEFINE_COMPILE(Tools::Development::Test::NotCompile, suite, #suite, name, #name, signature, expr, #expr)

#define TEST_DEFINE_COMPILE(type, suite, suitestr, name, namestr, signature, expr, exprstr) \
    template <class T> concept _test_compileconcept_ ## suite ## _ ## name = requires signature { { static_cast<T>(expr) }; }; \
    constexpr bool _test_compiles_ ## suite ## _ ## name = _test_compileconcept_ ## suite ## _ ## name <void>; \
    Tools::Development::Test::TestCaseAutoRegister _test_testcompile_ ## suite ## _ ## name(type{}, suitestr, namestr, exprstr, _test_compiles_ ## suite ## _ ## name, __FILE__, __LINE__);


#endif /* ifndef TOOLS_DEVELOPMENT_TEST_TEST_H */
