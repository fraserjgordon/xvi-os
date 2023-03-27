#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/FunctionalUtils.hh>


using namespace __XVI_STD_CORE_NS;


template <class T>
concept is_transparent = requires { typename T::is_transparent; };

TEST(FunctionalUtils, Plus)
{
    auto a = plus<int>{}(2, 3);
    auto b = plus<>{}(short(7), short(8));
    auto c = plus<short>{}(short(7), short(8));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 5);
    EXPECT_EQ(b, 15);
    EXPECT_EQ(c, 15);

    EXPECT(is_transparent<plus<>>);
    EXPECT(!is_transparent<plus<int>>);
}

TEST(FunctionalUtils, Minus)
{
    auto a = minus<int>{}(7, 4);
    auto b = minus<>{}(short(17), short(9));
    auto c = minus<short>{}(short(17), short(9));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, 8);
    EXPECT_EQ(c, 8);

    EXPECT(is_transparent<minus<>>);
    EXPECT(!is_transparent<minus<int>>);
}

TEST(FunctionalUtils, Multiplies)
{
    auto a = multiplies<int>{}(3, 4);
    auto b = multiplies<>{}(short(5), short(2));
    auto c = multiplies<short>{}(short(5), short(2));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 12);
    EXPECT_EQ(b, 10);
    EXPECT_EQ(c, 10);

    EXPECT(is_transparent<multiplies<>>);
    EXPECT(!is_transparent<multiplies<int>>);
}

TEST(FunctionalUtils, Divides)
{
    auto a = divides<int>{}(12, 6);
    auto b = divides<>{}(short(32), short(8));
    auto c = divides<short>{}(short(32), short(8));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 4);
    EXPECT_EQ(c, 4);

    EXPECT(is_transparent<divides<>>);
    EXPECT(!is_transparent<multiplies<int>>);
}

TEST(FunctionalUtils, Modulus)
{
    auto a = modulus<int>{}(12, 5);
    auto b = modulus<>{}(short(13), short(7));
    auto c = modulus<short>{}(short(13), short(7));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 2);
    EXPECT_EQ(b, 6);
    EXPECT_EQ(c, 6);

    EXPECT(is_transparent<modulus<>>);
    EXPECT(!is_transparent<modulus<int>>);
}

TEST(FunctionalUtils, Negate)
{
    auto a = negate<int>{}(-3);
    auto b = negate<>{}(short(10));
    auto c = negate<short>{}(short(10));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, -10);
    EXPECT_EQ(c, -10);

    EXPECT(is_transparent<negate<>>);
    EXPECT(!is_transparent<negate<int>>);
}

TEST(FunctionalUtils, EqualTo)
{
    struct weird { constexpr int operator==(const weird&) const { return 1; } };

    auto a = equal_to<int>{}(3, 6);
    auto b = equal_to<>{}(weird{}, weird{});
    auto c = equal_to<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, false);
    EXPECT_EQ(b, 1);
    EXPECT_EQ(c, true);

    EXPECT(is_transparent<equal_to<>>);
    EXPECT(!is_transparent<equal_to<int>>);
}

TEST(FunctionalUtils, NotEqualTo)
{
    struct weird { constexpr int operator!=(const weird&) const { return 0; } };

    auto a = not_equal_to<int>{}(3, 6);
    auto b = not_equal_to<>{}(weird{}, weird{});
    auto c = not_equal_to<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, true);
    EXPECT_EQ(b, 0);
    EXPECT_EQ(c, false);

    EXPECT(is_transparent<not_equal_to<>>);
    EXPECT(!is_transparent<not_equal_to<int>>);
}

TEST(FunctionalUtils, Greater)
{
    struct weird { constexpr int operator>(const weird&) const { return -3; } };

    auto a = greater<int>{}(7, 2);
    auto b = greater<>{}(weird{}, weird{});
    auto c = greater<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, true);
    EXPECT_EQ(b, -3);
    EXPECT_EQ(c, true);

    EXPECT(is_transparent<greater<>>);
    EXPECT(!is_transparent<greater<int>>);
}

TEST(FunctionalUtils, Less)
{
    struct weird { constexpr int operator<(const weird&) const { return 42; } };

    auto a = less<int>{}(6, 2);
    auto b = less<>{}(weird{}, weird{});
    auto c = less<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, false);
    EXPECT_EQ(b, 42);
    EXPECT_EQ(c, true);

    EXPECT(is_transparent<less<>>);
    EXPECT(!is_transparent<less<int>>);
}

TEST(FunctionalUtils, GreaterEqual)
{
    struct weird { constexpr int operator>=(const weird&) const { return -7; } };

    auto a = greater_equal<int>{}(7, 7);
    auto b = greater_equal<>{}(weird{}, weird{});
    auto c = greater_equal<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, true);
    EXPECT_EQ(b, -7);
    EXPECT_EQ(c, true);

    EXPECT(is_transparent<greater_equal<>>);
    EXPECT(!is_transparent<greater_equal<int>>);
}

TEST(FunctionalUtils, LessEqual)
{
    struct weird { constexpr int operator<=(const weird&) const { return 5; } };

    auto a = less_equal<int>{}(5, 5);
    auto b = less_equal<>{}(weird{}, weird{});
    auto c = less_equal<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, true);
    EXPECT_EQ(b, 5);
    EXPECT_EQ(c, true);

    EXPECT(is_transparent<less_equal<>>);
    EXPECT(!is_transparent<less_equal<int>>);
}

