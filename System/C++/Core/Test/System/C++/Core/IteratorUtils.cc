#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/IteratorUtils.hh>

#include <compare>
#include <functional>
#include <utility>


//! @todo more comprehensive tests of the invocable concepts.
//! @todo range concepts tests.
//! @todo begin, end, etc tests for class types.


using namespace __XVI_STD_CORE_NS;


struct iter_move_tester
{
    friend constexpr int iter_move(const iter_move_tester&) { return 42; };
};

TEST(IteratorUtils, RangesIterMove)
{
    // Case 1: overloaded iter_move found by ADL.
    {
        EXPECT_EQ(ranges::iter_move(iter_move_tester{}), 42);
    }

    // Case 2a: no ADL iter_move, given an lvalue.
    {
        struct movable
        {
            int val; 
            
            constexpr movable() = default;
            movable(const movable&) = delete;
            movable(movable&& m)
                : val(std::exchange(m.val, 0)) {}
        };

        movable x;
        x.val = 3;
        movable* xptr = &x;

        movable y = ranges::iter_move(xptr);

        EXPECT_EQ(y.val, 3);
        EXPECT_EQ(x.val, 0);
    }

    // Case 2b: no ADL iter_move, given something else.
    {
        int x = 7;
        int y = ranges::iter_move(&x);

        EXPECT_EQ(y, 7);
    }
}

struct iter_swap_tester
{
    int val;
    
    using value_type = int;
    int operator*() const;

    friend constexpr void iter_swap(iter_swap_tester& x, iter_swap_tester& y)
    {
        x.val += 10;
        y.val += 100;
    }
};

struct custom_swap
{
    int val;

    friend constexpr void swap(custom_swap& x, custom_swap& y)
    {
        x.val += 20;
        y.val += 10;
    }
};

template <class> struct iter_swap_data_one;
template <class> struct iter_swap_data_two;

template <class T>
struct iter_swap_data_one
{
    T x;

    constexpr iter_swap_data_one() = default;
    constexpr iter_swap_data_one(const iter_swap_data_one&) = default;
    constexpr iter_swap_data_one(const iter_swap_data_two<T>& t) : x{t.x} {}
    constexpr iter_swap_data_one& operator=(const iter_swap_data_one&) = default;
    constexpr iter_swap_data_one& operator=(const iter_swap_data_two<T>& t) { x = t.x; return *this; }
};

template <class T>
struct iter_swap_data_two
{
    T x;

    constexpr iter_swap_data_two() = default;
    constexpr iter_swap_data_two(const iter_swap_data_two&) = default;
    constexpr iter_swap_data_two(const iter_swap_data_one<T>& o) : x{o.x} {}
    constexpr iter_swap_data_two& operator=(const iter_swap_data_two&) = default;
    constexpr iter_swap_data_two& operator=(const iter_swap_data_one<T>& o) { x = o.x; return *this; };
};

TEST(IteratorUtils, RangesIterSwap)
{
    // Case 1: overloaded iter_swap found by ADL.
    {
        iter_swap_tester a{7};
        iter_swap_tester b{42};

        ranges::iter_swap(a, b);

        EXPECT_EQ(a.val, 17);
        EXPECT_EQ(b.val, 142);
    }

    // Case 2: the parameters are indirectly_readable and their dereferenced values are swappable via ranges::swap.
    {
        custom_swap a{3};
        custom_swap b{17};

        ranges::iter_swap(&a, &b);

        EXPECT_EQ(a.val, 23);
        EXPECT_EQ(b.val, 27);
    }

    // Case 3: the parameters are indirectly_movable_storable.
    {
        iter_swap_data_one<int> a;
        iter_swap_data_two<int> b;

        a.x = 1;
        b.x = 2;

        ranges::iter_swap(&a, &b);

        EXPECT_EQ(a.x, 2);
        EXPECT_EQ(b.x, 1);
    }
}

