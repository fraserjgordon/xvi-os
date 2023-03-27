#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/IteratorTraits.hh>


using namespace __XVI_STD_CORE_NS;


struct custom_type1 {};
struct custom_type2 {};

namespace __XVI_STD_CORE_NS
{

template <>
struct incrementable_traits<custom_type1> { using difference_type = long; };

template <>
struct incrementable_traits<custom_type2> { using difference_type = long; };

template <>
struct indirectly_readable_traits<custom_type1> { using value_type = short; };

template <>
struct indirectly_readable_traits<custom_type2> { using value_type = int; };

template <>
struct iterator_traits<custom_type2>
{
    using difference_type = short;
    using value_type = unsigned;
};

};


struct empty_mixin {};

template <class Mixin = empty_mixin>
struct cpp17_output_iterator : public Mixin
{
    int& operator*();
    cpp17_output_iterator& operator++();
    cpp17_output_iterator operator++(int);
};

template <class Mixin = empty_mixin>
struct cpp17_input_iterator : public Mixin
{
    // Basic iterator operations.
    int operator*();
    cpp17_input_iterator& operator++();
    cpp17_input_iterator operator++(int);

    // Input iterator additions.
    using difference_type = short;
    using value_type = int;
    bool operator==(const cpp17_input_iterator&) const;
};

template <class Mixin = empty_mixin>
struct cpp17_forward_iterator : public Mixin
{
    // Basic iterator operations.
    int& operator*(); // Changed: now returns a reference type.
    cpp17_forward_iterator& operator++();
    cpp17_forward_iterator operator++(int);

    // Input iterator additions.
    using difference_type = short;
    using value_type = int;
    bool operator==(const cpp17_forward_iterator&) const;
};

template <class Mixin = empty_mixin>
struct cpp17_bidirectional_iterator : public Mixin
{
    // Basic iterator operations.
    int& operator*();
    cpp17_bidirectional_iterator& operator++();
    cpp17_bidirectional_iterator operator++(int);

    // Input iterator additions.
    using difference_type = short;
    using value_type = int;
    bool operator==(const cpp17_bidirectional_iterator&) const;

    // Bidirectional iterator additions.
    cpp17_bidirectional_iterator& operator--();
    cpp17_bidirectional_iterator operator--(int);
};

template <class Mixin = empty_mixin>
struct cpp17_random_access_iterator : public Mixin
{
    // Basic iterator operations.
    int& operator*(); // Changed: now returns a reference type.
    cpp17_random_access_iterator& operator++();
    cpp17_random_access_iterator operator++(int);

    // Input iterator additions.
    using difference_type = short;
    using value_type = int;
    bool operator==(const cpp17_random_access_iterator&) const;

    // Bidirectional iterator additions.
    cpp17_random_access_iterator& operator--();
    cpp17_random_access_iterator operator--(int);

    // Random-access iterator additions.
    std::strong_ordering operator<=>(const cpp17_random_access_iterator&) const;
    cpp17_random_access_iterator& operator+=(difference_type);
    cpp17_random_access_iterator& operator-=(difference_type);
    cpp17_random_access_iterator operator+(difference_type) const;
    cpp17_random_access_iterator operator-(difference_type) const;
    difference_type operator-(const cpp17_random_access_iterator&) const;
    value_type& operator[](difference_type);
    template <class M> friend cpp17_random_access_iterator operator+(difference_type, const cpp17_random_access_iterator<M>&);
};


TEST(IteratorTraits, IterReferenceT)
{
    struct iter1 { short operator*() const; };
    struct iter2 { long& operator*(); };

    EXPECT_SAME_TYPE(iter_reference_t<int*>, int&);
    EXPECT_SAME_TYPE(iter_reference_t<iter1>, short);
    EXPECT_SAME_TYPE(iter_reference_t<iter2>, long&);
}

TEST(IteratorTraits, IncrementableTraits)
{
    struct incrementable { using difference_type = int; };
    struct subtractable { long operator-(const subtractable&) const; };

    // Partial specialization for T*.
    EXPECT_SAME_TYPE(typename incrementable_traits<int*>::difference_type, ptrdiff_t);

    // Partial specialization for const T.
    EXPECT((std::is_base_of_v<incrementable_traits<int*>, incrementable_traits<int* const>>));

    // Specialization for types with a difference_type type name.
    EXPECT_SAME_TYPE(typename incrementable_traits<incrementable>::difference_type, int);

    // Specialization for types that implement the subtract operator.
    EXPECT_SAME_TYPE(typename incrementable_traits<subtractable>::difference_type, long);
}

