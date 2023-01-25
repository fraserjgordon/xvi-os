#ifndef TOOLS_DEVELOPMENT_TEST_TESTSUITE_H
#define TOOLS_DEVELOPMENT_TEST_TESTSUITE_H


#include <map>
#include <memory>
#include <string>

#include <Tools/Development/Test/TestImpl.hh>


namespace Tools::Development::Test
{


class TestSuite;


enum class TestCaseType
{
    Basic,
    Compile,
    NotCompile,
};


class TestCase
{
public:

    using TestCaseFunction = TestCaseAutoRegister::TestCaseFunction;


    TestCase(std::string_view name, TestCaseFunction* testfn);
    TestCase(TestCaseType, std::string_view name, std::string_view expr, bool flag, std::string_view file, unsigned int line);

    void run(TestContext& context) const;

    std::string_view name() const
    {
        return m_name;
    }

private:

    std::string_view    m_name = {};
    std::string_view    m_expr = {};
    TestCaseType        m_type = TestCaseType::Basic;
    TestCaseFunction*   m_function = nullptr;
    bool                m_flag = false;
    std::string_view    m_file = {};
    unsigned int        m_line = 0;
};


class TestSuite
{
public:

    TestSuite(std::string_view name);

    void runAll(TestContext& context) const;

    void addTestCase(std::unique_ptr<TestCase>);

    std::string_view name() const
    {
        return m_name;
    }

private:

    using map_t = std::map<std::string_view, std::unique_ptr<TestCase>>;


    std::string_view    m_name = {};
    map_t               m_testcases = {};


    void runTestCase(TestContext&, TestCase&) const;
};


class TestSuiteManager
{
public:

    void registerTestCase(const char* suite, const char* testcase, TestCaseAutoRegister::TestCaseFunction testfn);
    void registerTestCase(Compile, const char* suite, const char* testcase, const char* expr, bool compiles, const char* file, unsigned int line);
    void registerTestCase(NotCompile, const char* suite, const char* testcase, const char* expr, bool compiles, const char* file, unsigned int line);

    void runAll(TestContext& context) const;


    static TestSuiteManager& instance();

private:

    using map_t = std::map<std::string_view, std::unique_ptr<TestSuite>>;


    map_t m_suites;


    void runTestSuite(TestContext&, TestSuite&) const;


    static TestSuiteManager* s_instance;
};


} // namespace Tools::Development::Test


#endif /* ifndef TOOLS_DEVELOPMENT_TEST_TESTSUITE_H */