TEST(IteratorUtils, IterConcept)
{
    struct iter
    {
        using value_type = int;
        using difference_type = int;

        int operator*() const;
        iter& operator++();
        iter operator++(int);
        iter& operator--();
        iter operator--(int);

        bool operator==(iter) const;
    };

    EXPECT_SAME_TYPE(iterator_traits<iter>::iterator_category, input_iterator_tag);
    EXPECT_SAME_TYPE(__detail::_ITER_CONCEPT<iter>, random_access_iterator_tag);
}

TEST(IteratorUtils, ConceptIndirectlyReadable)
{
    struct not_an_iterator {};

    EXPECT(indirectly_readable<int*&>);
    EXPECT(!indirectly_readable<void*>);
    EXPECT(!indirectly_readable<not_an_iterator>);
    EXPECT(indirectly_readable<const not_an_iterator*>);
}

TEST(IteratorUtils, ConceptIndirectlyWritable)
{
    struct not_an_iterator {};

    EXPECT((indirectly_writable<int*, int>));
    EXPECT((!indirectly_writable<const int*, int>));
    EXPECT((!indirectly_writable<void*, int>));
    EXPECT((!indirectly_writable<not_an_iterator, int>));
    EXPECT((indirectly_writable<not_an_iterator*&, not_an_iterator>));
}

struct weakly
{
    using difference_type = signed char;
    weakly& operator++();
    void operator++(int);
};

TEST(IteratorUtils, ConceptWeaklyIncrementable)
{
    EXPECT(weakly_incrementable<int*>);
    EXPECT(!weakly_incrementable<void*>);
    EXPECT(weakly_incrementable<void**>);
    EXPECT(weakly_incrementable<weakly>);
}

 struct increment
{
    using difference_type = signed char;
    increment& operator++();
    increment operator++(int);

    bool operator==(const increment&) const;
};

TEST(IteratorUtils, ConceptIncrementable)
{
    EXPECT(incrementable<int*>);
    EXPECT(!incrementable<void*>);
    EXPECT(incrementable<void**>);
    EXPECT(!incrementable<weakly>);
    EXPECT(incrementable<increment>);
}

struct minimal_sentinel {};
    
struct minimal_iter
{
    using difference_type = int;
    minimal_iter& operator++();
    minimal_iter operator++(int);
    int operator*();

    bool operator==(minimal_sentinel) const;
};

TEST(IteratorUtils, ConceptInputOrOutputIterator)
{
    EXPECT(input_or_output_iterator<int*>);
    EXPECT(!input_or_output_iterator<void*>);
    EXPECT(input_or_output_iterator<minimal_iter>);
}

TEST(IteratorUtils, ConceptSentinelFor)
{
    EXPECT((sentinel_for<int*, int*>));
    EXPECT((!sentinel_for<int, int*>));
    EXPECT((sentinel_for<minimal_sentinel, minimal_iter>));
}

TEST(IteratorUtils, ConceptSizedSentinelFor)
{
    //! @todo add more
    EXPECT((sized_sentinel_for<int*, int*>));
    EXPECT(!(sized_sentinel_for<void*, void*>));
}

struct minimal_output_iterator
{
    using iterator_concept = output_iterator_tag;
    using difference_type = short;
    minimal_output_iterator& operator++();
    minimal_output_iterator& operator++(int);
    int& operator*();
};

struct minimal_input_iterator
{
    using difference_type = short;
    using value_type = int;
    minimal_input_iterator& operator++();
    minimal_input_iterator operator++(int);
    int operator*() const;
};

struct minimal_forward_iterator
{
    using difference_type = short;
    using value_type = int;
    minimal_forward_iterator& operator++();
    minimal_forward_iterator operator++(int);
    int operator*() const;
    bool operator==(const minimal_forward_iterator&) const;
};

struct minimal_bidirectional_iterator
{
    using difference_type = short;
    using value_type = int;
    minimal_bidirectional_iterator& operator++();
    minimal_bidirectional_iterator operator++(int);
    int operator*() const;
    bool operator==(const minimal_bidirectional_iterator&) const;
    minimal_bidirectional_iterator& operator--();
    minimal_bidirectional_iterator operator--(int);
};

