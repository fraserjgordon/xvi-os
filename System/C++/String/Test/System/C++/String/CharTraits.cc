#include <Tools/Development/Test/Test.hh>

#include <System/C++/String/CharTraits.hh>

#include <cstring>


using namespace __XVI_STD_STRING_NS;


TEST(CharTraits, CharTypes)
{
    using Traits = char_traits<char>;

    EXPECT_SAME_TYPE(Traits::char_type, char);
    EXPECT_SAME_TYPE(Traits::int_type, int);
    EXPECT_SAME_TYPE(Traits::off_type, streamoff);
    EXPECT_SAME_TYPE(Traits::pos_type, streampos);
    EXPECT_SAME_TYPE(Traits::state_type, __XVI_STD_STRING_NS::mbstate_t);
    EXPECT_SAME_TYPE(Traits::comparison_category, strong_ordering);
}

TEST(CharTraits, CharAssign)
{
    using Traits = char_traits<char>;

    // Single character assignment.
    {
        char a = 0;
        char b = 1;

        Traits::assign(a, b);

        EXPECT_EQ(b, 1);
    }

    // Character fill.
    {
        char str[7] = "Hello!";

        Traits::assign(str, 5, 'x');
        EXPECT_EQ(std::memcmp(str, "xxxxx!", 7), 0);
    }
}

TEST(CharTraits, CharComparisons)
{
    using Traits = char_traits<char>;

    EXPECT(Traits::eq(0, 0));
    EXPECT(!Traits::eq(0, 1));
    EXPECT(Traits::lt(0, 1));
    EXPECT(!Traits::lt(1, 1));

    // Comparison is required to be unsigned.
    EXPECT(Traits::lt(0, char(0xff)));

    EXPECT_EQ(Traits::compare("Hello", "Hello", 5), 0);
    EXPECT_GT(Traits::compare("hello", "Hello", 5), 0);
}

TEST(CharTraits, CharLength)
{
    using Traits = char_traits<char>;

    EXPECT_EQ(Traits::length(""), 0);
    EXPECT_EQ(Traits::length("Hello"), 5);
}

TEST(CharTraits, CharFind)
{
    using Traits = char_traits<char>;

    const char str[6] = "Hello";

    EXPECT_EQ(Traits::find(str, 6, 'H'), &str[0]);
    EXPECT_EQ(Traits::find(str, 6, 'h'), nullptr);
    EXPECT_EQ(Traits::find(str, 6, '\0'), &str[5]);
    EXPECT_EQ(Traits::find(str, 6, 'l'), &str[2]);
}

TEST(CharTraits, CharMove)
{
    using Traits = char_traits<char>;

    char str[14];

    Traits::move(str, "Hello, world!", 14);
    ASSERT_EQ(std::memcmp(str, "Hello, world!", 14), 0);

    Traits::move(&str[7], str, 5);
    EXPECT_EQ(std::strcmp(str, "Hello, Hello!"), 0);

    Traits::move(str, "Hello, world!", 14);
    Traits::move(str, &str[7], 5);
    EXPECT_EQ(std::strcmp(str, "world, world!"), 0);
}

TEST(CharTraits, CharCopy)
{
    using Traits = char_traits<char>;

    char str[14];

    Traits::copy(str, "Hello, world!", 14);
    EXPECT_EQ(std::memcmp(str, "Hello, world!", 14), 0);
}

TEST(CharTraits, CharIntType)
{
    using Traits = char_traits<char>;

    EXPECT_EQ(Traits::eof(), -1);
    EXPECT_EQ(Traits::to_int_type('\0'), 0);
    EXPECT_NE(Traits::to_int_type(char(0xff)), -1);

    EXPECT(Traits::not_eof(Traits::to_int_type(char(0xff))));
    EXPECT(!Traits::not_eof(Traits::eof()));

    EXPECT_EQ(Traits::to_char_type(Traits::to_int_type(char(0xff))), char(0xff));

    EXPECT(Traits::eq_int_type(Traits::eof(), -1));
}

