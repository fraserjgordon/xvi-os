#include <Tools/Development/Test/Test.hh>

#include <System/C++/Core/MoveOnlyFunction.hh>


using namespace __XVI_STD_CORE_NS;


template <class Signature> struct sig
{
    using type = Signature;
};

template <class TestFn> struct tester;

template <class R, class... Args>
struct tester<R(Args...)>
{
    template <class TestFn>
    static constexpr void operator()(TestFn& test)
    {
        test(sig<R(Args...) noexcept(false)>{});
        test(sig<R(Args...) const noexcept(false)>{});
        test(sig<R(Args...) & noexcept(false)>{});
        test(sig<R(Args...) const & noexcept(false)>{});
        test(sig<R(Args...) && noexcept(false)>{});
        test(sig<R(Args...) const && noexcept(false)>{});
        test(sig<R(Args...) noexcept(true)>{});
        test(sig<R(Args...) const noexcept(true)>{});
        test(sig<R(Args...) & noexcept(true)>{});
        test(sig<R(Args...) const & noexcept(true)>{});
        test(sig<R(Args...) && noexcept(true)>{});
        test(sig<R(Args...) const && noexcept(true)>{});
    }
};

template <class Signature, class TestFn>
constexpr void run_tests(TestFn& test)
{
    tester<Signature>{}(test);
};


TEST(MoveOnlyFunction, Types)
{
    auto test = [&]<class Signature>(Signature)
    {
        EXPECT_SAME_TYPE(typename move_only_function<typename Signature::type>::result_type, int);
    };

    run_tests<int()>(test);
}

TEST(MoveOnlyFunction, DefaultConstructor)
{
    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F f;

        EXPECT(!f);
    };

    run_tests<int()>(test);
}

TEST(MoveOnlyFunction, NoCopyConstructor)
{
    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        EXPECT(!std::is_copy_constructible_v<F>);
    };

    run_tests<int()>(test);
}

TEST(MoveOnlyFunction, MoveConstructor)
{
    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F f1;
        F f2([] noexcept {});

        F f3(std::move(f1));
        F f4(std::move(f2));

        EXPECT(!f3);
        EXPECT(f4);
    };

    run_tests<void()>(test);
}

TEST(MoveOnlyFunction, ConvertingConstructor)
{
    struct move_only { move_only() = default; move_only(const move_only&) = delete; move_only(move_only&&) = default; };

    move_only nocopy;

    auto basic_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // A noexcept, non-ref-qualified, const lambda should be acceptable for all signature types.
        F f([] noexcept { return 0; });

        EXPECT(f);
    };

    struct memberdata { int x; };
    auto memberdata_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // A member data pointer should be acceptable for all signature types.
        F f1(&memberdata::x);

        EXPECT(f1);

        // A null member data pointer should be acceptable but create an empty function.
        int memberdata::*ptr = nullptr;
        F f2(ptr);

        EXPECT(!f2);
    };

    struct memberfn { int fn() const noexcept { return 0; } };
    auto memberfn_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // A const, noexcept, non-ref-qualified member function pointer should be acceptable for all signature types.
        F f1(&memberfn::fn);

        EXPECT(f1);

        // A null member function pointer should be acceptable but create an empty function.
        int (memberfn::*ptr)() const noexcept = nullptr;
        F f2(ptr);

        EXPECT(!f2);
    };

    auto mof_conversion_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // Conversion from a more-restrictive to less-restrictive signature should be allowed.
        F f1(move_only_function<void() const noexcept>([] noexcept {}));

        EXPECT(f1);

        // Construction from an empty different function type should leave the function empty.
        F f2(move_only_function<void() const noexcept>{});

        EXPECT(!f2);
    };

    run_tests<int()>(basic_test);
    run_tests<int(memberdata*)>(memberdata_test);
    run_tests<int(memberfn*)>(memberfn_test);
    run_tests<void()>(mof_conversion_test);

    // We should be able to construct with a move-only lambda.
    EXPECT((std::is_constructible_v<move_only_function<void()>, decltype([x = std::move(nocopy)] {})>));

    // We shouldn't be able to construct a noexcept function with a non-noexcept lambda.
    EXPECT(!(std::is_constructible_v<move_only_function<void() noexcept>, decltype([] noexcept(false) {})>));

    // We shouldn't be able to construct a const function with a mutable lambda.
    EXPECT(!(std::is_constructible_v<move_only_function<void() const>, decltype([x = std::move(nocopy)] mutable {})>));

    // We shouldn't be able to construct an rvalue function from a function object with an lvalue ref call operator.
    struct lvalue_call { void operator()() &; };
    EXPECT(!(std::is_constructible_v<move_only_function<void() &&>, lvalue_call>));
    EXPECT((std::is_constructible_v<move_only_function<void() &>, lvalue_call>));

    // We shouldn't be able to construct an lvalue function from a function object with an rvalue ref call operator.
    struct rvalue_call { void operator()() &&; };
    EXPECT(!(std::is_constructible_v<move_only_function<void() &>, rvalue_call>));
    EXPECT((std::is_constructible_v<move_only_function<void() &&>, rvalue_call>));

    // We shouldn't be able to construct a non-ref-qualified function from a function object with a ref-qualified call operator.
    EXPECT(!(std::is_constructible_v<move_only_function<void()>, lvalue_call>));
    EXPECT(!(std::is_constructible_v<move_only_function<void()>, rvalue_call>));
}
