#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Span.hh>

using namespace __XVI_STD_CORE_NS;


//! @todo tests for construction from a range and the range deduction guide.


TEST(Span, DynamicExtent)
{
    EXPECT_SAME_TYPE(decltype(dynamic_extent), const size_t);
    EXPECT_EQ(dynamic_extent, numeric_limits<size_t>::max());
}

TEST(Span, Types)
{
    using S = span<const int>;
    using S4 = span<int, 4>;

    EXPECT_SAME_TYPE(S::element_type, const int);
    EXPECT_SAME_TYPE(S::value_type, int);
    EXPECT_SAME_TYPE(S::size_type, size_t);
    EXPECT_SAME_TYPE(S::difference_type, ptrdiff_t);
    EXPECT_SAME_TYPE(S::pointer, const int*);
    EXPECT_SAME_TYPE(S::const_pointer, const int*);
    EXPECT_SAME_TYPE(S::reference, const int&);
    EXPECT_SAME_TYPE(S::const_reference, const int&);
    EXPECT_SAME_TYPE(S::const_iterator, const_iterator<S::iterator>);
    EXPECT_SAME_TYPE(S::reverse_iterator, reverse_iterator<S::iterator>);
    EXPECT_SAME_TYPE(S::const_reverse_iterator, const_iterator<S::reverse_iterator>);

    EXPECT_SAME_TYPE(S4::element_type, int);
    EXPECT_SAME_TYPE(S4::value_type, int);
    EXPECT_SAME_TYPE(S4::size_type, size_t);
    EXPECT_SAME_TYPE(S4::difference_type, ptrdiff_t);
    EXPECT_SAME_TYPE(S4::pointer, int*);
    EXPECT_SAME_TYPE(S4::const_pointer, const int*);
    EXPECT_SAME_TYPE(S4::reference, int&);
    EXPECT_SAME_TYPE(S4::const_reference, const int&);
    EXPECT_SAME_TYPE(S4::const_iterator, const_iterator<S4::iterator>);
    EXPECT_SAME_TYPE(S4::reverse_iterator, reverse_iterator<S4::iterator>);
    EXPECT_SAME_TYPE(S4::const_reverse_iterator, const_iterator<S4::reverse_iterator>);

    // The type of span::iterator is implementation-defined and this implementation defines it as a pointer.
    EXPECT_SAME_TYPE(S::iterator, S::pointer);
    EXPECT_SAME_TYPE(S4::iterator, S4::pointer);

    EXPECT_SAME_TYPE(decltype(S::extent), const size_t);
    EXPECT_SAME_TYPE(decltype(S4::extent), const size_t);
    EXPECT_EQ(S::extent, dynamic_extent);
    EXPECT_EQ(S4::extent, 4);
}

TEST(Span, DefaultConstructor)
{
    using S = span<int>;
    using S0 = span<int, 0>;
    using S4 = span<int, 4>;

    S s;
    S0 s0;

    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.data(), nullptr);

    EXPECT_EQ(s0.size(), 0);
    EXPECT_EQ(s0.data(), nullptr);

    EXPECT(!std::is_default_constructible_v<S4>);
}

TEST(Span, IteratorAndSizeConstructor)
{
    using S = span<int>;
    using S0 = span<int, 0>;
    using S4 = span<int, 4>;

    int array[4];

    S s(&array[0], 4);
    S0 s0(&array[0], 0);
    S4 s4(&array[0], 4);

    EXPECT_EQ(s.size(), 4);
    EXPECT_EQ(s0.size(), 0);
    EXPECT_EQ(s4.size(), 4);

    EXPECT_EQ(s.data(), &array[0]);
    EXPECT_EQ(s0.data(), &array[0]);
    EXPECT_EQ(s4.data(), &array[0]);

    EXPECT((__detail::__is_implicit_constructible_v<S, int*, size_t>));
    EXPECT(!(__detail::__is_implicit_constructible_v<S0, int*, size_t>));
    EXPECT(!(__detail::__is_implicit_constructible_v<S4, int*, size_t>));
}