TEST(IteratorTraits, IterDifferenceT)
{
    // No user-supplied specialization of iterator_traits - use incrementable_traits.
    EXPECT_SAME_TYPE(iter_difference_t<int*>, ptrdiff_t);

    // As above but testing that iter_difference_t uses remove_cvref.
    EXPECT_SAME_TYPE(iter_difference_t<int* const&>, ptrdiff_t);

    // User-supplied specialization of incrementable_traits.
    EXPECT_SAME_TYPE(iter_difference_t<custom_type1>, long);

    // Both incrementable_traits and iterator_traits are specialized - should use iterator_traits' typedef.
    EXPECT_SAME_TYPE(iter_difference_t<custom_type2>, short);
}

TEST(IteratorTraits, IndirectlyReadableTraits)
{
    struct member_value_type { using value_type = int; };
    struct member_element_type { using element_type = long; };
    struct both_member_types_same { using value_type = int; using element_type = const int; };
    struct both_member_types_different { using value_type = int; using element_type = long; };
    struct not_indirectly_readable { using value_type = void; };

    // Specialization using a member value_type type that isn't void.
    EXPECT_SAME_TYPE(typename indirectly_readable_traits<member_value_type>::value_type, int);

    // Specialization using a member value_type that is void.
    EXPECT(!__detail::__has_member_value_type<indirectly_readable_traits<not_indirectly_readable>>);

    // Specialization using a member element_type type.
    EXPECT_SAME_TYPE(typename indirectly_readable_traits<member_element_type>::value_type, long);

    // Specialization for types that have both value_type and element_type types as the same type.
    EXPECT_SAME_TYPE(typename indirectly_readable_traits<both_member_types_same>::value_type, int);

    // Specialization for types that have both value_type and element_types types but they're different.
    EXPECT(!__detail::__has_member_value_type<indirectly_readable_traits<both_member_types_different>>);

    // Partial specialization for T*.
    EXPECT_SAME_TYPE(typename indirectly_readable_traits<int*>::value_type, int);

    // Partial specialization for const T.
    EXPECT((std::is_base_of_v<indirectly_readable_traits<int*>, indirectly_readable_traits<int* const>>));

    // Partial specialization for arrays.
    EXPECT_SAME_TYPE(typename indirectly_readable_traits<const int[]>::value_type, int);
}

TEST(IteratorTraits, IterValueT)
{
    // No user-supplied specialization of iterator_traits - use indirectly_readable_traits.
    EXPECT_SAME_TYPE(iter_value_t<int*>, int);

    // As above but testing that iter_value_t uses remove_cvref.
    EXPECT_SAME_TYPE(iter_value_t<int* const&>, int);

    // User-supplied specialization of indirectly_readable_traits.
    EXPECT_SAME_TYPE(iter_value_t<custom_type1>, short);

    // Both indirectly_readable_traits and iterator_traits are specialized - should use iterator_traits' typedef.
    EXPECT_SAME_TYPE(iter_value_t<custom_type2>, unsigned);
}

TEST(IteratorTraits, Cpp17IteratorConcepts)
{
    // These are internal-use concepts but are tested separately here due to how important they are.
    EXPECT(__detail::__cpp17_iterator<cpp17_output_iterator<>>);
    EXPECT(!__detail::__cpp17_input_iterator<cpp17_output_iterator<>>);
    EXPECT(__detail::__cpp17_input_iterator<cpp17_input_iterator<>>);
    EXPECT(!__detail::__cpp17_forward_iterator<cpp17_input_iterator<>>);
    EXPECT(__detail::__cpp17_forward_iterator<cpp17_forward_iterator<>>);
    EXPECT(!__detail::__cpp17_bidirectional_iterator<cpp17_forward_iterator<>>);
    EXPECT(__detail::__cpp17_bidirectional_iterator<cpp17_bidirectional_iterator<>>);
    EXPECT(!__detail::__cpp17_random_access_iterator<cpp17_bidirectional_iterator<>>);
    EXPECT(__detail::__cpp17_random_access_iterator<cpp17_random_access_iterator<>>);
}

