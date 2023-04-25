#include <Tools/Development/Test/Test.hh>

#include <System/C++/String/String.hh>

#include <array>
#include <cstring>
#include <limits>


using namespace __XVI_STD_STRING_NS;


//! @todo make the content tests test the whole content, not just the first char.
//! @todo tests with non-default allocator types.
//! @todo how to test assign/assign_range with self-assignments in a way that will cause reallocation?


[[noreturn]] void __XVI_STD_STRING_NS::__detail::__string_precondition_failed(const char* message)
{
    throw logic_error(message);
}


TEST(String, StringTypes)
{
    EXPECT_SAME_TYPE(string::traits_type, char_traits<char>);
    EXPECT_SAME_TYPE(string::value_type, char);
    EXPECT_SAME_TYPE(string::allocator_type, allocator<char>);
    EXPECT_SAME_TYPE(string::size_type, size_t);
    EXPECT_SAME_TYPE(string::difference_type, ptrdiff_t);
    EXPECT_SAME_TYPE(string::pointer, char*);
    EXPECT_SAME_TYPE(string::const_pointer, const char*);
    EXPECT_SAME_TYPE(string::reference, char&);
    EXPECT_SAME_TYPE(string::const_reference, const char&);
    EXPECT_SAME_TYPE(string::const_reverse_iterator, reverse_iterator<string::const_iterator>);
    EXPECT_SAME_TYPE(string::reverse_iterator, reverse_iterator<string::iterator>);

    // The const_iterator type is implementation-defined. This implementation defines it to be the same as the
    // const_pointer type.
    EXPECT_SAME_TYPE(string::const_iterator, const char*);
    EXPECT_SAME_TYPE(string::iterator, char*);
}

TEST(String, DefaultConstructor)
{
    string s;

    EXPECT_EQ(s.size(), 0);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');
}

TEST(String, CopyConstructor)
{
    const string a;
    const string b = "Hello, World!";
    const string c = "This is a string long enough to not use SSO";

    string d(a);
    string e(b);
    string f(c);

    EXPECT_EQ(d.size(), 0);
    ASSERT_NE(d.data(), nullptr);
    EXPECT_EQ(*d.data(), '\0');

    EXPECT_EQ(e.size(), 13);
    ASSERT_NE(e.data(), nullptr);
    EXPECT_EQ(*e.data(), 'H');

    EXPECT_EQ(f.size(), 43);
    ASSERT_NE(f.data(), nullptr);
    EXPECT_EQ(*f.data(), 'T');
}

TEST(String, MoveConstructor)
{
    string a;
    string b = "Hello, World!";
    string c = "This is a string long enough to not use SSO";

    string d(std::move(a));
    string e(std::move(b));
    string f(std::move(c));

    EXPECT_EQ(d.size(), 0);
    ASSERT_NE(d.data(), nullptr);
    EXPECT_EQ(*d.data(), '\0');

    EXPECT_EQ(e.size(), 13);
    ASSERT_NE(e.data(), nullptr);
    EXPECT_EQ(*e.data(), 'H');

    EXPECT_EQ(f.size(), 43);
    ASSERT_NE(f.data(), nullptr);
    EXPECT_EQ(*f.data(), 'T');
}

TEST(String, SubstringCopyConstructors)
{
    string a;
    string b = "Hello, World!";
    string c = "This is a string long enough to not use SSO";

    const string d(a, 0);
    const string e(b, 7);
    const string f(c, 5, 35);

    EXPECT_EQ(d.size(), 0);
    ASSERT_NE(d.data(), nullptr);
    EXPECT_EQ(*d.data(), '\0');

    EXPECT_EQ(e.size(), 6);
    ASSERT_NE(e.data(), nullptr);
    EXPECT_EQ(*e.data(), 'W');

    EXPECT_EQ(f.size(), 35);
    ASSERT_NE(f.data(), nullptr);
    EXPECT_EQ(*f.data(), 'i');

    EXPECT_THROW(out_of_range, string g(a, 1));
}

TEST(String, SubstringMoveConstructors)
{
    string a;
    string b = "Hello, World!";
    string c = "This is a string long enough to not use SSO";

    string d(std::move(a), 0);
    string e(std::move(b), 7);
    string f(std::move(c), 5, 35);

    EXPECT_EQ(d.size(), 0);
    ASSERT_NE(d.data(), nullptr);
    EXPECT_EQ(*d.data(), '\0');

    EXPECT_EQ(e.size(), 6);
    ASSERT_NE(e.data(), nullptr);
    EXPECT_EQ(*e.data(), 'W');

    EXPECT_EQ(f.size(), 35);
    ASSERT_NE(f.data(), nullptr);
    EXPECT_EQ(*f.data(), 'i');

    EXPECT_THROW(out_of_range, string g(a, 1));
}

TEST(String, StringViewConstructor)
{
    string a(string_view{});
    string b("Hello, World!"sv);

    EXPECT_EQ(a.size(), 0);
    ASSERT_NE(a.data(), nullptr);
    EXPECT_EQ(*a.data(), '\0');

    EXPECT_EQ(b.size(), 13);
    ASSERT_NE(b.data(), nullptr);
    EXPECT_EQ(*b.data(), 'H');
}

TEST(String, StringViewSubstringConstructor)
{
    string s("Hello, World!"sv, 1, 9);

    EXPECT_EQ(s.size(), 9);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), 'e');
}

TEST(String, NullPointerConstructorDeleted)
{
    EXPECT(!(std::is_constructible_v<string, nullptr_t>));
}

TEST(String, CStringConstructor)
{
    string s("Hello, World!");

    EXPECT_EQ(s.size(), 13);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), 'H');

    EXPECT_THROW(logic_error, string(static_cast<const char*>(nullptr)));
}

TEST(String, PointerAndLengthConstructor)
{
    string s("Hello, World!", 5);

    EXPECT_EQ(s.size(), 5);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), 'H');

    EXPECT_THROW(logic_error, string(static_cast<const char*>(nullptr), 1));
    EXPECT_NO_THROW(string(static_cast<const char*>(nullptr), 0));
}

TEST(String, CharFillConstructor)
{
    string s(20, '?');

    EXPECT_EQ(s.size(), 20);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(s[0], '?');
    EXPECT_EQ(s[19], '?');
}

TEST(String, IteratorPairConstructor)
{
    const char* const cstr = "Hello, World!";

    string s(cstr+1, cstr+5);

    EXPECT_EQ(s.size(), 4);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), 'e');

    EXPECT_THROW(logic_error, string(cstr+1, cstr));
}

TEST(String, FromRangeConstructor)
{
    std::array<char, 14> arr { "Hello, World!" };

    string s(from_range, arr);

    EXPECT_EQ(s.size(), 14);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(s[0], 'H');
    EXPECT_EQ(s[13], '\0');
    EXPECT_EQ(s[14], '\0');
}

TEST(String, InitializerListConstructor)
{
    auto il = {'H', 'e', 'l', 'l', 'o'};
    string s(il);

    EXPECT_EQ(s.size(), 5);
    ASSERT_NE(s.data(), nullptr);
    EXPECT_EQ(s[0], 'H');
}