TEST(Span, IteratorAndSentinelConstructor)
{
    using S = span<int>;
    using S0 = span<int, 0>;
    using S4 = span<int, 4>;

    int array[4];

    auto b = begin(array);
    auto e = end(array);

    S s(b, e);
    S0 s0(b, b);
    S4 s4(b, e);

    EXPECT_EQ(s.size(), 4);
    EXPECT_EQ(s0.size(), 0);
    EXPECT_EQ(s4.size(), 4);

    EXPECT_EQ(s.data(), b);
    EXPECT_EQ(s0.data(), b);
    EXPECT_EQ(s4.data(), b);

    EXPECT((__detail::__is_implicit_constructible_v<S, int*, int*>));
    EXPECT(!(__detail::__is_implicit_constructible_v<S0, int*, int*>));
    EXPECT(!(__detail::__is_implicit_constructible_v<S4, int*, int*>));
}

TEST(Span, ArrayConstructors)
{
    using S = span<int>;
    using S0 = span<int, 0>;
    using S4 = span<int, 4>;

    using A0 = array<int, 0>;
    using A4 = array<int, 4>;

    // C arrays - note that we can't have a zero-length C array in C++.
    {
        int arr4[4];

        S s(arr4);
        S4 s4(arr4);

        EXPECT_EQ(s.size(), 4);
        EXPECT_EQ(s4.size(), 4);

        EXPECT_EQ(s.data(), arr4);
        EXPECT_EQ(s4.data(), arr4);
    }

    // std::array objects.
    {
        A0 a0;
        A4 a4;

        S s(a0);
        S t(a4);

        S0 s0(a0);
        S4 s4(a4);

        EXPECT_EQ(s.size(), 0);
        EXPECT_EQ(t.size(), 4);
        EXPECT_EQ(s0.size(), 0);
        EXPECT_EQ(s4.size(), 4);

        EXPECT_EQ(s.data(), a0.data());
        EXPECT_EQ(t.data(), a4.data());
        EXPECT_EQ(s0.data(), a0.data());
        EXPECT_EQ(s4.data(), a4.data());
    }
}

//! @todo - test for the range constructor (needs a range type!).

TEST(Span, CopyConstructor)
{
    // Trivial - implementation is by the compiler and doesn't need tested.
    EXPECT(std::is_trivially_copyable_v<span<int>>);
}

TEST(Span, ConvertingConstructor)
{
    using S = span<int>;
    using SC = span<const int>;
    using S4 = span<int, 4>;
    using S4C = span<const int, 4>;

    int array[4];

    S s(array);
    S4 s4(array);

    SC sc(s);
    S4C s4c(s4);

    EXPECT_EQ(s.size(), sc.size());
    EXPECT_EQ(s.data(), sc.data());

    EXPECT_EQ(s4.size(), s4c.size());
    EXPECT_EQ(s4.data(), s4c.data());
}

TEST(Span, CopyAssignment)
{
    // Trivial - implementation is by the compiler and doesn't need tested.
    EXPECT(std::is_trivially_copyable_v<span<int>>);
}

TEST(Span, Destructor)
{
    EXPECT(std::is_trivially_destructible_v<span<int>>);
}