TEST(IteratorTraits, IteratorTraitsFromCpp17Concepts)
{
    // Output iterators.
    using output_traits = iterator_traits<cpp17_output_iterator<>>;
    EXPECT_SAME_TYPE(output_traits::iterator_category, output_iterator_tag);
    EXPECT_SAME_TYPE(output_traits::value_type, void);
    EXPECT_SAME_TYPE(output_traits::difference_type, void);
    EXPECT_SAME_TYPE(output_traits::pointer, void);
    EXPECT_SAME_TYPE(output_traits::reference, void);

    // Input iterators.
    using input_traits = iterator_traits<cpp17_input_iterator<>>;
    EXPECT_SAME_TYPE(input_traits::iterator_category, input_iterator_tag);
    EXPECT_SAME_TYPE(input_traits::value_type, int);
    EXPECT_SAME_TYPE(input_traits::difference_type, short);
    EXPECT_SAME_TYPE(input_traits::pointer, void);
    EXPECT_SAME_TYPE(input_traits::reference, int);

    // Forward iterators.
    using forward_traits = iterator_traits<cpp17_forward_iterator<>>;
    EXPECT_SAME_TYPE(forward_traits::iterator_category, forward_iterator_tag);
    EXPECT_SAME_TYPE(forward_traits::value_type, int);
    EXPECT_SAME_TYPE(forward_traits::difference_type, short);
    EXPECT_SAME_TYPE(forward_traits::pointer, void);
    EXPECT_SAME_TYPE(forward_traits::reference, int&);

    // Bidirectional iterators.
    using bidirectional_traits = iterator_traits<cpp17_bidirectional_iterator<>>;
    EXPECT_SAME_TYPE(bidirectional_traits::iterator_category, bidirectional_iterator_tag);
    EXPECT_SAME_TYPE(bidirectional_traits::value_type, int);
    EXPECT_SAME_TYPE(bidirectional_traits::difference_type, short);
    EXPECT_SAME_TYPE(bidirectional_traits::pointer, void);
    EXPECT_SAME_TYPE(bidirectional_traits::reference, int&);

    // Random-access iterators.
    using random_traits = iterator_traits<cpp17_random_access_iterator<>>;
    EXPECT_SAME_TYPE(random_traits::iterator_category, random_access_iterator_tag);
    EXPECT_SAME_TYPE(random_traits::value_type, int);
    EXPECT_SAME_TYPE(random_traits::difference_type, short);
    EXPECT_SAME_TYPE(random_traits::pointer, void);
    EXPECT_SAME_TYPE(random_traits::reference, int&);

    // Check that the iterator category is overrideable.
    struct force_input_category { using iterator_category = input_iterator_tag; };
    EXPECT_SAME_TYPE(iterator_traits<cpp17_forward_iterator<force_input_category>>::iterator_category, input_iterator_tag);

    // Check that the pointer type is overridable via a typedef.
    struct force_pointer_type { using pointer = long*; };
    EXPECT_SAME_TYPE(iterator_traits<cpp17_input_iterator<force_pointer_type>>::pointer, long*);

    // Check that the pointer type is overridable via adding operator->.
    struct add_arrow_operator { const void* operator->() const; };
    EXPECT_SAME_TYPE(iterator_traits<cpp17_input_iterator<add_arrow_operator>>::pointer, const void*);

    // Check that the reference type is overridable.
    struct force_reference_type { using reference = volatile long&; };
    EXPECT_SAME_TYPE(iterator_traits<cpp17_input_iterator<force_reference_type>>::reference, volatile long&);

    // Check that the difference type is overridable on output iterators.
    struct force_difference_type { using difference_type = signed char; };
    EXPECT_SAME_TYPE(iterator_traits<cpp17_output_iterator<force_difference_type>>::difference_type, signed char);
}

TEST(IteratorTraits, IteratorTraitsFromIteratorTypeDefs)
{
    struct iterator_with_types
    {
        using iterator_category = forward_iterator_tag;
        using value_type = unsigned char;
        using difference_type = short;
        using reference = unsigned long&;
    };

    struct iterator_with_all_types : iterator_with_types
    {
        using pointer = const void*;
    };

    using traits = iterator_traits<iterator_with_types>;
    EXPECT_SAME_TYPE(traits::iterator_category, forward_iterator_tag);
    EXPECT_SAME_TYPE(traits::value_type, unsigned char);
    EXPECT_SAME_TYPE(traits::difference_type, short);
    EXPECT_SAME_TYPE(traits::pointer, void);
    EXPECT_SAME_TYPE(traits::reference, unsigned long&);

    using all_traits = iterator_traits<iterator_with_all_types>;
    EXPECT_SAME_TYPE(all_traits::pointer, const void*);
}

TEST(IteratorTraits, IteratorTraitsForPointers)
{
    using traits = iterator_traits<const int*>;

    EXPECT_SAME_TYPE(traits::iterator_concept, contiguous_iterator_tag);
    EXPECT_SAME_TYPE(traits::iterator_category, random_access_iterator_tag);
    EXPECT_SAME_TYPE(traits::value_type, int);
    EXPECT_SAME_TYPE(traits::difference_type, ptrdiff_t);
    EXPECT_SAME_TYPE(traits::pointer, const int*);
    EXPECT_SAME_TYPE(traits::reference, const int&);
}