TEST(String, CopyAssignment)
{
    const string a;
    const string b("Hello, World!");

    string c;
    string d;
    string e("Testing");
    string f("Testing");

    c = a;
    d = b;
    e = a;
    f = b;

    EXPECT_EQ(c.size(), 0);
    ASSERT_NE(c.data(), nullptr);
    EXPECT_EQ(*c.data(), '\0');

    EXPECT_EQ(d.size(), 13);
    ASSERT_NE(d.data(), nullptr);
    EXPECT_EQ(*d.data(), 'H');

    EXPECT_EQ(e.size(), 0);
    ASSERT_NE(e.data(), nullptr);
    EXPECT_EQ(*e.data(), '\0');

    EXPECT_EQ(f.size(), 13);
    ASSERT_NE(f.data(), nullptr);
    EXPECT_EQ(*f.data(), 'H');
}

TEST(String, MoveAssignment)
{
    string a;
    string b1("Hello, World!");
    string b2("Hello, World!");

    string c;
    string d;
    string e("Testing");
    string f("Testing");

    c = std::move(a);
    d = std::move(b1);
    e = std::move(a);
    f = std::move(b2);

    EXPECT_EQ(c.size(), 0);
    ASSERT_NE(c.data(), nullptr);
    EXPECT_EQ(*c.data(), '\0');

    EXPECT_EQ(d.size(), 13);
    ASSERT_NE(d.data(), nullptr);
    EXPECT_EQ(*d.data(), 'H');

    EXPECT_EQ(e.size(), 0);
    ASSERT_NE(e.data(), nullptr);
    EXPECT_EQ(*e.data(), '\0');

    EXPECT_EQ(f.size(), 13);
    ASSERT_NE(f.data(), nullptr);
    EXPECT_EQ(*f.data(), 'H');
}

TEST(String, StringViewAssignment)
{
    string a;
    string b;
    string c("Testing");
    string d("Testing");

    a = string_view{};
    b = "Hello, World!"sv;
    c = string_view{};
    d = "Hello, World!"sv;

    EXPECT_EQ(a.size(), 0);
    ASSERT_NE(a.data(), nullptr);
    EXPECT_EQ(*a.data(), '\0');

    EXPECT_EQ(b.size(), 13);
    ASSERT_NE(b.data(), nullptr);
    EXPECT_EQ(*b.data(), 'H');

    EXPECT_EQ(c.size(), 0);
    ASSERT_NE(c.data(), nullptr);
    EXPECT_EQ(*c.data(), '\0');

    EXPECT_EQ(d.size(), 13);
    ASSERT_NE(d.data(), nullptr);
    EXPECT_EQ(*d.data(), 'H');
}

TEST(String, NullPointerAssignmentDeleted)
{
    EXPECT(!(std::is_assignable_v<string&, nullptr_t>));
}

TEST(String, CStringAssignment)
{
    string a;
    string b("Testing");

    a = "Hello, World!";
    b = "Hello, World!";

    EXPECT_EQ(a.size(), 13);
    ASSERT_NE(a.data(), nullptr);
    EXPECT_EQ(*a.data(), 'H');

    EXPECT_EQ(b.size(), 13);
    ASSERT_NE(b.data(), nullptr);
    EXPECT_EQ(*b.data(), 'H');

    EXPECT_THROW(logic_error, a = static_cast<const char *>(nullptr));
}

TEST(String, CharAssignment)
{
    string a;
    string b("Testing");

    a = '?';
    b = '!';

    EXPECT_EQ(a.size(), 1);
    ASSERT_NE(a.data(), nullptr);
    EXPECT_EQ(*a.data(), '?');

    EXPECT_EQ(b.size(), 1);
    ASSERT_NE(b.data(), nullptr);
    EXPECT_EQ(*b.data(), '!');
}

TEST(String, InitializerListAssignment)
{
    auto il = {'H', 'i'};

    string a;
    string b("Testing");

    a = il;
    b = il;

    EXPECT_EQ(a.size(), 2);
    ASSERT_NE(a.data(), nullptr);
    EXPECT_EQ(*a.data(), 'H');

    EXPECT_EQ(b.size(), 2);
    ASSERT_NE(b.data(), nullptr);
    EXPECT_EQ(*b.data(), 'H');
}

TEST(String, Iterators)
{
    const char* const str = "Hello, World!";

    string empty;
    string s(str);

    EXPECT_NE(empty.begin(), nullptr);
    EXPECT_EQ(empty.end(), empty.begin());

    EXPECT_NE(s.begin(), nullptr);
    EXPECT_EQ(s.end(), s.begin() + 13);

    EXPECT_EQ(empty.begin(), empty.cbegin());
    EXPECT_EQ(empty.end(), empty.cend());
    EXPECT_EQ(s.begin(), s.cbegin());
    EXPECT_EQ(s.end(), s.cend());

    EXPECT_EQ(empty.rbegin(), string_view::reverse_iterator(empty.end()));
    EXPECT_EQ(empty.rend(), string_view::reverse_iterator(empty.begin()));
    EXPECT_EQ(s.rbegin(), string_view::reverse_iterator(s.end()));
    EXPECT_EQ(s.rend(), string_view::reverse_iterator(s.begin()));

    EXPECT_EQ(empty.rbegin(), empty.crbegin());
    EXPECT_EQ(empty.rend(), empty.crend());
    EXPECT_EQ(s.rbegin(), s.crbegin());
    EXPECT_EQ(s.rend(), s.crend());
}

TEST(String, Capacity)
{
    const string a;
    const string b("Hello, World!");

    EXPECT_EQ(a.size(), 0);
    EXPECT_EQ(a.length(), a.size());

    EXPECT_EQ(b.size(), 13);
    EXPECT_EQ(b.length(), b.size());

    EXPECT_EQ(a.max_size(), b.max_size());
}

TEST(String, Resize)
{
    string a;
    string b("Hello, World!");

    a.resize(5, '?');
    b.resize(2);

    EXPECT_EQ(a.size(), 5);
    EXPECT_EQ(a, "?????"sv);

    EXPECT_EQ(b.size(), 2);
    EXPECT_EQ(b, "He"sv);

    b.resize(6);

    EXPECT_EQ(b.size(), 6);
    EXPECT_EQ(b, "He\0\0\0\0"sv);

    // Exercise some size changes.
    a.resize(4096, '!');
    EXPECT_GE(a.capacity(), 4096);
    a.resize(72);
    EXPECT_GE(a.capacity(), 72);
    a.resize(65536, '=');
    EXPECT_GE(a.capacity(), 65536);
    a.resize(0);
    EXPECT_GE(a.capacity(), 0);

    EXPECT_EQ(a.size(), 0);
    EXPECT_EQ(*a.data(), '\0');

    EXPECT_THROW(length_error, a.resize(std::numeric_limits<size_t>::max()));
}

