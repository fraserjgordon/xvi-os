#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/IteratorAdaptors.hh>


using namespace __XVI_STD_CORE_NS;


//! @todo tests for insert iterators (need some containers!)
//! @todo tests for move_iterator and move_sentinel
//! @todo tests for common_iterator
//! @todo tests for counted_iterator
//! @todo rbegin, rend, etc tests for class types.


TEST(IteratorAdaptors, ReverseIteratorTypes)
{
    using Reverse = reverse_iterator<const int*>;

    EXPECT_SAME_TYPE(Reverse::iterator_type, const int*);
    EXPECT_SAME_TYPE(Reverse::iterator_concept, random_access_iterator_tag);
    EXPECT_SAME_TYPE(Reverse::iterator_category, random_access_iterator_tag);
    EXPECT_SAME_TYPE(Reverse::value_type, int);
    EXPECT_SAME_TYPE(Reverse::difference_type, ptrdiff_t);
    EXPECT_SAME_TYPE(Reverse::pointer, const int*);
    EXPECT_SAME_TYPE(Reverse::reference, const int&);
}

TEST(IteratorAdaptors, ReverseIteratorDefaultConstructor)
{
    using Reverse = reverse_iterator<const int*>;

    Reverse rev;

    EXPECT_EQ(rev.base(), nullptr);
}

TEST(IteratorAdaptors, ReverseIteratorBaseIteratorConstructor)
{
    using Reverse = reverse_iterator<const int*>;

    int array[3];
    Reverse rev(&array[0]);

    EXPECT_EQ(rev.base(), &array[0]);
}

TEST(IteratorAdaptors, ReverseIteratorCopyConstructor)
{
    using R1 = reverse_iterator<int*>;
    using R2 = reverse_iterator<const int*>;

    int array[3];
    const R1 r1a(&array[0]);
    
    R1 r1b(r1a);
    R2 r2(r1a);

    EXPECT_EQ(r1b.base(), &array[0]);
    EXPECT_EQ(r2.base(), &array[0]);
}

TEST(IteratorAdaptors, ReverseIteratorCopyAssignment)
{
    using R1 = reverse_iterator<int*>;
    using R2 = reverse_iterator<const int*>;

    int array[3];
    const R1 r1a(&array[0]);

    R1 r1b;
    R2 r2;

    r1b = r1a;
    r2 = r1a;

    EXPECT_EQ(r1b.base(), &array[0]);
    EXPECT_EQ(r2.base(), &array[0]);
}

TEST(IteratorAdaptors, ReverseIteratorDereference)
{
    using Reverse = reverse_iterator<int*>;

    int array[3] = {1, 2, 3};

    // Note: reverse iterators are off by one compared to the base iterator: they're pointed at the element after the
    //       one that'll be returned when they're dereferenced.
    Reverse rev(&array[1]);

    EXPECT_EQ(*rev, 1);
    EXPECT_EQ(rev.operator->(), &array[0]);
}

TEST(IteratorAdaptors, ReverseIteratorNavigation)
{
    using Reverse = reverse_iterator<int*>;

    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    Reverse rbegin(&array[9] + 1);
    Reverse rend(&array[0]);
    Reverse r(rbegin);

    EXPECT_EQ(rend - rbegin, 10);

    EXPECT_EQ(*r, 9);
    EXPECT_EQ(*(r + 1), 8);
    EXPECT_EQ(*((r + 7) - 2), 4);
    EXPECT_EQ(r + 10, rend);

    EXPECT_EQ(r++, rbegin);
    EXPECT_EQ(*(++r), 7);
    EXPECT_EQ(*(r--), 7);
    EXPECT_EQ(*(--r), 9);

    EXPECT_EQ(*(r += 8), 1);
    EXPECT_EQ(*(r -= 5), 6);

    EXPECT_EQ(*(6 + rbegin), 3);
    EXPECT_EQ(*(-2 + rend), 1);

    for (std::ptrdiff_t i = 0; i < 10; ++i)
        EXPECT_EQ(rbegin[i], 9 - i);

    for (std::ptrdiff_t i = 0; i < 10; ++i)
        EXPECT_EQ(rend[-(i + 1)], i);
}

TEST(IteratorAdaptors, ReverseIteratorComparison)
{
    using Reverse = reverse_iterator<int*>;

    int array[5];

    Reverse rbegin(&array[4] + 1);
    Reverse rend(&array[0]);
    Reverse r(&array[2]);

    EXPECT_EQ(rbegin, rbegin);
    EXPECT_NE(rbegin, rend);

    EXPECT_LT(rbegin, r);
    EXPECT_LT(r, rend);
    EXPECT_LT(rbegin, rend);

    EXPECT_LE(r, rend);
    EXPECT_LE(r, r);

    EXPECT_GT(rend, r);
    EXPECT_GT(r, rbegin);
    EXPECT_GT(rend, rbegin);

    EXPECT_GE(r, rbegin);
    EXPECT_GE(r, r);

    auto comp = rbegin <=> rend;
    EXPECT_EQ(comp, strong_ordering::less);
}

