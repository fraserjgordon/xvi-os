#include <Tools/Development/Test/Test.hh>

#include <System/C++/String/StringView.hh>

#include <array>
#include <cstring>


using namespace __XVI_STD_STRING_NS;


// This file currently only tests basic_string_view via string_view - this should be sufficient as the template doesn't
// vary in behaviour depending on the character type.


TEST(StringView, StringViewTypes)
{
    EXPECT_SAME_TYPE(string_view::traits_type, char_traits<char>);
    EXPECT_SAME_TYPE(string_view::value_type, char);
    EXPECT_SAME_TYPE(string_view::pointer, char*);
    EXPECT_SAME_TYPE(string_view::const_pointer, const char*);
    EXPECT_SAME_TYPE(string_view::reference, char&);
    EXPECT_SAME_TYPE(string_view::const_reference, const char&);
    EXPECT_SAME_TYPE(string_view::iterator, string_view::const_iterator);
    EXPECT_SAME_TYPE(string_view::const_reverse_iterator, reverse_iterator<string_view::const_iterator>);
    EXPECT_SAME_TYPE(string_view::reverse_iterator, string_view::const_reverse_iterator);
    EXPECT_SAME_TYPE(string_view::size_type, size_t);
    EXPECT_SAME_TYPE(string_view::difference_type, ptrdiff_t);

    // The const_iterator type is implementation-defined. This implementation defines it to be the same as the
    // const_pointer type.
    EXPECT_SAME_TYPE(string_view::const_iterator, const char*);
}

TEST(StringView, DefaultConstructor)
{
    string_view sv;

    EXPECT_EQ(sv.data(), nullptr);
    EXPECT_EQ(sv.size(), 0);
}

TEST(StringView, CopyConstructor)
{
    EXPECT(std::is_trivially_copy_constructible_v<string_view>);
}

TEST(StringView, MoveConstructor)
{
    EXPECT(std::is_trivially_move_constructible_v<string_view>);
}

TEST(StringView, CopyAssignment)
{
    EXPECT(std::is_trivially_copy_assignable_v<string_view>);
}

TEST(StringView, MoveAssignment)
{
    EXPECT(std::is_trivially_move_assignable_v<string_view>);
}

TEST(StringView, Destructor)
{
    EXPECT(std::is_trivially_destructible_v<string_view>);
}

TEST(StringView, CStringConstructor)
{
    const char* const str = "Hello, World!";

    string_view sv(str);

    EXPECT_EQ(sv.data(), str);
    EXPECT_EQ(sv.size(), 13);
}

TEST(StringView, NullptrConstructorDeleted)
{
    EXPECT(!(std::is_constructible_v<string_view, nullptr_t>));
}

TEST(StringView, PointerAndLengthConstructor)
{
    const char* const str = "Hello, World!";

    string_view sv(str, 5);

    EXPECT_EQ(sv.data(), str);
    EXPECT_EQ(sv.size(), 5);
}

TEST(StringView, IteratorAndSentinelConstructor)
{
    const char* const str = "Hello, World!";

    string_view sv(&str[7], &str[13] + 1);

    EXPECT_EQ(sv.data(), &str[7]);
    EXPECT_EQ(sv.size(), 7);
    EXPECT_EQ(sv.data()[6], '\0');
}

TEST(StringView, RangeConstructor)
{
    const std::array<char, 14> str { "Hello, World!" };

    string_view sv(str);

    EXPECT_EQ(sv.data(), str.data());
    EXPECT_EQ(sv.size(), str.size());
}