TEST(String, ResizeAndOverwrite)
{
    string a("Hello, World!");
    string b("This is a string long enough to not use SSO");

    a.resize_and_overwrite(5, [](auto p, auto m)
    {
        for (size_t i = 0; i < m; ++i)
            p[i] ^= 0x20;

        return m;
    });

    EXPECT_EQ(a.size(), 5);
    EXPECT_EQ(a, "hELLO"sv);

    b.resize_and_overwrite(4096, [](auto p, auto m)
    {
        std::memcpy(p + 43, " but now it's even longer!", 26);

        return 43 + 26;
    });

    EXPECT_EQ(b.size(), 69);
    EXPECT_EQ(b, "This is a string long enough to not use SSO but now it's even longer!"sv);
}

TEST(String, Reserve)
{
    string s;

    EXPECT_THROW(length_error, s.reserve(std::numeric_limits<size_t>::max()));

    auto sizes = {0U, 1U, 16U, 256U, 4096U, 65536U, 0x100000U, 0x1000000U};

    for (auto size : sizes)
    {
        s.reserve(size);
        EXPECT_GE(s.capacity(), size);
        EXPECT_NE(s.data(), nullptr);
        EXPECT_EQ(*s.data(), '\0');
    }
}

TEST(String, ShrinkToFit)
{
    auto sizes = {0U, 1U, 16U, 256U, 4096U, 65536U, 0x100000U, 0x1000000U};

    for (auto size : sizes)
    {
        // shrink_to_fit may reduce capacity or leave it unchanged but may not increase it.
        string a;
        string b;
        a.reserve(size);
        b.reserve(size);
        a.assign("Hello, World!");
        b.assign("This is a string long enough to not use SSO");

        auto asize = a.capacity();
        auto bsize = b.capacity();

        EXPECT_GE(asize, size);
        EXPECT_GE(bsize, size);

        a.shrink_to_fit();
        b.shrink_to_fit();

        if (a.size() < size)
            EXPECT_LE(a.capacity(), asize);

        if (b.size() < size)
            EXPECT_LE(b.capacity(), bsize);
    }
}

TEST(String, ElementAccess)
{
    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    string empty;
    string a(str1);
    const string b(str2);

    EXPECT_EQ(empty[0], '\0');

    for (size_t i = 0; i < 14; ++i)
        EXPECT_EQ(a[i], str1[i]);

    for (size_t i = 0; i < 13; ++i)
        EXPECT_EQ(a.at(i), str1[i]);

    for (size_t i = 0; i < 44; ++i)
        EXPECT_EQ(b[i], str2[i]);

    for (size_t i = 0; i < 43; ++i)
        EXPECT_EQ(b.at(i), str2[i]);

    EXPECT_THROW(out_of_range, empty.at(0));
    EXPECT_THROW(out_of_range, a.at(13));
    EXPECT_THROW(out_of_range, b.at(43));

    EXPECT_THROW(logic_error, a[14]);
    EXPECT_THROW(logic_error, b[44]);

    EXPECT_EQ(&a.front(), a.data());
    EXPECT_EQ(&a.back(), a.data() + 12);

    EXPECT_EQ(&b.front(), b.data());
    EXPECT_EQ(&b.back(), b.data() + 42);
}

TEST(String, Append)
{
    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    const string a(str1);
    const string b(str2);

    string s;

    s.append(s);

    EXPECT_EQ(s.size(), 0);
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');

    s.append(a);

    EXPECT_EQ(s, "Hello, World!"sv);

    s.append(a);

    EXPECT_EQ(s, "Hello, World!Hello, World!"sv);

    s.append(b);

    EXPECT_EQ(s, "Hello, World!Hello, World!This is a string long enough to not use SSO"sv);

    s.append(s);

    EXPECT_EQ(s, "Hello, World!Hello, World!This is a string long enough to not use SSOHello, World!Hello, World!This is a string long enough to not use SSO"sv);
}

TEST(String, AppendSubstring)
{
    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    const string a(str1);
    const string b(str2);

    string s;

    s.append(s, 0);

    EXPECT_EQ(s.size(), 0);
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');

    s.append(a, 0, 7);

    EXPECT_EQ(s, "Hello, "sv);

    s.append(s, 0, 5);

    EXPECT_EQ(s, "Hello, Hello"sv);

    s.append(a, 12);

    EXPECT_EQ(s, "Hello, Hello!"sv);

    s.append(b, 4, 1);
    s.append(b, 0, 5);

    EXPECT_EQ(s, "Hello, Hello! This "sv);

    s.append(b, 10, 7);
    s.append(b, 5, 2);
    s.append(b, 16);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO"sv);

    s.append(s, 5, 8);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO, Hello!"sv);
}

TEST(String, AppendStringView)
{
    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    const string_view empty;
    const string_view a(str1);
    const string_view b(str2);

    string s;

    s.append(empty);

    EXPECT_EQ(s.size(), 0);
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');

    s.append(a);

    EXPECT_EQ(s, "Hello, World!"sv);

    s.append(a);

    EXPECT_EQ(s, "Hello, World!Hello, World!"sv);

    s.append(b);

    EXPECT_EQ(s, "Hello, World!Hello, World!This is a string long enough to not use SSO"sv);

    s.append(string_view(s));

    EXPECT_EQ(s, "Hello, World!Hello, World!This is a string long enough to not use SSOHello, World!Hello, World!This is a string long enough to not use SSO"sv);
}

TEST(String, AppendStringViewSubstring)
{
    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    const string_view empty;
    const string_view a(str1);
    const string_view b(str2);

    string s;

    s.append(empty, 0);

    EXPECT_EQ(s.size(), 0);
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');

    s.append(a, 0, 7);

    EXPECT_EQ(s, "Hello, "sv);

    s.append(string_view(s), 0, 5);

    EXPECT_EQ(s, "Hello, Hello"sv);

    s.append(a, 12);

    EXPECT_EQ(s, "Hello, Hello!"sv);

    s.append(b, 4, 1);
    s.append(b, 0, 5);

    EXPECT_EQ(s, "Hello, Hello! This "sv);

    s.append(b, 10, 7);
    s.append(b, 5, 2);
    s.append(b, 16);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO"sv);

    s.append(string_view(s), 5, 8);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO, Hello!"sv);
}

TEST(String, AppendCString)
{
    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    string s;

    EXPECT_EQ(s.size(), 0);
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');

    s.append(str1);

    EXPECT_EQ(s, "Hello, World!"sv);

    s.append(str1);

    EXPECT_EQ(s, "Hello, World!Hello, World!"sv);

    s.append(str2);

    EXPECT_EQ(s, "Hello, World!Hello, World!This is a string long enough to not use SSO"sv);

    s.append(s.c_str());

    EXPECT_EQ(s, "Hello, World!Hello, World!This is a string long enough to not use SSOHello, World!Hello, World!This is a string long enough to not use SSO"sv);

    EXPECT_THROW(logic_error, s.append(nullptr));
}