TEST(IteratorAdaptors, ReverseIteratorIterMove)
{
    using Reverse = reverse_iterator<int*>;

    int array[3] = {0, 1, 2};

    Reverse r(&array[2]);

    auto x = ranges::iter_move(r);

    EXPECT_SAME_TYPE(decltype(x), int);
    EXPECT_EQ(x, 1);
}

TEST(IteratorAdaptors, ReverseIteratorIterSwap)
{
    using Reverse = reverse_iterator<int*>;

    int a[1] = {7};
    int b[1] = {42};

    Reverse r(&a[0] + 1);
    Reverse s(&b[0] + 1);

    ranges::iter_swap(r, s);

    EXPECT_EQ(*r, 42);
    EXPECT_EQ(*s, 7);
    EXPECT_EQ(a[0], 42);
    EXPECT_EQ(b[0], 7);
}

TEST(IteratorAdaptors, MakeReverseIterator)
{
    int array[3] = {1, 2, 3};

    auto r = make_reverse_iterator(&array[2] + 1);

    EXPECT_SAME_TYPE(decltype(r), reverse_iterator<int*>);
    EXPECT_EQ(r.base(), &array[2] + 1);
    EXPECT_EQ(*r, 3);
}

TEST(IteratorAdaptors, IterConstReferenceT)
{
    EXPECT_SAME_TYPE(iter_const_reference_t<int*>, const int&);
    EXPECT_SAME_TYPE(iter_const_reference_t<const int*>, const int&);
}

TEST(IteratorAdaptors, ConstIterator)
{
    EXPECT_SAME_TYPE(const_iterator<const int*>, const int*);
    EXPECT_SAME_TYPE(const_iterator<int*>, basic_const_iterator<int*>);
}

TEST(IteratorAdaptors, ConstSentinel)
{
    struct sentinel {};

    EXPECT_SAME_TYPE(const_sentinel<const int*>, const int*);
    EXPECT_SAME_TYPE(const_sentinel<int*>, basic_const_iterator<int*>);
    EXPECT_SAME_TYPE(const_sentinel<sentinel>, sentinel);
}

TEST(IteratorAdaptors, BasicConstIteratorTypes)
{
    using ConstIter = basic_const_iterator<int*>;

    EXPECT_SAME_TYPE(ConstIter::iterator_concept, contiguous_iterator_tag);
    EXPECT_SAME_TYPE(ConstIter::iterator_category, random_access_iterator_tag);
    EXPECT_SAME_TYPE(ConstIter::value_type, int);
    EXPECT_SAME_TYPE(ConstIter::difference_type, ptrdiff_t);
}

TEST(IteratorAdaptors, BasicConstIteratorDefaultConstructor)
{
    using ConstIter = basic_const_iterator<int*>;

    ConstIter i;

    EXPECT_EQ(i.base(), nullptr);
}

TEST(IteratorAdaptors, BasicConstIteratorBaseIteratorConstructor)
{
    using ConstIter = basic_const_iterator<int*>;

    int array[3];

    ConstIter i(&array[0]);

    EXPECT_EQ(i.base(), &array[0]);
}

TEST(IteratorAdaptors, BasicConstIteratorCopyConstructor)
{
    using CI1 = basic_const_iterator<int*>;
    using CI2 = basic_const_iterator<const int*>;

    int array[3];

    const CI1 ci1a(&array[0]);

    CI1 ci1b(ci1a);
    CI2 ci2(ci1a);

    EXPECT_EQ(ci1b.base(), &array[0]);
    EXPECT_EQ(ci2.base(), &array[0]);
}

TEST(IteratorAdaptors, BasicConstIteratorConvertingConstructor)
{
    struct data { int* x; constexpr operator int*() const { return x; } };
    using ConstIter = basic_const_iterator<int*>;

    int array[3];
    data d{.x = &array[1]};

    ConstIter i(d);

    EXPECT_EQ(i.base(), &array[1]);
}

TEST(IteratorAdaptors, BasicConstIteratorDereference)
{
    using ConstIter = basic_const_iterator<int*>;

    int array[3] = {1, 2, 3};

    ConstIter i{&array[1]};

    EXPECT_EQ(*i, 2);
    EXPECT_EQ(i.operator->(), &array[1]);
    EXPECT_SAME_TYPE(decltype(i.operator->()), const int*);
}

