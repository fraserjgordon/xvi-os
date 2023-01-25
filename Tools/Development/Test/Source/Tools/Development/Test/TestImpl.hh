#ifndef TOOLS_DEVELOPMENT_TEST_TESTIMPL_H
#define TOOLS_DEVELOPMENT_TEST_TESTIMPL_H


#include <string_view>

#include <Tools/Development/Test/Test.hh>


namespace Tools::Development::Test
{


using namespace std::literals;


class TestCase;
class TestSuite;


// Not derived from std::exception as we don't want to confuse it for a user exception.
struct AssertionFailedException {};


enum class ExpectationType
{
    Unknown = 0,
    True,
    False,
    Equal,
    NotEqual,
    GreaterThan,
    LessThan,
    GreaterEqual,
    LessEqual,
    NoThrow,
    Throw,
    ThrowAny,
};

struct Expectation::data_t
{
    TestContext&        m_context;
    std::string_view    m_expr;
    std::string_view    m_file;
    unsigned int        m_line;
    ExpectationType     m_type = ExpectationType::Unknown;
    bool                m_isAssertion = false;
    bool                m_passed = false;
    std::string         m_message = {};
};


class TestContext
{
public:

    void setTestLibName(const char* name);

    void beginTestSuite(const TestSuite*);
    void endTestSuite();
    void beginTestCase(const TestCase*);
    void endTestCase();

    void endTests();

    void expectationFailed(std::string message, std::string_view file, unsigned int line);

    bool allPassed() const;


private:

    const TestSuite*    m_currentSuite = nullptr;
    const TestCase*     m_currentCase = nullptr;
    bool                m_testpass = false;

    unsigned int        m_cases = 0;
    unsigned int        m_passes = 0;
    unsigned int        m_totalCases = 0;
    unsigned int        m_totalPasses = 0;

    unsigned int        m_suites = 0;
    unsigned int        m_suitePasses = 0;

    std::string         m_message = {};
    std::string_view    m_file = {};
    unsigned int        m_line = 0;

    std::string_view    m_emphasis = "\e[1m"sv;
    std::string_view    m_good = "\e[32m"sv;
    std::string_view    m_bad = "\e[31m"sv;
    std::string_view    m_normal = "\e[0m"sv;

    std::string_view    m_testLibName = {};

    bool                m_suiteHeadings = false;
};


}


#endif /* ifndef TOOLS_DEVELOPMENT_TEST_TESTIMPL_H */
