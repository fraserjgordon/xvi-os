#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/Array.hh>


using namespace __XVI_STD_CORE_NS;


TEST(Array, Types)
{
    using A = array<int, 32>;

    EXPECT_SAME_TYPE(A::value_type, int);
    EXPECT_SAME_TYPE(A::pointer, int*);
    EXPECT_SAME_TYPE(A::const_pointer, const int*);
    EXPECT_SAME_TYPE(A::reference, int&);
    EXPECT_SAME_TYPE(A::const_reference, const int&);
    EXPECT_SAME_TYPE(A::size_type, size_t);
    EXPECT_SAME_TYPE(A::difference_type, ptrdiff_t);
    EXPECT_SAME_TYPE(A::reverse_iterator, reverse_iterator<A::iterator>);
    EXPECT_SAME_TYPE(A::const_reverse_iterator, reverse_iterator<A::const_iterator>);

    // These are implementation-defined but this implementation defines them to be pointers.
    EXPECT_SAME_TYPE(A::iterator, int*);
    EXPECT_SAME_TYPE(A::const_iterator, const int*);
}

TEST(Array, Aggregate)
{
    using A = array<int, 8>;

    A a{0, 1, 2, 3, 4, 5, 6, 7};

    EXPECT_EQ(a.size(), 8);
    EXPECT_NE(a.data(), nullptr);
    EXPECT(!a.empty());

    for (size_t i = 0; i < 8; ++i)
        EXPECT_EQ(a[i], int(i));

    // If the operations are trivial, they're compiler-generated and don't need to be tested.
    EXPECT(std::is_trivially_default_constructible_v<A>);
    EXPECT(std::is_trivially_destructible_v<A>);
    EXPECT(std::is_trivially_copyable_v<A>);
    EXPECT(std::is_trivially_move_constructible_v<A>);
    EXPECT(std::is_trivially_move_assignable_v<A>);
    EXPECT(std::is_trivial_v<A>);
    EXPECT(std::is_standard_layout_v<A>);
}

TEST(Array, Fill)
{
    using A = array<int, 8>;

    A a{0, 1, 2, 3, 4, 5, 6, 7};

    a.fill(42);

    for (size_t i = 0; i < 8; ++i)
        EXPECT_EQ(a[i], 42);
}

TEST(Array, Swap)
{
    using A = array<int, 8>;

    A one{0, 1, 2, 3, 4, 5, 6, 7};
    A two{9, 8, 7, 6, 5, 4, 3, 2};

    one.swap(two);

    for (size_t i = 0; i < 8; ++i)
    {
        EXPECT_EQ(one[i], 9 - int(i));
        EXPECT_EQ(two[i], int(i));
    }
}

