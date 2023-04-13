#include <Tools/Development/Test/Test.hh>

#include <System/C++/String/StringFwd.hh>


using namespace __XVI_STD_STRING_NS;


TEST(StringFwd, TypeAliases)
{
    EXPECT_SAME_TYPE(string, basic_string<char>);
    EXPECT_SAME_TYPE(wstring, basic_string<wchar_t>);
    EXPECT_SAME_TYPE(u8string, basic_string<char8_t>);
    EXPECT_SAME_TYPE(u16string, basic_string<char16_t>);
    EXPECT_SAME_TYPE(u32string, basic_string<char32_t>);

    EXPECT_SAME_TYPE(string_view, basic_string_view<char>);
    EXPECT_SAME_TYPE(wstring_view, basic_string_view<wchar_t>);
    EXPECT_SAME_TYPE(u8string_view, basic_string_view<char8_t>);
    EXPECT_SAME_TYPE(u16string_view, basic_string_view<char16_t>);
    EXPECT_SAME_TYPE(u32string_view, basic_string_view<char32_t>);
}