TEST(StringView, Iterators)
{
    const char* const str = "Hello, World!";

    string_view empty;
    string_view sv(str);

    EXPECT_EQ(empty.begin(), nullptr);
    EXPECT_EQ(empty.end(), nullptr);

    EXPECT_EQ(sv.begin(), str);
    EXPECT_EQ(sv.end(), str + 13);

    EXPECT_EQ(empty.begin(), empty.cbegin());
    EXPECT_EQ(empty.end(), empty.cend());
    EXPECT_EQ(sv.begin(), sv.cbegin());
    EXPECT_EQ(sv.end(), sv.cend());

    EXPECT_EQ(empty.rbegin(), string_view::reverse_iterator(empty.end()));
    EXPECT_EQ(empty.rend(), string_view::reverse_iterator(empty.begin()));
    EXPECT_EQ(sv.rbegin(), string_view::reverse_iterator(sv.end()));
    EXPECT_EQ(sv.rend(), string_view::reverse_iterator(sv.begin()));

    EXPECT_EQ(empty.rbegin(), empty.crbegin());
    EXPECT_EQ(empty.rend(), empty.crend());
    EXPECT_EQ(sv.rbegin(), sv.crbegin());
    EXPECT_EQ(sv.rend(), sv.crend());
}

TEST(StringView, ElementAccess)
{
    const char* const str = "Hello, World!";

    string_view empty;
    string_view sv(str);

    for (size_t i = 0; i < 13; ++i)
        EXPECT_EQ(sv[i], str[i]);

    for (size_t i = 0; i < 13; ++i)
        EXPECT_EQ(sv.at(i), str[i]);

    EXPECT_THROW(out_of_range, empty.at(0));
    EXPECT_THROW(out_of_range, sv.at(13));

    EXPECT_EQ(&sv.front(), str);
    EXPECT_EQ(&sv.back(), str + 12);

    EXPECT_EQ(&sv.front(), sv.data());
}

TEST(StringView, Modifiers)
{
    const char* const str = "Hello, World!";

    string_view sv(str);

    sv.remove_suffix(1);

    EXPECT_EQ(sv.data(), str);
    EXPECT_EQ(sv.size(), 12);

    sv.remove_prefix(7);

    EXPECT_EQ(sv.data(), str + 7);
    EXPECT_EQ(sv.size(), 5);
}

TEST(StringView, Swap)
{
    const char* const str = "Hello, World!";

    string_view a;
    string_view b(str);

    a.swap(b);

    EXPECT_EQ(a.data(), str);
    EXPECT_EQ(a.size(), 13);
    EXPECT_EQ(b.data(), nullptr);
    EXPECT_EQ(b.size(), 0);
}

TEST(StringView, Copy)
{
    const char* const str = "Hello, World!";
    char buffer[5];

    string_view sv(str);

    sv.copy(buffer, 5);

    EXPECT_EQ(std::memcmp(buffer, str, 5), 0);

    sv.copy(buffer, 5, 7);

    EXPECT_EQ(std::memcmp(buffer, str + 7, 5), 0);
}

TEST(StringView, Substr)
{
    const char* const str = "Hello, World!";

    string_view sv(str);

    auto hello = sv.substr(0, 5);
    auto world = sv.substr(7);

    EXPECT_EQ(hello.data(), str);
    EXPECT_EQ(hello.size(), 5);

    EXPECT_EQ(world.data(), str + 7);
    EXPECT_EQ(world.size(), 6);
}

TEST(StringView, Compare)
{
    // Comparison should be lexicographic.
    {
        EXPECT_LT("aa"sv.compare("b"sv), 0);
    }

    // Differ only in length.
    {
        EXPECT_EQ("aa"sv.compare("aa"sv), 0);
        EXPECT_LT("a"sv.compare("aa"sv), 0);
        EXPECT_GT("aa"sv.compare("a"sv), 0);
    }

    // Comparison of a substring.
    {
        EXPECT_EQ("Hello, World!"sv.compare(7, 5, "World"sv), 0);
    }

    // Comparison of substrings.
    {
        EXPECT_EQ("Hello, World!"sv.compare(3, 2, "below"sv, 2, 2), 0);
    }

    // Comparison to a C string.
    {
        EXPECT_EQ("Hello, World!"sv.compare("Hello, World!"), 0);
    }

    // Comparison of a substring to a C string.
    {
        EXPECT_EQ("Hello, World!"sv.compare(7, 5, "World"), 0);
    }

    // Comparison of a substring to a pointer-length pair.
    {
        const char* const str = "below";
        EXPECT_EQ("Hello, World!"sv.compare(3, 2, &str[2], 2), 0);
    }
}

