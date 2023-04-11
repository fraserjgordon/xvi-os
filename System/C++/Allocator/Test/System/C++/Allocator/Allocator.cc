#include <Tools/Development/Test/Test.hh>

#include <System/C++/Allocator/Allocator.hh>
#include <System/C++/Allocator/AllocatorTraits.hh>

using namespace __XVI_STD_ALLOCATOR_NS;


TEST(Allocator, Types)
{
    using Alloc = allocator<int>;

    EXPECT_SAME_TYPE(Alloc::value_type, int);
    EXPECT_SAME_TYPE(Alloc::size_type, size_t);
    EXPECT_SAME_TYPE(Alloc::difference_type, ptrdiff_t);
    EXPECT_SAME_TYPE(Alloc::propagate_on_container_move_assignment, true_type);
}

TEST(Allocator, IsAlwaysEqual)
{
    using Alloc = allocator<int>;
    using Traits = allocator_traits<Alloc>;

    EXPECT(Traits::is_always_equal::value);
    EXPECT_SAME_TYPE(Alloc::is_always_equal, true_type);
}