TEST(String, AppendPointerAndLength)
{
    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    string s;

    s.append(nullptr, 0);

    EXPECT_EQ(s.size(), 0);
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');

    s.append(str1, 7);

    EXPECT_EQ(s, "Hello, "sv);

    s.append(s.c_str(), 5);

    EXPECT_EQ(s, "Hello, Hello"sv);

    s.append(str1 + 12);

    EXPECT_EQ(s, "Hello, Hello!"sv);

    s.append(str2 + 4, 1);
    s.append(str2, 5);

    EXPECT_EQ(s, "Hello, Hello! This "sv);

    s.append(str2 + 10, 7);
    s.append(str2 + 5, 2);
    s.append(str2 + 16);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO"sv);

    s.append(s.c_str() + 5, 8);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO, Hello!"sv);
}

TEST(String, AppendIteratorPair)
{
    //! @todo: test non-forward iterators as they have a different (less efficient) implementation.

    const char* const str1 = "Hello, World!";
    const char* const str2 = "This is a string long enough to not use SSO";

    string s;

    EXPECT_EQ(s.size(), 0);
    EXPECT_NE(s.data(), nullptr);
    EXPECT_EQ(*s.data(), '\0');

    s.append(str1, str1 + 7);

    EXPECT_EQ(s, "Hello, "sv);

    s.append(s.c_str(), s.c_str() + 5);

    EXPECT_EQ(s, "Hello, Hello"sv);

    s.append(str1 + 12, str1 + 13);

    EXPECT_EQ(s, "Hello, Hello!"sv);

    s.append(str2 + 4, str2 + 5);
    s.append(str2, str2 + 5);

    EXPECT_EQ(s, "Hello, Hello! This "sv);

    s.append(str2 + 10, str2 + 17);
    s.append(str2 + 5, str2 + 7);
    s.append(str2 + 16, str2 + 43);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO"sv);

    s.append(s.c_str() + 5, s.c_str() + 13);

    EXPECT_EQ(s, "Hello, Hello! This string is long enough to not use SSO, Hello!"sv);
}

TEST(String, AppendInitializerList)
{
    auto il = { 'H', 'e', 'l', 'l', 'o' };

    string s;

    s.append(il);

    EXPECT_EQ(s, "Hello"sv);
}

TEST(String, AppendRepeatedChar)
{
    string s;

    s.append(16, '!');

    EXPECT_EQ(s, "!!!!!!!!!!!!!!!!"sv);
}

TEST(String, AppendRange)
{
    //! @todo: test non-sized ranges as they have a different (less efficient) implementation.

    std::array<char, 14> arr1 { "Hello, World!" };
    std::array<char, 26> arr2 { "I hope it finds you well." };

    string s("I say this to you: "sv);

    s.append_range(arr1);

    EXPECT_EQ(s, "I say this to you: Hello, World!\0"sv);

    s.append_range(arr2);

    EXPECT_EQ(s, "I say this to you: Hello, World!\0I hope it finds you well.\0"sv);

    s.append_range(s);

    EXPECT_EQ(s, "I say this to you: Hello, World!\0I hope it finds you well.\0I say this to you: Hello, World!\0I hope it finds you well.\0"sv);
}

TEST(String, PushBack)
{
    string s;

    s.push_back('H');
    s.push_back('e');
    s.push_back('l');
    s.push_back('l');
    s.push_back('o');

    EXPECT_EQ(s, "Hello"sv);
}

TEST(String, PlusEquals)
{
    string s;

    s += "Hello"sv;

    EXPECT_EQ(s, "Hello"sv);

    s += ',';

    EXPECT_EQ(s, "Hello,"sv);

    s += " World";

    EXPECT_EQ(s, "Hello, World"sv);

    auto il = { '!', ' ' };
    s += il;

    EXPECT_EQ(s, "Hello, World! "sv);

    s += s;

    EXPECT_EQ(s, "Hello, World! Hello, World! "sv);

    s += s;

    EXPECT_EQ(s, "Hello, World! Hello, World! Hello, World! Hello, World! "sv);
}

TEST(String, Assign)
{
    const string empty;
    const string a("Hello, World!");
    const string b("This is a string long enough to not use SSO");

    string s;

    s.assign(empty);

    EXPECT(s.empty());

    s.assign(a);

    EXPECT_EQ(s, a);

    s.assign(empty);

    EXPECT(s.empty());

    s.assign(b);

    EXPECT_EQ(s, b);

    s.assign(empty);

    EXPECT(s.empty());

    s.assign(a);
    s.assign(b);

    EXPECT_EQ(s, b);

    s.assign(a);

    EXPECT_EQ(s, a);

    s.assign(s);
}

TEST(String, AssignRvalue)
{
    const char* const a = "Hello, World!";
    const char* const b = "This is a string long enough to not use SSO";

    string s;

    s.assign(string{});

    EXPECT(s.empty());

    s.assign(string(a));

    EXPECT_EQ(s, a);

    s.assign(string{});

    EXPECT(s.empty());

    s.assign(string(b));

    EXPECT_EQ(s, b);

    s.assign(string{});

    EXPECT(s.empty());

    s.assign(string(a));
    s.assign(string(b));

    EXPECT_EQ(s, b);

    s.assign(string(a));

    EXPECT_EQ(s, a);

    s.assign(std::move(s));
}

TEST(String, AssignSubstring)
{
    const string empty;
    const string a("Hello, World!");
    const string b("This is a string long enough to not use SSO");

    string s;

    s.assign(empty, 0);

    EXPECT(s.empty());

    s.assign(a, 0);

    EXPECT_EQ(s, a);

    s.assign(b, 0);

    EXPECT_EQ(s, b);

    s.assign(a, 7, 5);

    EXPECT_EQ(s, "World"sv);

    s.assign(b, 40, 3);

    EXPECT_EQ(s, "SSO"sv);

    s.assign(b, 0, 30);
    s.assign(s, 10, 6);

    EXPECT_EQ(s, "string"sv);
}

TEST(String, AssignStringView)
{
    const string_view empty;
    const string_view a("Hello, World!");
    const string_view b("This is a string long enough to not use SSO");

    string s;

    s.assign(empty);

    EXPECT(s.empty());

    s.assign(a);

    EXPECT_EQ(s, a);

    s.assign(empty);

    EXPECT(s.empty());

    s.assign(b);

    EXPECT_EQ(s, b);

    s.assign(empty);

    EXPECT(s.empty());

    s.assign(a);
    s.assign(b);

    EXPECT_EQ(s, b);

    s.assign(a);

    EXPECT_EQ(s, a);

    s.assign(string_view(s));
}

TEST(String, AssignStringViewSubstring)
{
    const string_view empty;
    const string_view a("Hello, World!");
    const string_view b("This is a string long enough to not use SSO");

    string s;

    s.assign(empty, 0);

    EXPECT(s.empty());

    s.assign(a, 0);

    EXPECT_EQ(s, a);

    s.assign(b, 0);

    EXPECT_EQ(s, b);

    s.assign(a, 7, 5);

    EXPECT_EQ(s, "World"sv);

    s.assign(b, 40, 3);

    EXPECT_EQ(s, "SSO"sv);

    s.assign(b, 0, 30);
    s.assign(string_view(s), 10, 6);

    EXPECT_EQ(s, "string"sv);
}