struct minimal_random_iterator
{
    using difference_type = ptrdiff_t;
    using value_type = int;
    minimal_random_iterator& operator++();
    minimal_random_iterator operator++(int);
    int operator*() const;
    bool operator==(const minimal_random_iterator&) const;
    minimal_random_iterator& operator--();
    minimal_random_iterator operator--(int);
    std::strong_ordering operator<=>(const minimal_random_iterator&) const;
    difference_type operator-(const minimal_random_iterator&) const;
    minimal_random_iterator& operator+=(difference_type);
    minimal_random_iterator& operator-=(difference_type);
    minimal_random_iterator operator+(difference_type) const;
    minimal_random_iterator operator-(difference_type) const;
    int operator[](difference_type) const;
    friend minimal_random_iterator operator+(difference_type, const minimal_random_iterator&);
};

struct minimal_contiguous_iterator
{
    using difference_type = ptrdiff_t;
    using value_type = int;
    minimal_contiguous_iterator& operator++();
    minimal_contiguous_iterator operator++(int);
    const int& operator*() const;
    bool operator==(const minimal_contiguous_iterator&) const;
    minimal_contiguous_iterator& operator--();
    minimal_contiguous_iterator operator--(int);
    std::strong_ordering operator<=>(const minimal_contiguous_iterator&) const;
    difference_type operator-(const minimal_contiguous_iterator&) const;
    minimal_contiguous_iterator& operator+=(difference_type);
    minimal_contiguous_iterator& operator-=(difference_type);
    minimal_contiguous_iterator operator+(difference_type) const;
    minimal_contiguous_iterator operator-(difference_type) const;
    const int& operator[](difference_type) const;
    friend minimal_contiguous_iterator operator+(difference_type, const minimal_contiguous_iterator&);
    using iterator_concept = contiguous_iterator_tag;
    const int* operator->() const;
};

TEST(IteratorUtils, ConceptOutputIterator)
{
    EXPECT((output_iterator<int*, int>));
    EXPECT(!(output_iterator<const int*, int>));
    EXPECT(!(output_iterator<void*, void>));
    EXPECT((output_iterator<minimal_output_iterator, int>));
    EXPECT(!(output_iterator<minimal_input_iterator, int>));
}

TEST(IteratorUtils, ConceptInputIterator)
{
    EXPECT(input_iterator<int*>);
    EXPECT(!input_iterator<void*>);
    EXPECT(input_iterator<minimal_input_iterator>);
    EXPECT(!input_iterator<minimal_output_iterator>);
}

TEST(IteratorUtils, ConceptForwardIterator)
{
    EXPECT(forward_iterator<int*>);
    EXPECT(!forward_iterator<void*>);
    EXPECT(forward_iterator<minimal_forward_iterator>);
    EXPECT(!forward_iterator<minimal_input_iterator>);
}

TEST(IteratorUtils, ConceptBidirectionalIterator)
{
    EXPECT(bidirectional_iterator<int*>);
    EXPECT(!bidirectional_iterator<void*>);
    EXPECT(bidirectional_iterator<minimal_bidirectional_iterator>);
    EXPECT(!bidirectional_iterator<minimal_forward_iterator>);
}

TEST(IteratorUtils, ConceptRandomAccessIterator)
{
    EXPECT(random_access_iterator<int*>);
    EXPECT(!random_access_iterator<void*>);
    EXPECT(random_access_iterator<minimal_random_iterator>);
    EXPECT(!random_access_iterator<minimal_bidirectional_iterator>);
}

TEST(IteratorUtils, ConceptContiguousIterator)
{
    EXPECT(contiguous_iterator<int*>);
    EXPECT(!contiguous_iterator<void*>);
    EXPECT(contiguous_iterator<minimal_contiguous_iterator>);
    EXPECT(!contiguous_iterator<minimal_random_iterator>);
}

TEST(IteratorUtils, ConceptIndirectlyUnaryInvocable)
{
    EXPECT((indirectly_unary_invocable<void(*)(const int&), const int*>));
}

TEST(IteratorUtils, ConceptIndirectRegularUnaryInvocable)
{
    EXPECT((indirect_regular_unary_invocable<void(*)(const int&), const int*>));
}

