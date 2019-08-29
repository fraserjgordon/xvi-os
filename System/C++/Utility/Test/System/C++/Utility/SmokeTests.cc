#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Any.hh>
#include <System/C++/Utility/Array.hh>
#include <System/C++/Utility/Bind.hh>
#include <System/C++/Utility/Bitset.hh>
#include <System/C++/Utility/CharConv.hh>
#include <System/C++/Utility/Chrono.hh>
#include <System/C++/Utility/Function.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Memory.hh>
#include <System/C++/Utility/MemoryResource.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/Ranges.hh>
#include <System/C++/Utility/ScopedAllocator.hh>
#include <System/C++/Utility/SharedPtr.hh>
#include <System/C++/Utility/Span.hh>
#include <System/C++/Utility/String.hh>
#include <System/C++/Utility/StringView.hh>
#include <System/C++/Utility/SystemError.hh>
#include <System/C++/Utility/Tuple.hh>
#include <System/C++/Utility/Variant.hh>

using namespace __XVI_STD_NS;
using namespace __XVI_STD_NS::experimental;


extern void external_thing();

struct non_trivial
{ 
    constexpr non_trivial() {}
    constexpr non_trivial(const non_trivial&) {}
    constexpr non_trivial(non_trivial&&) = default;
    non_trivial& operator=(const non_trivial&) = delete;
    non_trivial& operator=(non_trivial&&) = default;
};


int foo(pair<int, int> p)
{
    return p.first;
}

auto bar(int a, int b)
{
    return pair(a, b);
}

int baz(int a, float b)
{
    return get<int>(pair(a, b));
}

auto quux(int a, int b)
{
    static_assert(is_nothrow_move_assignable_v<int>);
    static_assert(is_trivially_move_assignable_v<__detail::__tuple_storage<0, int>>);
    static_assert(is_trivial_v<tuple<int, int>>);
    return tuple<int, long>(tuple<int, int>(a, b));
}

auto frob(int a)
{
    auto o = optional(a);
    static_assert(is_same_v<optional<int>, decltype(o)>);
    static_assert(is_trivially_copy_constructible_v<decltype(o)>);
    static_assert(is_trivially_move_constructible_v<decltype(o)>);
    static_assert(is_trivially_copy_assignable_v<decltype(o)>);
    static_assert(is_trivially_move_assignable_v<decltype(o)>);
    static_assert(is_trivially_destructible_v<decltype(o)>);
    static_assert(sizeof(o) == 2*sizeof(int));
    return o;
}

auto defrob(variant<int, float, non_trivial> v)
{
    return v;
}

optional<non_trivial> refrob(const optional<non_trivial>& o)
{
    return o;
}

constexpr auto defrob_const(variant<int, float, non_trivial> v)
{
    using V = variant<int, float, non_trivial>;
    static_assert(is_nothrow_default_constructible_v<int>);
    static_assert(!is_copy_assignable_v<V>);
    static_assert(sizeof(v) == 2*sizeof(void*));
    static_assert(alignof(v) == alignof(void*));
    V u = 42;
    v.swap(u);
    return v;
}

auto defrob_more(variant<int, float, non_trivial> v)
{
    return defrob_const(v);
}

auto unfrob()
{
    return variant<int, long>(short(42));
}

static_assert(is_same_v<__detail::__make_repeated_integer_sequence<int, 3, 1, 4>,
                        integer_sequence<int, 0, 0, 0, 1, 2, 2, 2, 2>>);

static_assert(is_same_v<tuple<int, float, bool>,
                       decltype(tuple_cat(tuple(), tuple(0, 0.0f), tuple(), tuple(false), tuple()))>);

static_assert(is_same_v<int&&, decltype(get<0>(tuple(0)))>);

static_assert(0 == get<int>(tuple(false, 0)));

static_assert(tuple(0) == tuple(0.0f));
static_assert(tuple(0) < tuple(42));

static_assert(!optional<int>());
static_assert(optional<int>(0));

static_assert(*optional<int>(42) == 42);

static_assert(!is_trivially_copy_assignable_v<non_trivial>);
static_assert(!is_trivially_copy_assignable_v<optional<non_trivial>>);

static_assert(sizeof(optional<bool>) == 2*sizeof(bool));