TEST(Span, Subviews)
{
    // From a dynamic-extent span.
    {
        using S = span<int>;

        int array[16];

        S s = array;

        auto f5 = s.first<5>();
        auto l3 = s.last<3>();
        auto ss7 = s.subspan<4, 7>();
        auto ss12 = s.subspan<4>();

        auto f = s.first(5);
        auto l = s.last(3);
        auto ss = s.subspan(4, 7);
        auto sse = s.subspan(4);

        EXPECT_SAME_TYPE(decltype(f5), span<int, 5>);
        EXPECT_SAME_TYPE(decltype(l3), span<int, 3>);
        EXPECT_SAME_TYPE(decltype(ss7), span<int, 7>);
        EXPECT_SAME_TYPE(decltype(ss12), span<int>);

        EXPECT_SAME_TYPE(decltype(f), span<int>);
        EXPECT_SAME_TYPE(decltype(l), span<int>);
        EXPECT_SAME_TYPE(decltype(ss), span<int>);
        EXPECT_SAME_TYPE(decltype(sse), span<int>);

        EXPECT_EQ(f5.size(), 5);
        EXPECT_EQ(f5.data(), s.data());
        EXPECT_EQ(l3.size(), 3);
        EXPECT_EQ(l3.data(), s.data() + s.size() - 3);
        EXPECT_EQ(ss7.size(), 7);
        EXPECT_EQ(ss7.data(), s.data() + 4);
        EXPECT_EQ(ss12.size(), 12);
        EXPECT_EQ(ss12.data(), s.data() + 4);

        EXPECT_EQ(f.size(), 5);
        EXPECT_EQ(f.data(), s.data());
        EXPECT_EQ(l.size(), 3);
        EXPECT_EQ(l.data(), s.data() + s.size() - 3);
        EXPECT_EQ(ss.size(), 7);
        EXPECT_EQ(ss.data(), s.data() + 4);
        EXPECT_EQ(sse.size(), 12);
        EXPECT_EQ(sse.data(), s.data() + 4);
    }

    // From a fixed-extent span.
    {
        using S = span<int, 16>;

        int array[16];

        S s = array;

        auto f5 = s.first<5>();
        auto l3 = s.last<3>();
        auto ss7 = s.subspan<4, 7>();
        auto ss12 = s.subspan<4>();

        auto f = s.first(5);
        auto l = s.last(3);
        auto ss = s.subspan(4, 7);
        auto sse = s.subspan(4);

        EXPECT_SAME_TYPE(decltype(f5), span<int, 5>);
        EXPECT_SAME_TYPE(decltype(l3), span<int, 3>);
        EXPECT_SAME_TYPE(decltype(ss7), span<int, 7>);
        EXPECT_SAME_TYPE(decltype(ss12), span<int, 12>);

        EXPECT_SAME_TYPE(decltype(f), span<int>);
        EXPECT_SAME_TYPE(decltype(l), span<int>);
        EXPECT_SAME_TYPE(decltype(ss), span<int>);
        EXPECT_SAME_TYPE(decltype(sse), span<int>);

        EXPECT_EQ(f5.size(), 5);
        EXPECT_EQ(f5.data(), s.data());
        EXPECT_EQ(l3.size(), 3);
        EXPECT_EQ(l3.data(), s.data() + s.size() - 3);
        EXPECT_EQ(ss7.size(), 7);
        EXPECT_EQ(ss7.data(), s.data() + 4);
        EXPECT_EQ(ss12.size(), 12);
        EXPECT_EQ(ss12.data(), s.data() + 4);

        EXPECT_EQ(f.size(), 5);
        EXPECT_EQ(f.data(), s.data());
        EXPECT_EQ(l.size(), 3);
        EXPECT_EQ(l.data(), s.data() + s.size() - 3);
        EXPECT_EQ(ss.size(), 7);
        EXPECT_EQ(ss.data(), s.data() + 4);
        EXPECT_EQ(sse.size(), 12);
        EXPECT_EQ(sse.data(), s.data() + 4);
    }
}

TEST(Span, Observers)
{
    using S = span<int>;
    using S0 = span<int, 0>;
    using S4 = span<int, 4>;

    int arr[4];

    S s(arr, 0);
    S t(arr, 4);
    S0 s0(arr, 0);
    S4 s4(arr, 4);

    EXPECT_EQ(s.size_bytes(), 0);
    EXPECT_EQ(t.size_bytes(), 4 * sizeof(int));
    EXPECT_EQ(s0.size_bytes(), 0);
    EXPECT_EQ(s4.size_bytes(), 4 * sizeof(int));

    EXPECT(s.empty());
    EXPECT(!t.empty());
    EXPECT(s0.empty());
    EXPECT(!s4.empty());
}