TEST(IteratorUtils, ConceptIndirectBinaryPredicate)
{
    EXPECT((indirect_binary_predicate<bool(*)(int, int), const int*, const int*>));
}

TEST(IteratorUtils, ConceptIndirectEquivalenceRelation)
{
    EXPECT((indirect_equivalence_relation<int(*)(int, int), const int*, const int*>));
}

TEST(IteratorUtils, ConceptIndirectStrictWeakOrder)
{
    EXPECT((indirect_strict_weak_order<int(*)(int, int), const int*, const int*>));
}

TEST(IteratorUtils, Projected)
{
    struct double_int
    {
        static int operator()(int x) { return x * 2; };
    };

    EXPECT((indirectly_unary_invocable<double_int, const int*>));

    using Proj = projected<const int*, double_int>;

    EXPECT_SAME_TYPE(Proj::value_type, int);
    EXPECT_SAME_TYPE(decltype(*declval<Proj>()), int);
}

TEST(IteratorUtils, ConceptIndirectlyMovable)
{
    struct data {};
    struct movable_thing { operator data() &&; };
    
    EXPECT((indirectly_movable<const int*, int*>));
    EXPECT(!(indirectly_movable<const int*, const int*>));
    EXPECT(!(indirectly_movable<void*, int*>));
    EXPECT((indirectly_movable<movable_thing*, data*>));
}

TEST(IteratorUtils, ConceptIndirectlyMovableStorable)
{
    EXPECT((indirectly_movable_storable<const int*, int*>));
    EXPECT(!(indirectly_movable_storable<const int*, const int*>));
    EXPECT((indirectly_movable_storable<iter_swap_data_one<int>*, iter_swap_data_two<int>*>));
    EXPECT((indirectly_movable_storable<iter_swap_data_two<int>*, iter_swap_data_one<int>*>));
}

TEST(IteratorUtils, ConceptIndirectlyCopyable)
{
    struct noncopy { void operator=(const noncopy&) = delete; };

    EXPECT((indirectly_copyable<const int*, int*>));
    EXPECT(!(indirectly_copyable<const int*, const int*>));
    EXPECT(!(indirectly_copyable<const noncopy*, noncopy*>));
}

TEST(IteratorUtils, ConceptIndirectlyCopyableStorable)
{
    EXPECT((indirectly_copyable_storable<const int*, int*>));
    EXPECT(!(indirectly_copyable_storable<const int*, const int*>));
}

TEST(IteratorUtils, ConceptIndirectlySwappable)
{
    EXPECT((indirectly_swappable<int*, int*>));
    EXPECT(!(indirectly_swappable<const int*, int*>));
    EXPECT((indirectly_swappable<iter_swap_tester&, iter_swap_tester&>));
    EXPECT((indirectly_swappable<custom_swap*, custom_swap*>));
    EXPECT((indirectly_swappable<iter_swap_data_one<int>*, iter_swap_data_two<int>*>));
}

TEST(IteratorUtils, ConceptIndirectlyComparable)
{
    EXPECT((indirectly_comparable<const int*, const int*, std::equal_to<>>));
    EXPECT(!(indirectly_comparable<const void*, const void*, std::equal_to<>>));
    EXPECT((indirectly_comparable<void**, void**, std::less<>>));
}

TEST(IteratorUtils, ConceptPermutable)
{
    EXPECT((permutable<int*>));
    EXPECT(!(permutable<const int*>));
}

TEST(IteratorUtils, ConceptMergeable)
{
    EXPECT((mergeable<const int*, const int*, int*>));
    EXPECT((mergeable<int**, void**, void**>));
    EXPECT(!(mergeable<const int*, const int*, const int*>));
}

TEST(IteratorUtils, ConceptSortable)
{
    EXPECT((sortable<int*, std::ranges::less>));
    EXPECT(!(sortable<const int*, std::ranges::less>));
}