TEST(String, AssignCString)
{
    const char* const a = "Hello, World!";
    const char* const b = "This is a string long enough to not use SSO";

    string s;

    s.assign(a);

    EXPECT_EQ(s, a);

    s.assign(b);

    EXPECT_EQ(s, b);

    s.assign(a);

    EXPECT_EQ(s, a);

    s.assign(s.c_str());

    EXPECT_THROW(logic_error, s.assign(nullptr));
}

TEST(String, AssignPointerAndLength)
{
    const char* const a = "Hello, World!";
    const char* const b = "This is a string long enough to not use SSO";

    string s;

    s.assign(nullptr, 0);

    EXPECT(s.empty());

    s.assign(a, 13);

    EXPECT_EQ(s, a);

    s.assign(b, 43);

    EXPECT_EQ(s, b);

    s.assign(a + 7, 5);

    EXPECT_EQ(s, "World"sv);

    s.assign(b + 40, 3);

    EXPECT_EQ(s, "SSO"sv);

    s.assign(b, 30);
    s.assign(s.c_str() + 10, 6);

    EXPECT_EQ(s, "string"sv);
}

TEST(String, AssignIteratorPair)
{
    const char* const a = "Hello, World!";
    const char* const b = "This is a string long enough to not use SSO";

    string s;

    s.assign(a, a + 13);

    EXPECT_EQ(s, a);

    s.assign(b, b + 43);

    EXPECT_EQ(s, b);

    s.assign(a + 7, a + 12);

    EXPECT_EQ(s, "World"sv);

    s.assign(b + 40, b + 43);

    EXPECT_EQ(s, "SSO"sv);

    s.assign(b, b + 30);
    s.assign(s.c_str() + 10, s.c_str() + 16);

    EXPECT_EQ(s, "string"sv);
}

TEST(String, AssignInitializerList)
{
    auto il = { 'H', 'e', 'l', 'l', 'o' };

    string s;

    s.assign(il);

    EXPECT_EQ(s, "Hello"sv);
}

TEST(String, AssignRepeatedChar)
{
    string s;

    s.assign(35, '?');

    EXPECT_EQ(s, "???????????????????????????????????"sv);
}

TEST(String, AssignRange)
{
    std::array<char, 14> arr1 { "Hello, World!" };
    std::array<char, 44> arr2 { "This is a string long enough to not use SSO" };
    std::array<char, 0> empty;

    string s;

    s.assign_range(arr1);

    EXPECT_EQ(s, "Hello, World!\0"sv);

    s.assign_range(arr2);

    EXPECT_EQ(s, "This is a string long enough to not use SSO\0"sv);

    s.assign_range(s);

    s.assign_range(empty);

    EXPECT(s.empty());
}

TEST(String, Insert)
{
    const string a("Beginning");
    const string b(" and end.");
    const string c(", middle");
    const string d("now we're at the ");
    const string e("A story: ");
    const string f(" The End!");

    string s;

    s.insert(0, a);

    EXPECT_EQ(s, "Beginning"sv);

    s.insert(9, b);

    EXPECT_EQ(s, "Beginning and end."sv);

    s.insert(9, c);
    
    EXPECT_EQ(s, "Beginning, middle and end."sv);

    s.insert(22, d);

    EXPECT_EQ(s, "Beginning, middle and now we're at the end."sv);

    s.insert(0, e);

    EXPECT_EQ(s, "A story: Beginning, middle and now we're at the end."sv);

    s.insert(52, f);

    EXPECT_EQ(s, "A story: Beginning, middle and now we're at the end. The End!"sv);

    EXPECT_THROW(out_of_range, s.insert(9999, a));
}


TEST(String, InsertSubstring)
{
    const string a("Hello, World!");
    const string b("This is quite a long string that we'll use to build up a story.");

    string s;

    s.insert(0, a, 0, 5);
    s.insert(5, a, 12);

    EXPECT_EQ(s, "Hello!"sv);

    s.insert(6, b, 62, 1);

    EXPECT_EQ(s, "Hello!."sv);

    s.insert(6, b, 21, 6);

    EXPECT_EQ(s, "Hello!string."sv);

    s.insert(6, a, 6, 1);

    EXPECT_EQ(s, "Hello! string."sv);

    s.insert(7, b, 0, 8);

    EXPECT_EQ(s, "Hello! This is string."sv);

    s.insert(14, b, 13, 2);

    EXPECT_EQ(s, "Hello! This is a string."sv);

    s.insert(16, b, 15, 5);

    EXPECT_EQ(s, "Hello! This is a long string."sv);

    s.insert(16, b, 7, 6);

    EXPECT_EQ(s, "Hello! This is a quite long string."sv);

    s.insert(0, a, 7, 5);

    EXPECT_EQ(s, "WorldHello! This is a quite long string."sv);

    s.insert(5, a, 5, 2);

    EXPECT_EQ(s, "World, Hello! This is a quite long string."sv);

    s.insert(0, s, 7, 5);

    EXPECT_EQ(s, "HelloWorld, Hello! This is a quite long string."sv);

    s.insert(5, s, 10, 2);

    EXPECT_EQ(s, "Hello, World, Hello! This is a quite long string."sv);
}

TEST(String, InsertStringView)
{
    const string_view a("Beginning");
    const string_view b(" and end.");
    const string_view c(", middle");
    const string_view d("now we're at the ");
    const string_view e("A story: ");
    const string_view f(" The End!");

    string s;

    s.insert(0, a);

    EXPECT_EQ(s, "Beginning"sv);

    s.insert(9, b);

    EXPECT_EQ(s, "Beginning and end."sv);

    s.insert(9, c);
    
    EXPECT_EQ(s, "Beginning, middle and end."sv);

    s.insert(22, d);

    EXPECT_EQ(s, "Beginning, middle and now we're at the end."sv);

    s.insert(0, e);

    EXPECT_EQ(s, "A story: Beginning, middle and now we're at the end."sv);

    s.insert(52, f);

    EXPECT_EQ(s, "A story: Beginning, middle and now we're at the end. The End!"sv);

    EXPECT_THROW(out_of_range, s.insert(9999, a));
}

TEST(String, InsertStringViewSubstring)
{
    const string_view a("Hello, World!");
    const string_view b("This is quite a long string that we'll use to build up a story.");

    string s;

    s.insert(0, a, 0, 5);
    s.insert(5, a, 12);

    EXPECT_EQ(s, "Hello!"sv);

    s.insert(6, b, 62, 1);

    EXPECT_EQ(s, "Hello!."sv);

    s.insert(6, b, 21, 6);

    EXPECT_EQ(s, "Hello!string."sv);

    s.insert(6, a, 6, 1);

    EXPECT_EQ(s, "Hello! string."sv);

    s.insert(7, b, 0, 8);

    EXPECT_EQ(s, "Hello! This is string."sv);

    s.insert(14, b, 13, 2);

    EXPECT_EQ(s, "Hello! This is a string."sv);

    s.insert(16, b, 15, 5);

    EXPECT_EQ(s, "Hello! This is a long string."sv);

    s.insert(16, b, 7, 6);

    EXPECT_EQ(s, "Hello! This is a quite long string."sv);

    s.insert(0, a, 7, 5);

    EXPECT_EQ(s, "WorldHello! This is a quite long string."sv);

    s.insert(5, a, 5, 2);

    EXPECT_EQ(s, "World, Hello! This is a quite long string."sv);

    s.insert(0, string_view(s), 7, 5);

    EXPECT_EQ(s, "HelloWorld, Hello! This is a quite long string."sv);

    s.insert(5, string_view(s), 10, 2);

    EXPECT_EQ(s, "Hello, World, Hello! This is a quite long string."sv);
}

