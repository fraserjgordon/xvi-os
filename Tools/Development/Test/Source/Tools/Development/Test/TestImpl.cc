#include <Tools/Development/Test/TestImpl.hh>

#include <cstdio>

#include <Tools/Development/Test/TestSuite.hh>


namespace Tools::Development::Test
{


using namespace std::literals;


Expectation::Expectation(TestContext& context, const char* expr, const char* file, unsigned int line) :
    m_data(new data_t{context, expr ? expr : "", file, line})
{
}


Expectation::~Expectation() noexcept(false)
{
    if (!m_data->m_passed)
    {
        m_data->m_context.expectationFailed(std::move(m_data->m_message), m_data->m_file, m_data->m_line);

        if (m_data->m_isAssertion)
        {
            // Need to do manual cleanup as we're throwing from this destructor!
            m_data.reset();
            throw AssertionFailedException{};
        }
    }
}


Expectation& Expectation::expectTrue(bool cond)
{
    m_data->m_type = ExpectationType::True;
    m_data->m_passed = cond;

    if (!m_data->m_passed)
        m_data->m_message = "condition evaluated to false: "s.append(m_data->m_expr);

    return *this;
}


void Expectation::setIsAssertion()
{
    m_data->m_isAssertion = true;
}


Expectation& Expectation::doExpectNoThrow(void (*fn)(void*), void* context) try
{
    m_data->m_type = ExpectationType::NoThrow;

    (*fn)(context);

    m_data->m_passed = true;
    return *this;
}
catch (std::exception& ex)
{
    m_data->m_passed = false;
    m_data->m_message = "exception thrown: "s + ex.what();
    return *this;
}
catch (...)
{
    m_data->m_passed = false;
    m_data->m_message = "exception thrown"s;
    return *this;
}


Expectation& Expectation::doExpectThrow(void (*fn)(void*), void* context)
{
    m_data->m_type = ExpectationType::Throw;
    m_data->m_passed = true;

    (*fn)(context);

    m_data->m_passed = false;
    return *this;
}


Expectation& Expectation::unexpectedException(std::exception_ptr eptr) try
{
    m_data->m_passed = false;
    std::rethrow_exception(eptr);
}
catch (std::exception& ex)
{
    m_data->m_message = "unexpected exception type: "s + ex.what();
    return *this;
}
catch (...)
{
    m_data->m_message = "unexpected exception type"s;
    return *this;
}


void TestContext::setTestLibName(const char* name)
{
    m_testLibName = {};
    if (name)
        m_testLibName = name;
}


void TestContext::beginTestSuite(const TestSuite* suite)
{
    m_currentSuite = suite;
    m_currentCase = nullptr;

    m_suites += 1;

    m_cases = 0;
    m_passes = 0;

    if (m_suiteHeadings)
    {
        std::fprintf(stderr, "%sBeginning test suite %s%s%s:%s\n", 
            m_emphasis.data(),
            m_testLibName.data(),
            m_testLibName.length() ? "." : "",
            suite->name().data(),
            m_normal.data()
        );
    }
}


void TestContext::endTestSuite()
{
    if (m_suiteHeadings)
    {
        std::fprintf(stderr, "%sFinished test suite %s. %s%u of %u tests passed.%s\n",
            m_emphasis.data(),
            m_currentSuite->name().data(),
            m_passes == m_cases ? m_good.data() : m_bad.data(),
            m_passes,
            m_cases,
            m_normal.data()
        );
    }

    m_currentSuite = nullptr;
    m_currentCase = nullptr;

    if (m_cases == m_passes)
        m_suitePasses += 1;

    m_totalCases += m_cases;
    m_totalPasses += m_passes;

    m_cases = 0;
    m_passes = 0;
}


void TestContext::beginTestCase(const TestCase* testcase)
{
    m_currentCase = testcase;
    m_cases += 1;
    m_testpass = true;

    m_message = {};
    m_file = {};
    m_line = 0;

    std::fprintf(stderr, "%s%s%s%s%s:  ",
        (!m_suiteHeadings && m_testLibName.length() > 0) ? m_testLibName.data() : "",
        (!m_suiteHeadings && m_testLibName.length() > 0) ? "." : "",
        m_suiteHeadings ? "  " : m_currentSuite->name().data(),
        m_suiteHeadings ? "" : ".",
        testcase->name().data()
    );
}


void TestContext::endTestCase()
{
    if (m_testpass)
    {
        m_passes += 1;
        std::fprintf(stderr, "%spass%s\n",
            m_good.data(),
            m_normal.data()
        );
    }
    else
    {
        std::fprintf(stderr, "%sfailed%s (%s; %s:%u)\n",
            m_bad.data(),
            m_normal.data(),
            m_message.c_str(),
            m_file.data(),
            m_line
        );
    }

    m_currentCase = nullptr;
}


void TestContext::endTests()
{
    if (m_suitePasses == m_suites)
    {
        std::fprintf(stderr, "%s%s%sAll %u test suites passed (%u test cases)%s\n",
            (m_testLibName.length() > 0) ? m_testLibName.data() : "",
            (m_testLibName.length() > 0) ? ": " : "",
            m_good.data(),
            m_suites,
            m_totalCases,
            m_normal.data()
        );
    }
    else
    {
        std::fprintf(stderr, "%s%s%s%u/%u test suites passed (%u/%u test cases)%s\n",
            (m_testLibName.length() > 0) ? m_testLibName.data() : "",
            (m_testLibName.length() > 0) ? ": " : "",
            m_bad.data(),
            m_suitePasses,
            m_suites,
            m_totalPasses,
            m_totalCases,
            m_normal.data()
        );
    }
}


bool TestContext::allPassed() const
{
    return m_suites == m_suitePasses;
}


void TestContext::expectationFailed(std::string message, std::string_view file, unsigned int line)
{
    m_testpass = false;

    if (m_line == 0)
    {
        m_message = std::move(message);
        m_file = file;
        m_line = line;
    }

    if (m_message.empty())
        m_message = "<unknown>"s;
}


} // namespace Tools::Development::Test