TEST(FunctionalUtils, RangesEqualTo)
{
    auto a = ranges::equal_to{}(42, 13);

    EXPECT_SAME_TYPE(decltype(a), bool);

    EXPECT_EQ(a, false);

    EXPECT(is_transparent<ranges::equal_to>);
}

TEST(FunctionalUtils, RangesNotEqualTo)
{
    auto a = ranges::not_equal_to{}(2, 6);

    EXPECT_SAME_TYPE(decltype(a), bool);

    EXPECT_EQ(a, true);

    EXPECT(is_transparent<ranges::not_equal_to>);
}

TEST(FunctionalUtils, RangesGreater)
{
    auto a = ranges::greater{}(7, 10);

    EXPECT_SAME_TYPE(decltype(a), bool);

    EXPECT_EQ(a, false);

    EXPECT(is_transparent<ranges::greater>);
}

TEST(FunctionalUtils, RangesLess)
{
    auto a = ranges::less{}(7, 10);

    EXPECT_SAME_TYPE(decltype(a), bool);

    EXPECT_EQ(a, true);

    EXPECT(is_transparent<ranges::less>);
}

TEST(FunctionalUtils, RangesGreaterEqual)
{
    auto a = ranges::greater_equal{}(6, 7);

    EXPECT_SAME_TYPE(decltype(a), bool);

    EXPECT_EQ(a, false);

    EXPECT(is_transparent<ranges::greater_equal>);
}

TEST(FunctionalUtils, RangesLessEqual)
{
    auto a = ranges::less_equal{}(3, 19);

    EXPECT_SAME_TYPE(decltype(a), bool);

    EXPECT_EQ(a, true);

    EXPECT(is_transparent<ranges::less_equal>);
}

TEST(FunctionalUtils, LogicalAnd)
{
    struct weird { constexpr int operator&&(const weird&) const { return 7; } };

    auto a = logical_and<bool>{}(true, false);
    auto b = logical_and<>{}(weird{}, weird{});
    auto c = logical_and<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, false);
    EXPECT_EQ(b, 7);
    EXPECT_EQ(c, true);

    EXPECT(is_transparent<logical_and<>>);
    EXPECT(!is_transparent<logical_and<bool>>);
}

TEST(FunctionalUtils, LogicalOr)
{
    struct weird { constexpr int operator||(const weird&) const { return 14; } };

    auto a = logical_or<bool>{}(true, false);
    auto b = logical_or<>{}(weird{}, weird{});
    auto c = logical_or<weird>{}(weird{}, weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), bool);

    EXPECT_EQ(a, true);
    EXPECT_EQ(b, 14);
    EXPECT_EQ(c, true);

    EXPECT(is_transparent<logical_or<>>);
    EXPECT(!is_transparent<logical_or<bool>>);
}

TEST(FunctionalUtils, LogicalNot)
{
    struct weird { constexpr int operator!() const { return 2; } };

    auto a = logical_not<bool>{}(false);
    auto b = logical_not<>{}(weird{});

    EXPECT_SAME_TYPE(decltype(a), bool);
    EXPECT_SAME_TYPE(decltype(b), int);

    EXPECT_EQ(a, true);
    EXPECT_EQ(b, 2);

    EXPECT(is_transparent<logical_not<>>);
    EXPECT(!is_transparent<logical_not<bool>>);
}

TEST(FunctionalUtils, BitAnd)
{
    auto a = bit_and<int>{}(0x7f, 0x83);
    auto b = bit_and<>{}(short(0x55), short(0xaa));
    auto c = bit_and<short>{}(short(0x55), short(0xaa));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 0x03);
    EXPECT_EQ(b, 0);
    EXPECT_EQ(c, 0);

    EXPECT(is_transparent<bit_and<>>);
    EXPECT(!is_transparent<bit_and<int>>);
}

TEST(FunctionalUtils, BitOr)
{
    auto a = bit_or<int>{}(0x42, 0x35);
    auto b = bit_or<>{}(short(0x55), short(0xaa));
    auto c = bit_or<short>{}(short(0x55), short(0xaa));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 0x77);
    EXPECT_EQ(b, 0xff);
    EXPECT_EQ(c, 0xff);

    EXPECT(is_transparent<bit_or<>>);
    EXPECT(!is_transparent<bit_or<int>>);
}

TEST(FunctionalUtils, BitXor)
{
    auto a = bit_xor<int>{}(0x33, 0x22);
    auto b = bit_xor<>{}(short(0xff), short(0xaa));
    auto c = bit_xor<short>{}(short(0xff), short(0xaa));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ(a, 0x11);
    EXPECT_EQ(b, 0x55);
    EXPECT_EQ(c, 0x55);

    EXPECT(is_transparent<bit_xor<>>);
    EXPECT(!is_transparent<bit_xor<int>>);
}

TEST(FunctionalUtils, BitNot)
{
    auto a = bit_not<int>{}(0x3f);
    auto b = bit_not<>{}(short(0xaa));
    auto c = bit_not<short>{}(short(0xaa));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), int);
    EXPECT_SAME_TYPE(decltype(c), short);

    EXPECT_EQ((a & 0xff), 0xc0);
    EXPECT_EQ((b & 0xff), 0x55);
    EXPECT_EQ((c & 0xff), 0x55);

    EXPECT(is_transparent<bit_not<>>);
    EXPECT(!is_transparent<bit_not<int>>);
}

TEST(FunctionalUtils, Identity)
{
    auto a = identity{}(3);
    auto b = identity{}(short(5));

    EXPECT_SAME_TYPE(decltype(a), int);
    EXPECT_SAME_TYPE(decltype(b), short);

    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, 5);

    EXPECT(is_transparent<identity>);
}
