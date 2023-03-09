#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/ReferenceWrapper.hh>

#include <type_traits>


using namespace __XVI_STD_CORE_NS;


TEST(ReferenceWrapper, RefStorage)
{
    int var = 42;
    auto wrapped = ref(var);

    EXPECT_EQ(&var, &wrapped.get());
    EXPECT_EQ(&var, &static_cast<int&>(wrapped));
}


TEST(ReferenceWrapper, DeductionGuide)
{
    int meow = 42;
    const int quux = 0;

    EXPECT_SAME_TYPE(decltype(reference_wrapper(meow)), reference_wrapper<int>);
    EXPECT_SAME_TYPE(decltype(reference_wrapper(quux)), reference_wrapper<const int>);
}


TEST(ReferenceWrapper, Ref)
{
    int meow = 42;
    const int quux = 0;

    EXPECT_SAME_TYPE(reference_wrapper<int>, decltype(ref(meow)));
    EXPECT_SAME_TYPE(reference_wrapper<const int>, decltype(ref(quux)));

    EXPECT_SAME_TYPE(reference_wrapper<int>, decltype(ref(ref(meow))));
}

TEST(ReferenceWrapper, Cref)
{
    int meow = 42;
    const int quux = 0;

    EXPECT_SAME_TYPE(reference_wrapper<const int>, decltype(cref(meow)));
    EXPECT_SAME_TYPE(reference_wrapper<const int>, decltype(cref(quux)));

    EXPECT_SAME_TYPE(reference_wrapper<const int>, decltype(cref(cref(meow))));
}