TEST(StringView, StartsWith)
{
    EXPECT("Hello, World!"sv.starts_with("Hello"));
    EXPECT(!"Hello, World!"sv.starts_with("hi"));

    EXPECT("Hello, World!"sv.starts_with('H'));
    EXPECT(!"Hello, World"sv.starts_with('!'));

    EXPECT("Hello, World!"sv.starts_with("He"sv));
    EXPECT(!"Hello, World!"sv.starts_with("Wo"sv));
}

TEST(StringView, EndsWith)
{
    EXPECT("Hello, World!"sv.ends_with("World!"));
    EXPECT(!"Hello, World!"sv.ends_with("World?"));

    EXPECT("Hello, World!"sv.ends_with('!'));
    EXPECT(!"Hello, World"sv.ends_with('H'));

    EXPECT("Hello, World!"sv.ends_with("orld!"sv));
    EXPECT(!"Hello, World!"sv.ends_with("old!"sv));
}

TEST(StringView, Contains)
{
    EXPECT("Hello, World!"sv.contains("ello"));
    EXPECT(!"Hello, World!"sv.contains("old"));

    EXPECT("Hello, World!"sv.contains('l'));
    EXPECT(!"Hello, World!"sv.contains('a'));

    EXPECT("Hello, World!"sv.contains("Wo"sv));
    EXPECT(!"Hello, World!"sv.contains("Hwllo"sv));
}

TEST(StringView, Find)
{
    const char* const str = "Hello, World!";
    
    string_view sv(str);

    EXPECT_EQ(sv.find("World"), 7);
    EXPECT_EQ(sv.find('l'), 2);
    EXPECT_EQ(sv.find("He"sv), 0);
    EXPECT_EQ(sv.find("Ho"), string_view::npos);
}

TEST(StringView, Rfind)
{
    const char* const str = "Hello, World!";

    string_view sv(str);

    EXPECT_EQ(sv.rfind("World"), 7);
    EXPECT_EQ(sv.rfind('l'), 10);
    EXPECT_EQ(sv.rfind("He"sv), 0);
    EXPECT_EQ(sv.rfind("Ho"), string_view::npos);
}

TEST(StringView, DeductionGuides)
{
    const char* const cstr = "Hello, World!";
    const std::array<char, 14> arr { "Hello, World!" };

    basic_string_view bsv1(cstr, cstr+13);
    basic_string_view bsv2(arr);

    EXPECT_SAME_TYPE(decltype(bsv1), string_view);
    EXPECT_SAME_TYPE(decltype(bsv2), string_view);

    EXPECT_EQ(bsv1.data(), cstr);
    EXPECT_EQ(bsv1.size(), 13);
    EXPECT_EQ(bsv2.data(), arr.data());
    EXPECT_EQ(bsv2.size(), arr.size());
}

TEST(StringView, Literals)
{
    auto sv = "Hello, World!"sv;
    auto wsv = L"Hello, World!"sv;
    auto u8sv = u8"Hello, World!"sv;
    auto u16sv = u"Hello, World!"sv;
    auto u32sv = U"Hello, World!"sv;

    EXPECT_SAME_TYPE(decltype(sv), string_view);
    EXPECT_SAME_TYPE(decltype(wsv), wstring_view);
    EXPECT_SAME_TYPE(decltype(u8sv), u8string_view);
    EXPECT_SAME_TYPE(decltype(u16sv), u16string_view);
    EXPECT_SAME_TYPE(decltype(u32sv), u32string_view);
}