TEST(String, InsertCString)
{
    const char* const a = "Beginning";
    const char* const b = " and end.";
    const char* const c = ", middle";
    const char* const d = "now we're at the ";
    const char* const e = "A story: ";
    const char* const f = " The End!";

    string s;

    s.insert(0, a);

    EXPECT_EQ(s, "Beginning"sv);

    s.insert(9, b);

    EXPECT_EQ(s, "Beginning and end."sv);

    s.insert(9, c);
    
    EXPECT_EQ(s, "Beginning, middle and end."sv);

    s.insert(22, d);

    EXPECT_EQ(s, "Beginning, middle and now we're at the end."sv);

    s.insert(0, e);

    EXPECT_EQ(s, "A story: Beginning, middle and now we're at the end."sv);

    s.insert(52, f);

    EXPECT_EQ(s, "A story: Beginning, middle and now we're at the end. The End!"sv);

    EXPECT_THROW(out_of_range, s.insert(9999, a));
}

TEST(String, InsertPointerAndLength)
{
    const char* const a = "Hello, World!";
    const char* const b = "This is quite a long string that we'll use to build up a story.";

    string s;

    s.insert(0, a, 5);
    s.insert(5, a + 12, 1);

    EXPECT_EQ(s, "Hello!"sv);

    s.insert(6, b + 62, 1);

    EXPECT_EQ(s, "Hello!."sv);

    s.insert(6, b + 21, 6);

    EXPECT_EQ(s, "Hello!string."sv);

    s.insert(6, a + 6, 1);

    EXPECT_EQ(s, "Hello! string."sv);

    s.insert(7, b, 8);

    EXPECT_EQ(s, "Hello! This is string."sv);

    s.insert(14, b + 13, 2);

    EXPECT_EQ(s, "Hello! This is a string."sv);

    s.insert(16, b + 15, 5);

    EXPECT_EQ(s, "Hello! This is a long string."sv);

    s.insert(16, b + 7, 6);

    EXPECT_EQ(s, "Hello! This is a quite long string."sv);

    s.insert(0, a + 7, 5);

    EXPECT_EQ(s, "WorldHello! This is a quite long string."sv);

    s.insert(5, a + 5, 2);

    EXPECT_EQ(s, "World, Hello! This is a quite long string."sv);

    s.insert(0, s.c_str() + 7, 5);

    EXPECT_EQ(s, "HelloWorld, Hello! This is a quite long string."sv);

    s.insert(5, s.c_str() + 10, 2);

    EXPECT_EQ(s, "Hello, World, Hello! This is a quite long string."sv);
}

TEST(String, InsertRepeatedChar)
{
    string s("Hello, World!");

    s.insert(12, 5, '!');

    EXPECT_EQ(s, "Hello, World!!!!!!"sv);

    s.insert(2, 7, 'l');

    EXPECT_EQ(s, "Helllllllllo, World!!!!!!"sv);

    s.insert(16, 4, 'r');

    EXPECT_EQ(s, "Helllllllllo, Worrrrrld!!!!!!"sv);
}

TEST(String, InsertCharAt)
{
    string s("Hello World");

    auto iter = s.insert(s.end(), '!');

    EXPECT_EQ(iter, s.begin() + 11);
    EXPECT_EQ(s, "Hello World!"sv);

    iter = s.insert(s.begin() + 5, ',');

    EXPECT_EQ(iter, s.begin() + 5);
    EXPECT_EQ(s, "Hello, World!"sv);

    EXPECT_THROW(logic_error, s.insert(s.end() + 1, 'x'));
}

TEST(String, InsertRepeatedCharAt)
{
    string s("Hello, World!");

    auto iter = s.insert(s.end(), 5, '!');

    EXPECT_EQ(iter, s.begin() + 13);
    EXPECT_EQ(s, "Hello, World!!!!!!"sv);

    iter = s.insert(s.begin() + 2, 7, 'l');

    EXPECT_EQ(iter, s.begin() + 2);
    EXPECT_EQ(s, "Helllllllllo, World!!!!!!"sv);

    iter = s.insert(s.begin() + 16, 4, 'r');

    EXPECT_EQ(iter, s.begin() + 16);
    EXPECT_EQ(s, "Helllllllllo, Worrrrrld!!!!!!"sv);

    EXPECT_THROW(logic_error, s.insert(s.end() + 1, 3, 'x'));
}

TEST(String, InsertIteratorPairAt)
{
    const char* const a = "Hello, World!";
    const char* const b = "This is quite a long string that we'll use to build up a story.";

    string s;

    s.insert(s.begin(), a, a + 5);
    s.insert(s.end(), a + 12, a + 13);

    EXPECT_EQ(s, "Hello!"sv);

    s.insert(s.begin() + 6, b + 62, b + 63);

    EXPECT_EQ(s, "Hello!."sv);

    s.insert(s.begin() + 6, b + 21, b + 27);

    EXPECT_EQ(s, "Hello!string."sv);

    s.insert(s.begin() + 6, a + 6, a + 7);

    EXPECT_EQ(s, "Hello! string."sv);

    s.insert(s.begin() + 7, b, b + 8);

    EXPECT_EQ(s, "Hello! This is string."sv);

    s.insert(s.begin() + 14, b + 13, b + 15);

    EXPECT_EQ(s, "Hello! This is a string."sv);

    s.insert(s.begin() + 16, b + 15, b + 20);

    EXPECT_EQ(s, "Hello! This is a long string."sv);

    s.insert(s.begin() + 16, b + 7, b + 13);

    EXPECT_EQ(s, "Hello! This is a quite long string."sv);

    s.insert(s.begin(), a + 7, a + 12);

    EXPECT_EQ(s, "WorldHello! This is a quite long string."sv);

    s.insert(s.begin() + 5, a + 5, a + 7);

    EXPECT_EQ(s, "World, Hello! This is a quite long string."sv);

    s.insert(s.begin(), s.c_str() + 7, s.c_str() + 12);

    EXPECT_EQ(s, "HelloWorld, Hello! This is a quite long string."sv);

    s.insert(s.begin() + 5, s.c_str() + 10, s.c_str() + 12);

    EXPECT_EQ(s, "Hello, World, Hello! This is a quite long string."sv);

    EXPECT_THROW(logic_error, s.insert(s.end() + 1, a, a));
}