TEST(Span, ElementAccess)
{
    using S = span<int>;
    using S0 = span<int, 0>;
    using S4 = span<int, 4>;

    int arr[4] = {0, 1, 2, 3};

    S s(arr, 0);
    S t(arr, 4);
    S0 s0(arr, 0);
    S4 s4(arr, 4);

    const S sc(s);
    const S tc(t);
    const S0 s0c(s0);
    const S4 s4c(s4);

    EXPECT_SAME_TYPE(decltype(s.data()), S::pointer);
    EXPECT_SAME_TYPE(decltype(s0.data()), S0::pointer);
    EXPECT_SAME_TYPE(decltype(s4.data()), S4::pointer);
    EXPECT_SAME_TYPE(decltype(sc.data()), S::pointer);
    EXPECT_SAME_TYPE(decltype(s0.data()), S0::pointer);
    EXPECT_SAME_TYPE(decltype(s4.data()), S4::pointer);

    EXPECT_SAME_TYPE(decltype(s.front()), S::reference);
    EXPECT_SAME_TYPE(decltype(s4.front()), S::reference);
    EXPECT_SAME_TYPE(decltype(s4.front()), S::reference);
    EXPECT_SAME_TYPE(decltype(s4.back()), S::reference);
    EXPECT_SAME_TYPE(decltype(sc.front()), S::reference);
    EXPECT_SAME_TYPE(decltype(s4c.front()), S::reference);
    EXPECT_SAME_TYPE(decltype(s4c.front()), S4::reference);
    EXPECT_SAME_TYPE(decltype(s4c.back()), S4::reference);

    EXPECT_SAME_TYPE(decltype(s[0]), S::reference);
    EXPECT_SAME_TYPE(decltype(s4[0]), S::reference);
    EXPECT_SAME_TYPE(decltype(sc[0]), S::reference);
    EXPECT_SAME_TYPE(decltype(s4c[0]), S::reference);

    EXPECT_EQ(s.data(), arr);
    EXPECT_EQ(t.data(), arr);
    EXPECT_EQ(s0.data(), arr);
    EXPECT_EQ(s4.data(), arr);
    EXPECT_EQ(sc.data(), arr);
    EXPECT_EQ(tc.data(), arr);
    EXPECT_EQ(s0c.data(), arr);
    EXPECT_EQ(s4c.data(), arr);

    EXPECT_EQ(t.front(), 0);
    EXPECT_EQ(t.back(), 3);
    EXPECT_EQ(s4.front(), 0);
    EXPECT_EQ(s4.back(), 3);
    EXPECT_EQ(tc.front(), 0);
    EXPECT_EQ(tc.back(), 3);
    EXPECT_EQ(s4c.front(), 0);
    EXPECT_EQ(s4c.back(), 3);

    for (size_t i = 0; i < 4; ++i)
    {
        auto ii = int(i);

        EXPECT_EQ(t[i], ii);
        EXPECT_EQ(s4[i], ii);
        EXPECT_EQ(tc[i], ii);
        EXPECT_EQ(s4c[i], ii);
    }
}