constexpr auto result = visit
(
    [](auto x) -> int { return x; },
    variant<int, float>(42)
);

//constexpr auto result2 = visit
//(
//    [](auto x, auto y) -> int { return x + y; },
//    variant<int, float>(20),
//    variant<int, float>(22)
//);

static_assert(result == 42);

any any_test(int a)
{
    return a;
}

function<int(int,int)> test_fn = [](int a, int b) { return a + b; };
int invoke_test_fn()
{
    return test_fn(30, 12);
}

static_assert(is_void_v<decltype(delete declval<int*>())>);

int error_a = 0;
int exception_test(int a) __XVI_CXX_UTILITY_FN_TRY
{
    return 42 / a;
}
__XVI_CXX_UTILITY_FN_CATCH(...,
{
    error_a = a;
    throw;
})


auto shared_ptr_test(int a)
{
    return make_shared<int>(a);
}

auto shared_ptr_test2(int a)
{
    auto p = shared_ptr_test(a);
    return make_pair(p, p);
}

weak_ptr<int> weak_ptr_test(int a)
{
    return shared_ptr_test(a);
}

shared_ptr<int> weak_ptr_test2(int a)
{
    return weak_ptr_test(a).lock();
}

auto bind_front_test(int a, int b = 0)
{
    return bind_front([](auto x, auto y, auto z) { return x + y + z; }, a, b);
}

int bind_front_test2()
{
    return bind_front_test(40)(2);
}

auto bind_test(int a, int b = 0)
{
    return bind([](auto x, auto y, auto z) { return x + y + z; }, a, placeholders::_1, b);
}

int bind_test2()
{
    return bind_test(40)(2);
}

using namespace __XVI_STD_NS::ranges;

char test_array[42];

//static_assert(WeaklyIncrementable<char*>);
//static_assert(_Cpp17ForwardIterator<char*>);

static_assert(ranges::begin(test_array) == test_array);
static_assert(ranges::end(test_array) == test_array + 42);
static_assert(ranges::cbegin(test_array) == test_array);
static_assert(ranges::cend(test_array) == test_array + 42);
static_assert(ranges::size(test_array) == 42);
//static_assert(ranges::data(test_array) == test_array);
//static_assert(ranges::cdata(test_array) == test_array);

auto make_test_array(int len)
{
    return make_shared_default_init<int[]>(len);
}

constexpr const char letters[] = "abcdabcdefg";
constexpr const char needle[] = "abcd";
static_assert(mismatch(__XVI_STD_NS::begin(letters), __XVI_STD_NS::end(letters),
                       __XVI_STD_NS::begin(needle), __XVI_STD_NS::end(needle))
                .second == __XVI_STD_NS::end(needle) - 1);
static_assert(find_end(__XVI_STD_NS::begin(letters), __XVI_STD_NS::end(letters),
                       __XVI_STD_NS::begin(needle), __XVI_STD_NS::end(needle) - 1)
                == &letters[4]);

static_assert(lower_bound(&needle[0], &needle[4], 'a') == &needle[0]);
static_assert(lower_bound(&needle[0], &needle[4], 'b') == &needle[1]);
static_assert(lower_bound(&needle[0], &needle[4], 'c') == &needle[2]);
static_assert(lower_bound(&needle[0], &needle[4], 'd') == &needle[3]);
static_assert(lower_bound(&needle[0], &needle[4], 'e') == &needle[4]);

static_assert(upper_bound(&needle[0], &needle[4], 'a') == &needle[1]);
static_assert(upper_bound(&needle[0], &needle[4], 'b') == &needle[2]);
static_assert(upper_bound(&needle[0], &needle[4], 'c') == &needle[3]);
static_assert(upper_bound(&needle[0], &needle[4], 'd') == &needle[4]);
static_assert(upper_bound(&needle[0], &needle[4], 'e') == &needle[4]);


constexpr bool rotate_test()
{
    char array[] = {'1', '2', '3', '4', '5', '6', '7'};
    char target[] = { '3', '4', '5', '6', '7', '1', '2' };
    auto p = rotate(&array[0], &array[2], &array[7]);
    return equal(&array[0], &array[7], &target[0]) && *p == '1';
}

static_assert(rotate_test());