TEST(String, InsertInitializerListAt)
{
    auto il = { 'H', 'e', 'l', 'l', 'o' };

    string s(", World!");

    auto iter = s.insert(s.begin(), il);

    EXPECT_EQ(iter, s.begin());
    EXPECT_EQ(s, "Hello, World!"sv);

    EXPECT_THROW(logic_error, s.insert(s.end() + 1, il));
}

TEST(String, InsertRange)
{
    const std::array<char, 10> a = {"Beginning"};
    const std::array<char, 10> b = {" and end."};
    const std::array<char, 9>  c = {", middle"};
    const std::array<char, 18> d = {"now we're at the "};
    const std::array<char, 10> e = {"A story: "};
    const std::array<char, 10> f = {" The End!"};

    string s;

    s.insert_range(s.begin(), a);

    EXPECT_EQ(s, "Beginning\0"sv);

    s.insert_range(s.end(), b);

    EXPECT_EQ(s, "Beginning\0 and end.\0"sv);

    s.insert_range(s.begin() + 10, c);
    
    EXPECT_EQ(s, "Beginning\0, middle\0 and end.\0"sv);

    s.insert_range(s.begin() + 24, d);

    EXPECT_EQ(s, "Beginning\0, middle\0 and now we're at the \0end.\0"sv);

    s.insert_range(s.begin(), e);

    EXPECT_EQ(s, "A story: \0Beginning\0, middle\0 and now we're at the \0end.\0"sv);

    s.insert_range(s.begin() + 57, f);

    EXPECT_EQ(s, "A story: \0Beginning\0, middle\0 and now we're at the \0end.\0 The End!\0"sv);

    EXPECT_THROW(logic_error, s.insert_range(s.begin() + 9999, a));
}

TEST(String, Erase)
{
    string s("Hello, how are you today? My world is going well!");

    s.erase(34, 14);

    EXPECT_EQ(s, "Hello, how are you today? My world!"sv);

    s.erase(7, 22);

    EXPECT_EQ(s, "Hello, world!"sv);

    s.erase(5);

    EXPECT_EQ(s, "Hello"sv);

    s.erase();

    EXPECT(s.empty());
}

TEST(String, EraseAt)
{
    string s("Hello, World!");

    s.erase(s.begin() + 5);

    EXPECT_EQ(s, "Hello World!"sv);

    s.erase(s.end() - 1);

    EXPECT_EQ(s, "Hello World"sv);

    EXPECT_THROW(logic_error, s.erase(s.end()));
}

TEST(String, EraseIteratorRange)
{
    string s("Hello, how are you today? My world is going well!");

    s.erase(s.begin() + 34, s.begin() + 48);

    EXPECT_EQ(s, "Hello, how are you today? My world!"sv);

    s.erase(s.begin() + 7, s.begin() + 29);

    EXPECT_EQ(s, "Hello, world!"sv);

    s.erase(s.begin() + 5, s.end());

    EXPECT_EQ(s, "Hello"sv);

    s.erase(s.begin(), s.begin());

    EXPECT_EQ(s, "Hello"sv);

    s.erase(s.begin() + 2, s.begin() + 2);

    EXPECT_EQ(s, "Hello"sv);

    s.erase(s.end(), s.end());

    EXPECT_EQ(s, "Hello"sv);

    EXPECT_THROW(logic_error, s.erase(s.end(), s.begin()));

    s.erase(s.begin(), s.end());

    EXPECT(s.empty());
}

TEST(String, PopBack)
{
    string s("123");

    s.pop_back();

    EXPECT_EQ(s, "12"sv);

    s.pop_back();

    EXPECT_EQ(s, "1"sv);

    s.pop_back();

    EXPECT(s.empty());

    EXPECT_THROW(logic_error, s.pop_back());
}

/*TEST(StringView, Modifiers)
{
    const char* const str = "Hello, World!";

    string_view sv(str);

    sv.remove_suffix(1);

    EXPECT_EQ(sv.data(), str);
    EXPECT_EQ(sv.size(), 12);

    sv.remove_prefix(7);

    EXPECT_EQ(sv.data(), str + 7);
    EXPECT_EQ(sv.size(), 5);
}*/

TEST(String, Swap)
{
    const char* const str = "Hello, World!";

    string a;
    string b(str);

    a.swap(b);

    EXPECT_EQ(a.data(), str);
    EXPECT_EQ(a.size(), 13);
    EXPECT_EQ(b.data(), nullptr);
    EXPECT_EQ(b.size(), 0);
}

TEST(String, Copy)
{
    const char* const str = "Hello, World!";
    char buffer[5];

    string s(str);

    s.copy(buffer, 5);

    EXPECT_EQ(std::memcmp(buffer, str, 5), 0);

    s.copy(buffer, 5, 7);

    EXPECT_EQ(std::memcmp(buffer, str + 7, 5), 0);
}

TEST(String, Substr)
{
    const char* const str = "Hello, World!";

    string s(str);

    auto hello = s.substr(0, 5);
    auto world = s.substr(7);

    EXPECT_EQ(hello.data(), str);
    EXPECT_EQ(hello.size(), 5);

    EXPECT_EQ(world.data(), str + 7);
    EXPECT_EQ(world.size(), 6);
}

TEST(String, Compare)
{
    // Comparison should be lexicographic.
    {
        EXPECT_LT("aa"s.compare("b"s), 0);
    }

    // Differ only in length.
    {
        EXPECT_EQ("aa"s.compare("aa"s), 0);
        EXPECT_LT("a"s.compare("aa"s), 0);
        EXPECT_GT("aa"s.compare("a"s), 0);
    }

    // Comparison of a substring.
    {
        EXPECT_EQ("Hello, World!"s.compare(7, 5, "World"s), 0);
    }

    // Comparison of substrings.
    {
        EXPECT_EQ("Hello, World!"s.compare(3, 2, "below"s, 2, 2), 0);
    }

    // Comparison to a C string.
    {
        EXPECT_EQ("Hello, World!"s.compare("Hello, World!"), 0);
    }

    // Comparison of a substring to a C string.
    {
        EXPECT_EQ("Hello, World!"s.compare(7, 5, "World"), 0);
    }

    // Comparison of a substring to a pointer-length pair.
    {
        const char* const str = "below";
        EXPECT_EQ("Hello, World!"s.compare(3, 2, &str[2], 2), 0);
    }
}

TEST(String, StartsWith)
{
    EXPECT("Hello, World!"s.starts_with("Hello"));
    EXPECT(!"Hello, World!"s.starts_with("hi"));

    EXPECT("Hello, World!"s.starts_with('H'));
    EXPECT(!"Hello, World"s.starts_with('!'));

    EXPECT("Hello, World!"s.starts_with("He"sv));
    EXPECT(!"Hello, World!"s.starts_with("Wo"sv));
}

TEST(String, EndsWith)
{
    EXPECT("Hello, World!"s.ends_with("World!"));
    EXPECT(!"Hello, World!"s.ends_with("World?"));

    EXPECT("Hello, World!"s.ends_with('!'));
    EXPECT(!"Hello, World"s.ends_with('H'));

    EXPECT("Hello, World!"s.ends_with("orld!"sv));
    EXPECT(!"Hello, World!"s.ends_with("old!"sv));
}