TEST(CharTraits, Char8Types)
{
    using Traits = char_traits<char8_t>;

    EXPECT_SAME_TYPE(Traits::char_type, char8_t);
    EXPECT_SAME_TYPE(Traits::int_type, unsigned int);
    EXPECT_SAME_TYPE(Traits::off_type, streamoff);
    EXPECT_SAME_TYPE(Traits::pos_type, u8streampos);
    EXPECT_SAME_TYPE(Traits::state_type, __XVI_STD_STRING_NS::mbstate_t);
    EXPECT_SAME_TYPE(Traits::comparison_category, strong_ordering);
}

TEST(CharTraits, Char8Assign)
{
    using Traits = char_traits<char8_t>;

    // Single character assignment.
    {
        char8_t a = 0;
        char8_t b = 1;

        Traits::assign(a, b);

        EXPECT_EQ(b, 1);
    }

    // Character fill.
    {
        char8_t str[7] = u8"Hello!";

        Traits::assign(str, 5, u8'x');
        EXPECT_EQ(std::memcmp(str, u8"xxxxx!", 7), 0);
    }
}

TEST(CharTraits, Char8Comparisons)
{
    using Traits = char_traits<char8_t>;

    EXPECT(Traits::eq(0, 0));
    EXPECT(!Traits::eq(0, 1));
    EXPECT(Traits::lt(0, 1));
    EXPECT(!Traits::lt(1, 1));

    // Comparison is required to be unsigned.
    EXPECT(Traits::lt(0, char8_t(0xff)));

    EXPECT_EQ(Traits::compare(u8"Hello", u8"Hello", 5), 0);
    EXPECT_GT(Traits::compare(u8"hello", u8"Hello", 5), 0);
}

TEST(CharTraits, Char8Length)
{
    using Traits = char_traits<char8_t>;

    EXPECT_EQ(Traits::length(u8""), 0);
    EXPECT_EQ(Traits::length(u8"Hello"), 5);
}

TEST(CharTraits, Char8Find)
{
    using Traits = char_traits<char8_t>;

    const char8_t str[6] = u8"Hello";

    EXPECT_EQ(Traits::find(str, 6, u8'H'), &str[0]);
    EXPECT_EQ(Traits::find(str, 6, u8'h'), nullptr);
    EXPECT_EQ(Traits::find(str, 6, u8'\0'), &str[5]);
    EXPECT_EQ(Traits::find(str, 6, u8'l'), &str[2]);
}

TEST(CharTraits, Char8Move)
{
    using Traits = char_traits<char8_t>;

    char8_t str[14];

    Traits::move(str, u8"Hello, world!", 14);
    ASSERT_EQ(std::memcmp(str, u8"Hello, world!", 14), 0);

    Traits::move(&str[7], str, 5);
    EXPECT_EQ(std::memcmp(str, u8"Hello, Hello!", 14), 0);

    Traits::move(str, u8"Hello, world!", 14);
    Traits::move(str, &str[7], 5);
    EXPECT_EQ(std::memcmp(str, u8"world, world!", 14), 0);
}

TEST(CharTraits, Char8Copy)
{
    using Traits = char_traits<char8_t>;

    char8_t str[14];

    Traits::copy(str, u8"Hello, world!", 14);
    EXPECT_EQ(std::memcmp(str, u8"Hello, world!", 14), 0);
}

TEST(CharTraits, Char8IntType)
{
    using Traits = char_traits<char8_t>;

    EXPECT_EQ(Traits::eof(), -1U);
    EXPECT_EQ(Traits::to_int_type(u8'\0'), 0);
    EXPECT_NE(Traits::to_int_type(char8_t(0xff)), -1U);

    EXPECT(Traits::not_eof(Traits::to_int_type(char8_t(0xff))));
    EXPECT(!Traits::not_eof(Traits::eof()));

    EXPECT_EQ(Traits::to_char_type(Traits::to_int_type(char8_t(0xff))), char8_t(0xff));

    EXPECT(Traits::eq_int_type(Traits::eof(), -1U));
}

