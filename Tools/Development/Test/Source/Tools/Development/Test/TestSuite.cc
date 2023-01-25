#include <Tools/Development/Test/TestSuite.hh>


namespace Tools::Development::Test
{


using namespace std::literals;


TestSuiteManager* TestSuiteManager::s_instance = nullptr;


TestCaseAutoRegister::TestCaseAutoRegister(const char* suite, const char* name, TestCaseFunction* testfn)
{
    TestSuiteManager::instance().registerTestCase(suite, name, testfn);
}


TestCaseAutoRegister::TestCaseAutoRegister(Compile, const char* suite, const char* name, const char* expr, bool compiles, const char* file, unsigned int line)
{
    TestSuiteManager::instance().registerTestCase(Compile{}, suite, name, expr, compiles, file, line);
}


TestCaseAutoRegister::TestCaseAutoRegister(NotCompile, const char* suite, const char* name, const char* expr, bool compiles, const char* file, unsigned int line)
{
    TestSuiteManager::instance().registerTestCase(NotCompile{}, suite, name, expr, compiles, file, line);
}


int runAllTests()
{
    TestContext context;

    if (&_test_testlibname)
        context.setTestLibName(_test_testlibname);

    TestSuiteManager::instance().runAll(context);

    return context.allPassed() ? 0 : -1;
}


TestCase::TestCase(std::string_view name, TestCaseFunction* testfn) :
    m_name{name},
    m_function{testfn}
{
}


TestCase::TestCase(TestCaseType type, std::string_view name, std::string_view expr, bool flag, std::string_view file, unsigned int line) :
    m_name{name},
    m_expr{expr},
    m_type{type},
    m_flag{flag},
    m_file{file},
    m_line{line}
{
}


void TestCase::run(TestContext& context) const
{
    context.beginTestCase(this);

    if (m_type == TestCaseType::Basic)
    {
        try
        {
            (*m_function)(context);
        }
        catch (AssertionFailedException)
        {
            // All we needed to do was catch this exception.
        }
        catch (std::exception& ex)
        {
            auto message = "unhandled exception: "s + ex.what();
            context.expectationFailed(std::move(message), __FILE__, __LINE__);
        }
        catch (...)
        {
            context.expectationFailed("unhandled exception"s, __FILE__, __LINE__);
        }
    }
    else
    {
        if (m_type == TestCaseType::Compile)
        {
            if (!m_flag)
                context.expectationFailed("expression failed to compile: "s.append(m_expr), m_file, m_line);
        }
        else
        {
            if (m_flag)
                context.expectationFailed("expression unexpectedly compiled: "s.append(m_expr), m_file, m_line);
        }
    }

    context.endTestCase();
}


TestSuite::TestSuite(std::string_view name) :
    m_name{name}
{
}


void TestSuite::runAll(TestContext& context) const
{
    context.beginTestSuite(this);

    for (const auto& testcase : m_testcases)
        testcase.second->run(context);

    context.endTestSuite();
}


void TestSuite::addTestCase(std::unique_ptr<TestCase> testcase)
{
    auto name = testcase->name();
    m_testcases.insert({name, std::move(testcase)});
}


void TestSuiteManager::registerTestCase(const char* suite, const char* name, TestCaseAutoRegister::TestCaseFunction* testfn)
{
    // Ensure that the test suite has been created.
    auto suite_iter = m_suites.find(suite);
    if (suite_iter == m_suites.end())
        suite_iter = m_suites.insert({std::string_view{suite}, std::make_unique<TestSuite>(suite)}).first;

    suite_iter->second->addTestCase(std::make_unique<TestCase>(name, testfn));
}


void TestSuiteManager::registerTestCase(Compile, const char* suite, const char* name, const char* expr, bool compiles, const char* file, unsigned int line)
{
    // Ensure that the test suite has been created.
    auto suite_iter = m_suites.find(suite);
    if (suite_iter == m_suites.end())
        suite_iter = m_suites.insert({std::string_view{suite}, std::make_unique<TestSuite>(suite)}).first;

    suite_iter->second->addTestCase(std::make_unique<TestCase>(TestCaseType::Compile, name, expr, compiles, file, line));
}


void TestSuiteManager::registerTestCase(NotCompile, const char* suite, const char* name, const char* expr, bool compiles, const char* file, unsigned int line)
{
    // Ensure that the test suite has been created.
    auto suite_iter = m_suites.find(suite);
    if (suite_iter == m_suites.end())
        suite_iter = m_suites.insert({std::string_view{suite}, std::make_unique<TestSuite>(suite)}).first;

    suite_iter->second->addTestCase(std::make_unique<TestCase>(TestCaseType::NotCompile, name, expr, compiles, file, line));
}


void TestSuiteManager::runAll(TestContext& context) const
{
    if (m_suites.empty())
        return;    

    for (const auto& suite : m_suites)
        suite.second->runAll(context);

    context.endTests();
}


TestSuiteManager& TestSuiteManager::instance()
{
    if (!s_instance)
        s_instance = new TestSuiteManager{};

    return *s_instance;
}


} // namespace Tools::Development::Test