constexpr bool shift_right_test()
{
    char array[] = {'1', '2', '3', '4', '5', '6', '7'};
    char target[] = {'x', 'x', '1', '2', '3', '4', '5'};
    shift_right(&array[0], &array[7], 2);
    return equal(&array[2], &array[7], &target[2]);
}

static_assert(shift_right_test());

constexpr bool set_union_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;
    
    char one[] = "aaabbbbbe";
    char two[] = "aaabbbcddf";
    char expect[] = "aaabbbbbcddef";

    char out[32] = {};

    auto result = set_union(begin(one), end(one)-1, begin(two), end(two)-1, begin(out));

    return equal(begin(out), result, begin(expect), end(expect)-1);
}

static_assert(set_union_test());

constexpr bool set_intersection_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;
    
    char one[] = "aaabbbbbe";
    char two[] = "aaabbbcddf";
    char expect[] = "aaabbb";

    char out[32] = {};

    auto result = set_intersection(begin(one), end(one)-1, begin(two), end(two)-1, begin(out));

    return equal(begin(out), result, begin(expect), end(expect)-1);
}

static_assert(set_intersection_test());

constexpr bool set_difference_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;
    
    char one[] = "aaabbbbbe";
    char two[] = "aaabbbcddf";
    char expect[] = "bbe";

    char out[32] = {};

    auto result = set_difference(begin(one), end(one)-1, begin(two), end(two)-1, begin(out));

    return equal(begin(out), result, begin(expect), end(expect)-1);
}

static_assert(set_difference_test());

constexpr bool set_symmetric_difference_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;
    
    char one[] = "aaabbbbbe";
    char two[] = "aaabbbcddf";
    char expect[] = "bbcddef";

    char out[32] = {};

    auto result = set_symmetric_difference(begin(one), end(one)-1, begin(two), end(two)-1, begin(out));

    return equal(begin(out), result, begin(expect), end(expect)-1);
}

static_assert(set_symmetric_difference_test());

constexpr bool heap_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;

    char in[] = "afadadbcbbbbbababea";
    char expect[] = "aaaaaabbbbbbbbcddef";

    make_heap(begin(in), end(in)-1);
    bool heap = is_heap(begin(in), end(in)-1);
    sort_heap(begin(in), end(in)-1);

    return heap && equal(begin(in), end(in)-1, begin(expect), end(expect)-1);
}

static_assert(heap_test());

//static_assert(bit_cast<uint32_t>(0.0f) == 0);

constexpr bool partition_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;

    char in[] = "afadadbcbbbbbababea";

    auto pred = [](const char& __x)
    {
        return __x < 'c';
    };

    partition(begin(in), end(in)-1, pred);

    return is_partitioned(begin(in), end(in)-1, pred);
}

static_assert(partition_test());

constexpr bool sort_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;

    char in[] = "afadadbcbbbbbababea";
    char expect[] = "aaaaaabbbbbbbbcddef";

    sort(begin(in), end(in)-1);

    return is_sorted(begin(in), end(in)-1)
           && equal(begin(in), end(in)-1, begin(expect), end(expect)-1);
}

static_assert(sort_test());

constexpr bool nth_element_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;

    char in[] = "afadadbcbbbbbababea";

    nth_element(begin(in), begin(in)+14, end(in)-1);

    return in[14] == 'c';
}

static_assert(nth_element_test());

constexpr bool partial_sort_test()
{
    using __XVI_STD_NS::begin;
    using __XVI_STD_NS::end;

    char in[] = "afadadbcbbbbbababeafghijklmnopqrstuvwxy";

    partial_sort(begin(in), begin(in)+14, end(in)-1);

    return is_sorted_until(begin(in), end(in)-1) >= begin(in)+14;
}

static_assert(partial_sort_test());


using namespace chrono;

static_assert(year_month_day{sys_days{days{0}}} == year_month_day{1970y, January, 1d});
static_assert(year_month_day{sys_days{days{18'024}}} == year_month_day{2019y, May, 8d});

static_assert(weekday{1d/January/1970} == Thursday);
static_assert(weekday{January/1/1971} == Friday);
static_assert(weekday{2019y/May/8} == Wednesday);

static_assert("Hello"sv.length() == 5);
static_assert("Hello, World!"sv.find("World") == 7);

static_assert(sizeof(string) == 4*sizeof(void*));

string string_test()
{
    return string();
}