TEST(CharTraits, Char16Types)
{
    using Traits = char_traits<char16_t>;

    EXPECT_SAME_TYPE(Traits::char_type, char16_t);
    EXPECT_SAME_TYPE(Traits::int_type, uint_least16_t);
    EXPECT_SAME_TYPE(Traits::off_type, streamoff);
    EXPECT_SAME_TYPE(Traits::pos_type, u16streampos);
    EXPECT_SAME_TYPE(Traits::state_type, __XVI_STD_STRING_NS::mbstate_t);
    EXPECT_SAME_TYPE(Traits::comparison_category, strong_ordering);
}

TEST(CharTraits, Char16Assign)
{
    using Traits = char_traits<char16_t>;

    // Single character assignment.
    {
        char16_t a = 0;
        char16_t b = 1;

        Traits::assign(a, b);

        EXPECT_EQ(b, 1);
    }

    // Character fill.
    {
        char16_t str[7] = u"Hello!";

        Traits::assign(str, 5, u'x');
        EXPECT_EQ(std::memcmp(str, u"xxxxx!", 14), 0);
    }
}

TEST(CharTraits, Char16Comparisons)
{
    using Traits = char_traits<char16_t>;

    EXPECT(Traits::eq(0, 0));
    EXPECT(!Traits::eq(0, 1));
    EXPECT(Traits::lt(0, 1));
    EXPECT(!Traits::lt(1, 1));

    // Comparison is required to be unsigned.
    EXPECT(Traits::lt(0, char16_t(0xff)));

    EXPECT_EQ(Traits::compare(u"Hello", u"Hello", 5), 0);
    EXPECT_GT(Traits::compare(u"hello", u"Hello", 5), 0);
}

TEST(CharTraits, Char16Length)
{
    using Traits = char_traits<char16_t>;

    EXPECT_EQ(Traits::length(u""), 0);
    EXPECT_EQ(Traits::length(u"Hello"), 5);
}

TEST(CharTraits, Char16Find)
{
    using Traits = char_traits<char16_t>;

    const char16_t str[6] = u"Hello";

    EXPECT_EQ(Traits::find(str, 6, u'H'), &str[0]);
    EXPECT_EQ(Traits::find(str, 6, u'h'), nullptr);
    EXPECT_EQ(Traits::find(str, 6, u'\0'), &str[5]);
    EXPECT_EQ(Traits::find(str, 6, u'l'), &str[2]);
}

TEST(CharTraits, Char16Move)
{
    using Traits = char_traits<char16_t>;

    char16_t str[14];

    Traits::move(str, u"Hello, world!", 14);
    ASSERT_EQ(std::memcmp(str, u"Hello, world!", 28), 0);

    Traits::move(&str[7], str, 5);
    EXPECT_EQ(std::memcmp(str, u"Hello, Hello!", 28), 0);

    Traits::move(str, u"Hello, world!", 14);
    Traits::move(str, &str[7], 5);
    EXPECT_EQ(std::memcmp(str, u"world, world!", 28), 0);
}

TEST(CharTraits, Char16Copy)
{
    using Traits = char_traits<char16_t>;

    char16_t str[14];

    Traits::copy(str, u"Hello, world!", 14);
    EXPECT_EQ(std::memcmp(str, u"Hello, world!", 28), 0);
}

TEST(CharTraits, Char16IntType)
{
    using Traits = char_traits<char16_t>;

    EXPECT_EQ(Traits::eof(), uint16_t(-1U));
    EXPECT_EQ(Traits::to_int_type(u'\0'), 0);
    EXPECT_NE(Traits::to_int_type(char16_t(0xff)), uint16_t(-1U));

    EXPECT(Traits::not_eof(Traits::to_int_type(char16_t(0xff))));
    EXPECT(!Traits::not_eof(Traits::eof()));

    EXPECT_EQ(Traits::to_char_type(Traits::to_int_type(char16_t(0xff))), char16_t(0xff));

    EXPECT(Traits::eq_int_type(Traits::eof(), uint16_t(-1U)));
}