TEST(Span, Iterators)
{
    using S = span<int>;
    using S0 = span<int, 0>;
    using S4 = span<int, 4>;

    int arr[4] = {0, 1, 2, 3};

    S s(arr, 4);
    S0 s0(arr, 0);
    S4 s4(arr, 4);

    const S sc(s);
    const S0 s0c(s0);
    const S4 s4c(s4);

    EXPECT_SAME_TYPE(decltype(s.begin()), S::iterator);
    EXPECT_SAME_TYPE(decltype(s.end()), S::iterator);
    EXPECT_SAME_TYPE(decltype(s.cbegin()), S::const_iterator);
    EXPECT_SAME_TYPE(decltype(s.cend()), S::const_iterator);
    EXPECT_SAME_TYPE(decltype(s.rbegin()), S::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s.rend()), S::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s.crbegin()), S::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s.crend()), S::const_reverse_iterator);

    EXPECT_SAME_TYPE(decltype(s0.begin()), S0::iterator);
    EXPECT_SAME_TYPE(decltype(s0.end()), S0::iterator);
    EXPECT_SAME_TYPE(decltype(s0.cbegin()), S0::const_iterator);
    EXPECT_SAME_TYPE(decltype(s0.cend()), S0::const_iterator);
    EXPECT_SAME_TYPE(decltype(s0.rbegin()), S0::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s0.rend()), S0::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s0.crbegin()), S0::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s0.crend()), S0::const_reverse_iterator);

    EXPECT_SAME_TYPE(decltype(s4.begin()), S4::iterator);
    EXPECT_SAME_TYPE(decltype(s4.end()), S4::iterator);
    EXPECT_SAME_TYPE(decltype(s4.cbegin()), S4::const_iterator);
    EXPECT_SAME_TYPE(decltype(s4.cend()), S4::const_iterator);
    EXPECT_SAME_TYPE(decltype(s4.rbegin()), S4::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s4.rend()), S4::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s4.crbegin()), S4::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s4.crend()), S4::const_reverse_iterator);

    EXPECT_SAME_TYPE(decltype(sc.begin()), S::iterator);
    EXPECT_SAME_TYPE(decltype(sc.end()), S::iterator);
    EXPECT_SAME_TYPE(decltype(sc.cbegin()), S::const_iterator);
    EXPECT_SAME_TYPE(decltype(sc.cend()), S::const_iterator);
    EXPECT_SAME_TYPE(decltype(sc.rbegin()), S::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(sc.rend()), S::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(sc.crbegin()), S::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(sc.crend()), S::const_reverse_iterator);

    EXPECT_SAME_TYPE(decltype(s0c.begin()), S0::iterator);
    EXPECT_SAME_TYPE(decltype(s0c.end()), S0::iterator);
    EXPECT_SAME_TYPE(decltype(s0c.cbegin()), S0::const_iterator);
    EXPECT_SAME_TYPE(decltype(s0c.cend()), S0::const_iterator);
    EXPECT_SAME_TYPE(decltype(s0c.rbegin()), S0::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s0c.rend()), S0::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s0c.crbegin()), S0::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s0c.crend()), S0::const_reverse_iterator);

    EXPECT_SAME_TYPE(decltype(s4c.begin()), S4::iterator);
    EXPECT_SAME_TYPE(decltype(s4c.end()), S4::iterator);
    EXPECT_SAME_TYPE(decltype(s4c.cbegin()), S4::const_iterator);
    EXPECT_SAME_TYPE(decltype(s4c.cend()), S4::const_iterator);
    EXPECT_SAME_TYPE(decltype(s4c.rbegin()), S4::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s4c.rend()), S4::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s4c.crbegin()), S4::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(s4c.crend()), S4::const_reverse_iterator);

    EXPECT_EQ(&*s.begin(), s.data());
    EXPECT_EQ(&*s.end(), s.data() + s.size());

    EXPECT_EQ(&*s4.begin(), s4.data());
    EXPECT_EQ(&*s4.end(), s4.data() + s4.size());

    EXPECT_EQ(&*sc.begin(), sc.data());
    EXPECT_EQ(&*sc.end(), sc.data() + sc.size());

    EXPECT_EQ(&*sc.begin(), sc.data());
    EXPECT_EQ(&*sc.end(), sc.data() + sc.size());

    for (size_t i = 0; i < 4; ++i)
    {
        auto ii = int(i);

        EXPECT_EQ(*(s.begin() + i), ii);
        EXPECT_EQ(*(s.end() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(s.cbegin() + i), ii);
        EXPECT_EQ(*(s.cend() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(s.rbegin() + ii), 3 - ii);
        EXPECT_EQ(*(s.rend() - (ii + 1)), ii);
        EXPECT_EQ(*(s.crbegin() + ii), 3 - ii);
        EXPECT_EQ(*(s.crend() - (ii + 1)), ii);

        EXPECT_EQ(*(s4.begin() + i), ii);
        EXPECT_EQ(*(s4.end() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(s4.cbegin() + i), ii);
        EXPECT_EQ(*(s4.cend() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(s4.rbegin() + ii), 3 - ii);
        EXPECT_EQ(*(s4.rend() - (ii + 1)), ii);
        EXPECT_EQ(*(s4.crbegin() + ii), 3 - ii);
        EXPECT_EQ(*(s4.crend() - (ii + 1)), ii);

        EXPECT_EQ(*(sc.begin() + i), ii);
        EXPECT_EQ(*(sc.end() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(sc.cbegin() + i), ii);
        EXPECT_EQ(*(sc.cend() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(sc.rbegin() + ii), 3 - ii);
        EXPECT_EQ(*(sc.rend() - (ii + 1)), ii);
        EXPECT_EQ(*(sc.crbegin() + ii), 3 - ii);
        EXPECT_EQ(*(sc.crend() - (ii + 1)), ii);

        EXPECT_EQ(*(s4c.begin() + i), ii);
        EXPECT_EQ(*(s4c.end() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(s4c.cbegin() + i), ii);
        EXPECT_EQ(*(s4c.cend() - (i + 1)), 3 - ii);
        EXPECT_EQ(*(s4c.rbegin() + ii), 3 - ii);
        EXPECT_EQ(*(s4c.rend() - (ii + 1)), ii);
        EXPECT_EQ(*(s4c.crbegin() + ii), 3 - ii);
        EXPECT_EQ(*(s4c.crend() - (ii + 1)), ii);
    }
}

TEST(Span, DeductionGuides)
{
    int arr[4];
    int* b = &arr[0];
    int* e = b + 4;

    array<int, 4> a{};
    const array<int, 4> ac{};

    // Deduction from iterator+sentinel or iterator+length.
    EXPECT_SAME_TYPE(decltype(span(b, e)), span<int>);
    EXPECT_SAME_TYPE(decltype(span(b, 4)), span<int>);

    // Deduction from C array.
    EXPECT_SAME_TYPE(decltype(span(arr)), span<int, 4>);

    // Deduction from mutable & immutable std::array.
    EXPECT_SAME_TYPE(decltype(span(a)), span<int, 4>);
    EXPECT_SAME_TYPE(decltype(span(ac)), span<const int, 4>);

    // Range deduction.
    //! @todo tests range deduction guide
}

TEST(Span, EnableView)
{
    EXPECT(ranges::enable_view<span<int>>);
}

TEST(Span, EnableBorrowedRange)
{
    EXPECT(ranges::enable_borrowed_range<span<int>>);
}

TEST(Span, AsBytes)
{
    using S = span<int>;
    using S4 = span<int, 4>;

    int test[] = {1, 2, 3, 4};

    S s(test);
    S4 s4(test);

    auto sb = as_bytes(s);
    auto s4b = as_bytes(s4);

    EXPECT_SAME_TYPE(decltype(sb), span<const byte>);
    EXPECT_SAME_TYPE(decltype(s4b), span<const byte, 4 * sizeof(int)>);

    EXPECT_EQ(sb.data(), reinterpret_cast<const byte*>(s.data()));
    EXPECT_EQ(s4b.data(), reinterpret_cast<const byte*>(s4.data()));
}

TEST(Span, AsWritableBytes)
{
    using S = span<int>;
    using S4 = span<int, 4>;

    int test[] = {1, 2, 3, 4};

    S s(test);
    S4 s4(test);

    auto sb = as_writable_bytes(s);
    auto s4b = as_writable_bytes(s4);

    EXPECT_SAME_TYPE(decltype(sb), span<byte>);
    EXPECT_SAME_TYPE(decltype(s4b), span<byte, 4 * sizeof(int)>);

    EXPECT_EQ(sb.data(), reinterpret_cast<byte*>(s.data()));
    EXPECT_EQ(s4b.data(), reinterpret_cast<byte*>(s4.data()));
}

TEST(Span, IsRange)
{
    EXPECT(ranges::contiguous_range<span<int>>);
}
