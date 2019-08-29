#include <System/C++/TypeTraits/TypeTraits.hh>

using namespace __XVI_STD_NS;


static_assert(is_same_v<add_lvalue_reference_t<int>, int&>);
static_assert(is_same_v<add_lvalue_reference_t<void(int)noexcept>, void(&)(int)noexcept>);
static_assert(is_same_v<add_lvalue_reference_t<void(int)const>, void(int)const>);

static_assert(is_same_v<add_rvalue_reference_t<int&>, int&>);

static_assert(is_same_v<make_signed_t<char>, signed char>);
static_assert(is_same_v<make_signed_t<char32_t>, int>);

static_assert(is_same_v<add_pointer_t<int>, int*>);
static_assert(is_same_v<add_pointer_t<void(int)noexcept>, void(*)(int)noexcept>);
static_assert(is_same_v<add_pointer_t<void(int)const>, void(int)const>);
static_assert(is_same_v<add_pointer_t<const void>, const void*>);

static_assert(alignof(aligned_storage_t<3>) == 4);
static_assert(sizeof(aligned_storage_t<3>) == 4);

static_assert(aligned_union<1, char, short, int>::alignment_value == 4);

static_assert(is_same_v<long, common_type_t<int, unsigned short, long>>);

static_assert(is_same_v<const volatile int&, common_reference_t<volatile int&, const int&>>);
static_assert(is_same_v<const int&, common_reference_t<int&, int&&>>);

struct throwing_int_conversion { operator int() noexcept(false); };

static_assert(is_nothrow_convertible_v<int, long>);
static_assert(!is_convertible_v<bool, char*>);
static_assert(is_convertible_v<throwing_int_conversion, int>);
static_assert(!is_nothrow_convertible_v<throwing_int_conversion, int>);

struct invocable_test { int obj; float memfn(int) noexcept(false);};

static_assert(is_invocable_v<decltype(&invocable_test::memfn), invocable_test&, short>);
static_assert(!is_nothrow_invocable_v<decltype(&invocable_test::memfn), invocable_test&, short>);
static_assert(is_nothrow_invocable_v<decltype(&invocable_test::obj), invocable_test&>);
static_assert(is_invocable_r_v<double, decltype(&invocable_test::memfn), invocable_test&, int>);
static_assert(!is_invocable_r_v<char*, decltype(&invocable_test::obj), invocable_test&>);

using PF1 = bool (&)();
using PF2 = short (*)(long);

struct S
{
    operator PF2() const;
    double operator()(char, int&);
    void fn(long) const;
    char data;
};

using PMF = void (S::*)(long) const;
using PMD = char S::*;

static_assert(is_same_v<invoke_result_t<S, int>, short>);
static_assert(is_same_v<invoke_result_t<S&, unsigned char, int&>, double>);
static_assert(is_same_v<invoke_result_t<PF1>, bool>);
//static_assert(is_same_v<invoke_result_t<PMF, unique_ptr<S>, int>, void>);
static_assert(is_same_v<invoke_result_t<PMD, S>, char&&>);
static_assert(is_same_v<invoke_result_t<PMD, const S*>, const char&>);