TEST(IteratorUtils, Advance)
{
    int array[5];

    auto p = &array[0];

    advance(p, 1);
    EXPECT_EQ(p, &array[1]);

    advance(p, 3);
    EXPECT_EQ(p, &array[4]);

    advance(p, -2);
    EXPECT_EQ(p, &array[2]);
}

TEST(IteratorUtils, Distance)
{
    int array[10];

    auto p = &array[0];
    auto q = &array[3];
    auto r = &array[7];
    auto s = &array[9];

    EXPECT_EQ(distance(p, q), 3);
    EXPECT_EQ(distance(q, r), 4);
    EXPECT_EQ(distance(p, s), 9);
    EXPECT_EQ(distance(s, r), -2);
    EXPECT_EQ(distance(r, p), -7);
}

TEST(IteratorUtils, Next)
{
    int array[10];

    auto p = &array[0];
    auto q = &array[3];
    auto r = &array[7];
    auto s = &array[9];

    EXPECT_EQ(next(p, 3), q);
    EXPECT_EQ(next(q, 4), r);
    EXPECT_EQ(next(r, 2), s);
    EXPECT_EQ(next(s, -6), q);
    EXPECT_EQ(next(r, -7), p);
}

TEST(IteratorUtils, Prev)
{
    int array[10];

    auto p = &array[0];
    auto q = &array[3];
    auto r = &array[7];
    auto s = &array[9];

    EXPECT_EQ(prev(p, -3), q);
    EXPECT_EQ(prev(q, -4), r);
    EXPECT_EQ(prev(r, -2), s);
    EXPECT_EQ(prev(s, 6), q);
    EXPECT_EQ(prev(r, 7), p);
}

TEST(IteratorUtils, RangesAdvance)
{
    // Basic advancement by a distance for random-access iterators.
    {
        int array[5];

        auto p = &array[0];

        ranges::advance(p, 1);
        EXPECT_EQ(p, &array[1]);

        ranges::advance(p, 3);
        EXPECT_EQ(p, &array[4]);

        ranges::advance(p, -2);
        EXPECT_EQ(p, &array[2]);
    }

    //! @todo basic advancement by a distance for bidirectional iterators.
    {
    }

    //! @todo basic advancement by a positive distance for non-bidirectional iterators.
    {
    }

    // Advancement to an assignable sentinel.
    {
        int array[5];

        int* p = &array[0];
        int* const s = &array[3];

        ranges::advance(p, s);
        EXPECT_EQ(p, s);
    }

    // Advancement to a non-assignable but sized sentinel.
    {
        int array[5];

        int* p = &array[0];
        const int* const s = &array[3];

        ranges::advance(p, s);
        EXPECT_EQ(p, s);
    }

    //! @todo advancement to a non-assignable, non-sized sentinel.
    {
    }

    // Advancement to a bound or by a distance, whichever is smaller, with a sized sentinel.
    {
        int array[5];

        int* p = &array[0];
        int* const s = &array[3];

        EXPECT_EQ(ranges::advance(p, 2, s), 0);
        EXPECT_EQ(p, &array[2]);

        EXPECT_EQ(ranges::advance(p, 2, s), 1);
        EXPECT_EQ(p, s);

        EXPECT_EQ(ranges::advance(p, 2, s), 2);
    }

    //! @todo advancement to a bound or by a distance, whichever is smaller, with a non-sized sentinel and with a
    //!       bidirectional iterator type.
    {
    }

    //! @todo advancement to a bound or by a distance, whichever is smaller, with a non-sized sentinel and with a
    //!       non-bidirectional iterator type.
}

TEST(IteratorUtils, RangesDistance)
{
    // Distance for an iterator and sized sentinel.
    {
        int array[10];

        auto p = &array[0];
        auto q = &array[3];
        auto r = &array[7];
        auto s = &array[9];

        EXPECT_EQ(ranges::distance(p, q), 3);
        EXPECT_EQ(ranges::distance(q, r), 4);
        EXPECT_EQ(ranges::distance(p, s), 9);
        EXPECT_EQ(ranges::distance(s, r), -2);
        EXPECT_EQ(ranges::distance(r, p), -7);
    }

    //! @todo distance for an iterator and non-sized sentinel.
    {
    }

    //! @todo distance for a sized range.
    {
    }

    //! @todo distance for a non-sized range.
}