TEST(String, Contains)
{
    EXPECT("Hello, World!"s.contains("ello"));
    EXPECT(!"Hello, World!"s.contains("old"));

    EXPECT("Hello, World!"s.contains('l'));
    EXPECT(!"Hello, World!"s.contains('a'));

    EXPECT("Hello, World!"s.contains("Wo"sv));
    EXPECT(!"Hello, World!"s.contains("Hwllo"sv));
}

TEST(String, Find)
{
    const char* const str = "Hello, World!";
    
    const string s(str);

    EXPECT_EQ(s.find("World"), 7);
    EXPECT_EQ(s.find('l'), 2);
    EXPECT_EQ(s.find("He"sv), 0);
    EXPECT_EQ(s.find("Ho"), string::npos);
}

TEST(String, Rfind)
{
    const char* const str = "Hello, World!";

    const string s(str);

    EXPECT_EQ(s.rfind("World"), 7);
    EXPECT_EQ(s.rfind('l'), 10);
    EXPECT_EQ(s.rfind("He"sv), 0);
    EXPECT_EQ(s.rfind("Ho"), string::npos);
}

TEST(String, FindFirstOf)
{
    const char* const str = "Hello, World!";

    const string s(str);

    EXPECT_EQ(s.find_first_of(",!"), 5);
    EXPECT_EQ(s.find_first_of(",!", 7), 12);
    EXPECT_EQ(s.find_first_of("xyz"), string_view::npos);
    EXPECT_EQ(s.find_first_of('l'), 2);
    EXPECT_EQ(s.find_first_of('l', 3), 3);
    EXPECT_EQ(s.find_first_of('?'), string_view::npos);
    EXPECT_EQ(s.find_first_of("aeiou"sv), 1);
    EXPECT_EQ(s.find_first_of("aeiou"sv, 7), 8);
    EXPECT_EQ(s.find_first_of("AEIOU"sv), string_view::npos);
    EXPECT_EQ(s.find_first_of("aeiou", 0, 5), 1);
    EXPECT_EQ(s.find_first_of("aeiou", 5, 5), 8);
    EXPECT_EQ(s.find_first_of("aeiou", 2, 3), string_view::npos);
}

TEST(String, FindLastOf)
{
    const char* const str = "Hello, World!";

    const string s(str);

    EXPECT_EQ(s.find_last_of(",!"), 12);
    EXPECT_EQ(s.find_last_of(",!", 7), 5);
    EXPECT_EQ(s.find_last_of("xyz"), string_view::npos);
    EXPECT_EQ(s.find_last_of('l'), 10);
    EXPECT_EQ(s.find_last_of('l', 3), 2);
    EXPECT_EQ(s.find_last_of('?'), string_view::npos);
    EXPECT_EQ(s.find_last_of("aeiou"sv), 8);
    EXPECT_EQ(s.find_last_of("aeiou"sv, 7), 4);
    EXPECT_EQ(s.find_last_of("AEIOU"sv), string_view::npos);
    EXPECT_EQ(s.find_last_of("aeiou", string_view::npos, 5), 8);
    EXPECT_EQ(s.find_last_of("aeiou", 8, 5), 4);
    EXPECT_EQ(s.find_last_of("aeiou", string_view::npos, 1), string_view::npos);
}

TEST(String, FindFirstNotOf)
{
    const char* const str = "Hello, World!";

    const string s(str);

    EXPECT_EQ(s.find_first_not_of("01234567890"), 0);
    EXPECT_EQ(s.find_first_not_of("World", 7), 12);
    EXPECT_EQ(s.find_first_not_of("Helo, Wrd!"), string_view::npos);
    EXPECT_EQ(s.find_first_not_of('H'), 1);
    EXPECT_EQ(s.find_first_not_of('l', 2), 4);
    EXPECT_EQ(s.find_first_not_of('!', 12), string_view::npos);
    EXPECT_EQ(s.find_first_not_of("aeiou"sv), 0);
    EXPECT_EQ(s.find_first_not_of("Word!"sv, 8), 10);
    EXPECT_EQ(s.find_first_not_of(" ,!HWdelor"sv), string_view::npos);
    EXPECT_EQ(s.find_first_not_of("roledWH", 0, 7), 5);
    EXPECT_EQ(s.find_first_not_of("roledWH", 7, 7), 12);
    EXPECT_EQ(s.find_first_not_of(", roledHW", 1, 7), 7);
}

TEST(String, MaxSizeChecking)
{
    u32string u32s;

    EXPECT_THROW(length_error, u32s.reserve((std::numeric_limits<size_t>::max() / sizeof(char32_t)) + 1));
}

TEST(String, DeductionGuides)
{
    const char* const cstr = "Hello, World!";
    const std::array<char, 14> arr { "Hello, World!" };
    const string_view sv(cstr);

    basic_string bsv1(cstr, cstr+13);
    //basic_string bsv2(from_range, arr);
    basic_string bsv3(sv);
    //basic_string bsv4(sv, 7, 5);

    EXPECT_SAME_TYPE(decltype(bsv1), string);
    //EXPECT_SAME_TYPE(decltype(bsv2), string);
    EXPECT_SAME_TYPE(decltype(bsv3), string);
    //EXPECT_SAME_TYPE(decltype(bsv4), string);

    EXPECT_EQ(bsv1.data(), cstr);
    EXPECT_EQ(bsv1.size(), 13);
    //EXPECT_EQ(bsv2.data(), arr.data());
    //EXPECT_EQ(bsv2.size(), arr.size());
    EXPECT_EQ(bsv3.data(), cstr);
    EXPECT_EQ(bsv3.size(), 13);
    //EXPECT_EQ(bsv4.data(), cstr+7);
    //EXPECT_EQ(bsv4.size(), 5);
}

TEST(String, Literals)
{
    auto s = "Hello, World!"s;
    auto ws = L"Hello, World!"s;
    auto u8s = u8"Hello, World!"s;
    auto u16s = u"Hello, World!"s;
    auto u32s = U"Hello, World!"s;

    EXPECT_SAME_TYPE(decltype(s), string);
    EXPECT_SAME_TYPE(decltype(ws), wstring);
    EXPECT_SAME_TYPE(decltype(u8s), u8string);
    EXPECT_SAME_TYPE(decltype(u16s), u16string);
    EXPECT_SAME_TYPE(decltype(u32s), u32string);
}

TEST(String, Comparisons)
{
    auto s = "Hello, World!"s;

    EXPECT(s == s);
    EXPECT(s != "Word hero!"sv);
    EXPECT(s == "Hello, World!");
    EXPECT(s != "Testing 123.");
    EXPECT("Hello, World!" == s);
    EXPECT("Testing 123." != s);

    EXPECT(s < "Word hero!"sv);
    EXPECT("Word hero!"sv > s);

    EXPECT(s <= "Hello, World!");
    EXPECT("Hello, World!" >= s);

    EXPECT((s <=> s) == 0);
    EXPECT((s <=> "Hello, World!") == 0);
    EXPECT(("Word hero!" <=> s) > 0);
}
