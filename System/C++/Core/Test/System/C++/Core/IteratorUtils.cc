#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/IteratorUtils.hh>

#include <compare>
#include <utility>


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