TEST(Array, IteratorsAndAccessors)
{
    using A = array<int, 8>;

    A a{0, 1, 2, 3, 4, 5, 6, 7};
    const A ca{0, 1, 2, 3, 4, 5, 6, 7};

    EXPECT_SAME_TYPE(decltype(a.begin()), A::iterator);
    EXPECT_SAME_TYPE(decltype(a.end()), A::iterator);
    EXPECT_SAME_TYPE(decltype(a.cbegin()), A::const_iterator);
    EXPECT_SAME_TYPE(decltype(a.cend()), A::const_iterator);
    EXPECT_SAME_TYPE(decltype(a.rbegin()), A::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(a.rend()), A::reverse_iterator);
    EXPECT_SAME_TYPE(decltype(a.crbegin()), A::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(a.crend()), A::const_reverse_iterator);

    EXPECT_SAME_TYPE(decltype(ca.begin()), A::const_iterator);
    EXPECT_SAME_TYPE(decltype(ca.end()), A::const_iterator);
    EXPECT_SAME_TYPE(decltype(ca.cbegin()), A::const_iterator);
    EXPECT_SAME_TYPE(decltype(ca.cend()), A::const_iterator);
    EXPECT_SAME_TYPE(decltype(ca.rbegin()), A::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(ca.rend()), A::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(ca.crbegin()), A::const_reverse_iterator);
    EXPECT_SAME_TYPE(decltype(ca.crend()), A::const_reverse_iterator);

    EXPECT_SAME_TYPE(decltype(a.front()), A::reference);
    EXPECT_SAME_TYPE(decltype(a.back()), A::reference);

    EXPECT_SAME_TYPE(decltype(ca.front()), A::const_reference);
    EXPECT_SAME_TYPE(decltype(ca.back()), A::const_reference);

    EXPECT_SAME_TYPE(decltype(a.data()), A::pointer);

    EXPECT_SAME_TYPE(decltype(ca.data()), A::const_pointer);

    EXPECT_SAME_TYPE(decltype(a[0]), A::reference);
    EXPECT_SAME_TYPE(decltype(a.at(0)), A::reference);

    EXPECT_SAME_TYPE(decltype(ca[0]), A::const_reference);
    EXPECT_SAME_TYPE(decltype(ca.at(0)), A::const_reference);

    EXPECT_EQ(&a.front(), a.data());
    EXPECT_EQ(&a.back(), a.data() + (a.size() - 1));
    EXPECT_EQ(&*a.begin(), a.data());
    EXPECT_EQ(&*a.end(), a.data() + a.size());

    EXPECT_EQ(&ca.front(), ca.data());
    EXPECT_EQ(&ca.back(), ca.data() + (ca.size() - 1));
    EXPECT_EQ(&*ca.begin(), ca.data());
    EXPECT_EQ(&*ca.end(), ca.data() + ca.size());

    for (size_t i = 0; i < 8; ++i)
    {
        auto ii = int(i);

        EXPECT_EQ(a[i], ii);
        EXPECT_EQ(a.at(i), ii);
        EXPECT_EQ(*(a.data() + i), ii);
        EXPECT_EQ(*(a.begin() + i), ii);
        EXPECT_EQ(*(a.end() - (i + 1)), 7 - ii);
        EXPECT_EQ(*(a.cbegin() + i), ii);
        EXPECT_EQ(*(a.cend() - (i + 1)), 7 - ii);
        EXPECT_EQ(*(a.rbegin() + ii), 7 - ii);
        EXPECT_EQ(*(a.rend() - (ii + 1)), ii);
        EXPECT_EQ(*(a.crbegin() + ii), 7 - ii);
        EXPECT_EQ(*(a.crend() - (ii + 1)), ii);

        EXPECT_EQ(ca[i], ii);
        EXPECT_EQ(ca.at(i), ii);
        EXPECT_EQ(*(ca.data() + i), ii);
        EXPECT_EQ(*(ca.begin() + i), ii);
        EXPECT_EQ(*(ca.end() - (i + 1)), 7 - ii);
        EXPECT_EQ(*(ca.cbegin() + i), ii);
        EXPECT_EQ(*(ca.cend() - (i + 1)), 7 - ii);
        EXPECT_EQ(*(ca.rbegin() + ii), 7 - ii);
        EXPECT_EQ(*(ca.rend() - (ii + 1)), ii);
        EXPECT_EQ(*(ca.crbegin() + ii), 7 - ii);
        EXPECT_EQ(*(ca.crend() - (ii + 1)), ii);
    }
}

TEST(Array, DeductionGuide)
{
    array a{0, 1, 2, 3};

    EXPECT_SAME_TYPE(decltype(a), array<int, 4>);
}

TEST(Array, ZeroExtent)
{
    using A = array<int, 0>;

    A a;

    EXPECT_EQ(a.size(), 0);
    EXPECT_EQ(a.begin(), a.end());
}

TEST(Array, ToArray)
{
    int test[3] = {1, 2, 3};

    auto arr = to_array(test);

    EXPECT_SAME_TYPE(decltype(arr), array<int, 3>);

    EXPECT_EQ(arr.size(), 3);

    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
}

TEST(Array, TupleInterface)
{
    using A = array<int, 4>;

    A a{1, 2, 3, 4};

    EXPECT_EQ(tuple_size_v<A>, 4);
    EXPECT_SAME_TYPE(tuple_element_t<0, A>, int);
    EXPECT_SAME_TYPE(tuple_element_t<1, A>, int);
    EXPECT_SAME_TYPE(tuple_element_t<2, A>, int);
    EXPECT_SAME_TYPE(tuple_element_t<3, A>, int);

    EXPECT_EQ(get<0>(a), 1);
    EXPECT_EQ(get<1>(a), 2);
    EXPECT_EQ(get<2>(a), 3);
    EXPECT_EQ(get<3>(a), 4);
}