TEST(CharTraits, Char32Types)
{
    using Traits = char_traits<char32_t>;

    EXPECT_SAME_TYPE(Traits::char_type, char32_t);
    EXPECT_SAME_TYPE(Traits::int_type, uint_least32_t);
    EXPECT_SAME_TYPE(Traits::off_type, streamoff);
    EXPECT_SAME_TYPE(Traits::pos_type, u32streampos);
    EXPECT_SAME_TYPE(Traits::state_type, __XVI_STD_STRING_NS::mbstate_t);
    EXPECT_SAME_TYPE(Traits::comparison_category, strong_ordering);
}

TEST(CharTraits, Char32Assign)
{
    using Traits = char_traits<char32_t>;

    // Single character assignment.
    {
        char32_t a = 0;
        char32_t b = 1;

        Traits::assign(a, b);

        EXPECT_EQ(b, 1);
    }

    // Character fill.
    {
        char32_t str[7] = U"Hello!";

        Traits::assign(str, 5, U'x');
        EXPECT_EQ(std::memcmp(str, U"xxxxx!", 28), 0);
    }
}

TEST(CharTraits, Char32Comparisons)
{
    using Traits = char_traits<char32_t>;

    EXPECT(Traits::eq(0, 0));
    EXPECT(!Traits::eq(0, 1));
    EXPECT(Traits::lt(0, 1));
    EXPECT(!Traits::lt(1, 1));

    // Comparison is required to be unsigned.
    EXPECT(Traits::lt(0, char32_t(0xff)));

    EXPECT_EQ(Traits::compare(U"Hello", U"Hello", 5), 0);
    EXPECT_GT(Traits::compare(U"hello", U"Hello", 5), 0);
}

TEST(CharTraits, Char32Length)
{
    using Traits = char_traits<char32_t>;

    EXPECT_EQ(Traits::length(U""), 0);
    EXPECT_EQ(Traits::length(U"Hello"), 5);
}

TEST(CharTraits, Char32Find)
{
    using Traits = char_traits<char32_t>;

    const char32_t str[6] = U"Hello";

    EXPECT_EQ(Traits::find(str, 6, U'H'), &str[0]);
    EXPECT_EQ(Traits::find(str, 6, U'h'), nullptr);
    EXPECT_EQ(Traits::find(str, 6, U'\0'), &str[5]);
    EXPECT_EQ(Traits::find(str, 6, U'l'), &str[2]);
}

TEST(CharTraits, Char32Move)
{
    using Traits = char_traits<char32_t>;

    char32_t str[14];

    Traits::move(str, U"Hello, world!", 14);
    ASSERT_EQ(std::memcmp(str, U"Hello, world!", 56), 0);

    Traits::move(&str[7], str, 5);
    EXPECT_EQ(std::memcmp(str, U"Hello, Hello!", 56), 0);

    Traits::move(str, U"Hello, world!", 14);
    Traits::move(str, &str[7], 5);
    EXPECT_EQ(std::memcmp(str, U"world, world!", 56), 0);
}

TEST(CharTraits, Char32Copy)
{
    using Traits = char_traits<char32_t>;

    char32_t str[14];

    Traits::copy(str, U"Hello, world!", 14);
    EXPECT_EQ(std::memcmp(str, U"Hello, world!", 56), 0);
}

TEST(CharTraits, Char32IntType)
{
    using Traits = char_traits<char32_t>;

    EXPECT_EQ(Traits::eof(), uint32_t(-1U));
    EXPECT_EQ(Traits::to_int_type(U'\0'), 0);
    EXPECT_NE(Traits::to_int_type(char32_t(0xff)), uint32_t(-1U));

    EXPECT(Traits::not_eof(Traits::to_int_type(char32_t(0xff))));
    EXPECT(!Traits::not_eof(Traits::eof()));

    EXPECT_EQ(Traits::to_char_type(Traits::to_int_type(char32_t(0xff))), char32_t(0xff));

    EXPECT(Traits::eq_int_type(Traits::eof(), uint32_t(-1U)));
}