TEST(IteratorUtils, RangesNext)
{
    // Next without a distance.
    {
        int array[3];

        auto p = &array[0];
        auto q = &array[1];
        auto r = &array[2];

        EXPECT_EQ(ranges::next(p), q);
        EXPECT_EQ(ranges::next(q), r);
    }
    
    // Next with a distance.
    {
        int array[10];

        auto p = &array[0];
        auto q = &array[3];
        auto r = &array[7];
        auto s = &array[9];

        EXPECT_EQ(ranges::next(p, 3), q);
        EXPECT_EQ(ranges::next(q, 4), r);
        EXPECT_EQ(ranges::next(r, 2), s);
        EXPECT_EQ(ranges::next(s, -6), q);
        EXPECT_EQ(ranges::next(r, -7), p);
    }

    // Next with a sentinel.
    {
        int array[5];

        int* const p = &array[0];
        int* const s = &array[3];

        EXPECT_EQ(ranges::next(p, s), s);
    }

    // Next with a distance and a sentinel.
    {
        int array[5];

        int* const p = &array[0];
        int* const s = &array[3];

        EXPECT_EQ(ranges::next(p, 2, s), &array[2]);
        EXPECT_EQ(ranges::next(p, 5, s), s);
    }
}

TEST(IteratorUtils, RangesPrev)
{
    // Previous without a distance.
    {
        int array[3];

        auto p = &array[0];
        auto q = &array[1];
        auto r = &array[2];

        EXPECT_EQ(ranges::prev(r), q);
        EXPECT_EQ(ranges::prev(q), p);
    }

    // Previous with a distance.
    {
        int array[10];

        auto p = &array[0];
        auto q = &array[3];
        auto r = &array[7];
        auto s = &array[9];

        EXPECT_EQ(ranges::prev(p, -3), q);
        EXPECT_EQ(ranges::prev(q, -4), r);
        EXPECT_EQ(ranges::prev(r, -2), s);
        EXPECT_EQ(ranges::prev(s, 6), q);
        EXPECT_EQ(ranges::prev(r, 7), p);
    }

    // Previous with a distance and a sentinel.
    {
        int array[10];
        
        int* const p = &array[4];
        int* const s = &array[1];

        EXPECT_EQ(ranges::prev(p, 5, s), s);
    }
}

TEST(IteratorUtils, ArrayIteratorFunctions)
{
    int array[10];

    EXPECT_EQ(begin(array), &array[0]);
    EXPECT_EQ(end(array), &array[9] + 1);
    EXPECT_EQ(cbegin(array), begin(array));
    EXPECT_EQ(cend(array), end(array));
    EXPECT_EQ(size(array), 10);
    EXPECT_EQ(ssize(array), 10);
    EXPECT_EQ(data(array), begin(array));
    EXPECT(!empty(array));
}

TEST(IteratorUtils, InitializerListIteratorFunctions)
{
    // Only data and empty have specific initializer list overloads. The others are the normal class-type overloads.
    //
    // We have to use qualified names here otherwise ADL will try to find the functions in the std namespace of the host.
    auto init = {1, 2, 3, 4, 5};

    EXPECT_EQ(__XVI_STD_CORE_NS::begin(init), init.begin());
    EXPECT_EQ(__XVI_STD_CORE_NS::end(init), init.end());
    EXPECT_EQ(__XVI_STD_CORE_NS::cbegin(init), __XVI_STD_CORE_NS::begin(init));
    EXPECT_EQ(__XVI_STD_CORE_NS::cend(init), __XVI_STD_CORE_NS::end(init));
    EXPECT_EQ(__XVI_STD_CORE_NS::size(init), init.size());
    EXPECT_EQ(__XVI_STD_CORE_NS::ssize(init), __XVI_STD_CORE_NS::size(init));
    EXPECT_EQ(__XVI_STD_CORE_NS::data(init), init.begin());
    EXPECT(!__XVI_STD_CORE_NS::empty(init));
}