TEST(IteratorAdaptors, BasicConstIteratorNavigation)
{
    using ConstIter = basic_const_iterator<int*>;

    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    ConstIter cbegin(&array[0]);
    ConstIter cend(&array[9] + 1);
    ConstIter c(cbegin);

    EXPECT_EQ(cend - cbegin, 10);

    EXPECT_EQ(*c, 0);
    EXPECT_EQ(*(c + 1), 1);
    EXPECT_EQ(*((c + 7) - 2), 5);
    EXPECT_EQ(c + 10, cend);

    EXPECT_EQ(c++, cbegin);
    EXPECT_EQ(*(++c), 2);
    EXPECT_EQ(*(c--), 2);
    EXPECT_EQ(*(--c), 0);

    EXPECT_EQ(*(c += 8), 8);
    EXPECT_EQ(*(c -= 5), 3);

    EXPECT_EQ(*(6 + cbegin), 6);
    EXPECT_EQ(*(-2 + cend), 8);

    for (std::ptrdiff_t i = 0; i < 10; ++i)
        EXPECT_EQ(cbegin[i], i);

    for (std::ptrdiff_t i = 0; i < 10; ++i)
        EXPECT_EQ(cend[-(i + 1)], 9 - i);
}

TEST(IteratorAdaptors, BasicConstIteratorComparison)
{
    using ConstIter = basic_const_iterator<int*>;

    int array[5];

    ConstIter cbegin(&array[0]);
    ConstIter cend(&array[4] + 1);
    ConstIter c(&array[2]);

    EXPECT_EQ(cbegin, cbegin);
    EXPECT_NE(cbegin, cend);
    EXPECT_EQ(cbegin, &array[0]);
    EXPECT_NE(&array[0], cend);

    EXPECT_LT(cbegin, c);
    EXPECT_LT(c, cend);
    EXPECT_LT(cbegin, cend);
    EXPECT_LT(&array[0], c);

    EXPECT_LE(c, cend);
    EXPECT_LE(c, c);
    EXPECT_LE(cbegin, &array[0]);

    EXPECT_GT(cend, c);
    EXPECT_GT(c, cbegin);
    EXPECT_GT(cend, cbegin);
    EXPECT_GT(c, &array[0]);

    EXPECT_GE(c, cbegin);
    EXPECT_GE(c, c);
    EXPECT_GE(&array[0], cbegin);

    auto comp = cbegin <=> cend;
    EXPECT_EQ(comp, strong_ordering::less);

    comp = cbegin <=> &array[0];
    EXPECT_EQ(comp, strong_ordering::equal);
}

TEST(IteratorAdaptors, BasicConstIteratorIterMove)
{
    using ConstIter = basic_const_iterator<int*>;    

    int array[3] = {0, 1, 2};

    ConstIter c(&array[2]);

    auto x = ranges::iter_move(c);

    EXPECT_SAME_TYPE(decltype(x), int);
    EXPECT_EQ(x, 2);
}

TEST(IteratorAdaptors, UnreachableSentinel)
{
    unreachable_sentinel_t sentinel;

    int array[3];

    EXPECT_NE(unreachable_sentinel, &array[0]);
    EXPECT_NE(sentinel, &array[2] + 1);
}

TEST(IteratorAdaptors, ArrayReverseIteratorFunctions)
{
    int array[10];

    EXPECT_SAME_TYPE(decltype(rbegin(array)), reverse_iterator<int*>);
    EXPECT_SAME_TYPE(decltype(rend(array)), reverse_iterator<int*>);
    EXPECT_SAME_TYPE(decltype(crbegin(array)), reverse_iterator<const int*>);
    EXPECT_SAME_TYPE(decltype(crend(array)), reverse_iterator<const int*>);

    EXPECT_EQ(rbegin(array).base(), &array[9] + 1);
    EXPECT_EQ(rend(array).base(), &array[0]);
    EXPECT_EQ(crbegin(array), rbegin(array));
    EXPECT_EQ(crend(array), rend(array));
}

TEST(IteratorAdaptors, InitializerListReverseIteratorFunctions)
{
    auto il = {1, 2, 3, 4, 5};

    // We have to use qualified names here otherwise ADL will try to find the functions in the std namespace of the host.
    EXPECT_SAME_TYPE(decltype(__XVI_STD_NS::rbegin(il)), reverse_iterator<const int*>);
    EXPECT_SAME_TYPE(decltype(__XVI_STD_NS::rend(il)), reverse_iterator<const int*>);
    EXPECT_SAME_TYPE(decltype(__XVI_STD_NS::crbegin(il)), reverse_iterator<const int*>);
    EXPECT_SAME_TYPE(decltype(__XVI_STD_NS::crend(il)), reverse_iterator<const int*>);

    EXPECT_EQ(__XVI_STD_NS::rbegin(il).base(), il.end());
    EXPECT_EQ(__XVI_STD_NS::rend(il).base(), il.begin());
    EXPECT_EQ(__XVI_STD_NS::crbegin(il), __XVI_STD_NS::rbegin(il));
    EXPECT_EQ(__XVI_STD_NS::crend(il), __XVI_STD_NS::rend(il));
}