TEST(CharTraits, WcharTypes)
{
    using Traits = char_traits<wchar_t>;

    EXPECT_SAME_TYPE(Traits::char_type, wchar_t);
    EXPECT_SAME_TYPE(Traits::int_type, wint_t);
    EXPECT_SAME_TYPE(Traits::off_type, streamoff);
    EXPECT_SAME_TYPE(Traits::pos_type, wstreampos);
    EXPECT_SAME_TYPE(Traits::state_type, __XVI_STD_STRING_NS::mbstate_t);
    EXPECT_SAME_TYPE(Traits::comparison_category, strong_ordering);
}

TEST(CharTraits, WcharAssign)
{
    using Traits = char_traits<wchar_t>;

    // Single character assignment.
    {
        wchar_t a = 0;
        wchar_t b = 1;

        Traits::assign(a, b);

        EXPECT_EQ(b, 1);
    }

    // Character fill.
    {
        wchar_t str[7] = L"Hello!";

        Traits::assign(str, 5, L'x');
        EXPECT_EQ(std::memcmp(str, L"xxxxx!", 28), 0);
    }
}

TEST(CharTraits, WcharComparisons)
{
    using Traits = char_traits<wchar_t>;

    EXPECT(Traits::eq(0, 0));
    EXPECT(!Traits::eq(0, 1));
    EXPECT(Traits::lt(0, 1));
    EXPECT(!Traits::lt(1, 1));

    // Comparison is required to be unsigned.
    EXPECT(Traits::lt(0, wchar_t(0xff)));

    EXPECT_EQ(Traits::compare(L"Hello", L"Hello", 5), 0);
    EXPECT_GT(Traits::compare(L"hello", L"Hello", 5), 0);
}

TEST(CharTraits, WcharLength)
{
    using Traits = char_traits<wchar_t>;

    EXPECT_EQ(Traits::length(L""), 0);
    EXPECT_EQ(Traits::length(L"Hello"), 5);
}

TEST(CharTraits, WcharFind)
{
    using Traits = char_traits<wchar_t>;

    const wchar_t str[6] = L"Hello";

    EXPECT_EQ(Traits::find(str, 6, L'H'), &str[0]);
    EXPECT_EQ(Traits::find(str, 6, L'h'), nullptr);
    EXPECT_EQ(Traits::find(str, 6, L'\0'), &str[5]);
    EXPECT_EQ(Traits::find(str, 6, L'l'), &str[2]);
}

TEST(CharTraits, WcharMove)
{
    using Traits = char_traits<wchar_t>;

    wchar_t str[14];

    Traits::move(str, L"Hello, world!", 14);
    ASSERT_EQ(std::memcmp(str, L"Hello, world!", 56), 0);

    Traits::move(&str[7], str, 5);
    EXPECT_EQ(std::memcmp(str, L"Hello, Hello!", 56), 0);

    Traits::move(str, L"Hello, world!", 14);
    Traits::move(str, &str[7], 5);
    EXPECT_EQ(std::memcmp(str, L"world, world!", 56), 0);
}

TEST(CharTraits, WcharCopy)
{
    using Traits = char_traits<wchar_t>;

    wchar_t str[14];

    Traits::copy(str, L"Hello, world!", 14);
    EXPECT_EQ(std::memcmp(str, L"Hello, world!", 56), 0);
}

TEST(CharTraits, WcharIntType)
{
    using Traits = char_traits<wchar_t>;

    EXPECT_EQ(Traits::eof(), wint_t(-1U));
    EXPECT_EQ(Traits::to_int_type(L'\0'), 0);
    EXPECT_NE(Traits::to_int_type(wchar_t(0xff)), wint_t(-1U));

    EXPECT(Traits::not_eof(Traits::to_int_type(wchar_t(0xff))));
    EXPECT(!Traits::not_eof(Traits::eof()));

    EXPECT_EQ(Traits::to_char_type(Traits::to_int_type(wchar_t(0xff))), wchar_t(0xff));

    EXPECT(Traits::eq_int_type(Traits::eof(), wint_t(-1U)));
}
