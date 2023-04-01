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

TEST(MoveOnlyFunction, InPlaceTypeConstructor)
{
    struct callable
    {
        int x;
        int y;

        constexpr int operator()(int z) const noexcept
        {
            return x + y + z;
        }
    };

    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F f(in_place_type<callable>, 15, 20);

        EXPECT(f);

        if constexpr (is_invocable_v<F&, int>)
            EXPECT_EQ(f(7), 42);
        else
            EXPECT_EQ(std::move(f)(7), 42);
    };

    run_tests<int(int)>(test);
}

TEST(MoveOnlyFunction, MoveAssignment)
{
    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F a;
        F b([] noexcept {});

        F c;
        F d([] noexcept {});
        F e;
        F f([] noexcept {});

        c = std::move(a);
        d = std::move(a);
        e = std::move(b);
        f = std::move(b);

        EXPECT(!c);
        EXPECT(!d);
        EXPECT(e);
        EXPECT(f);
    };

    run_tests<void()>(test);
}

TEST(MoveOnlyFunction, NullptrAssignment)
{
    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F a;
        F b([] noexcept {});

        a = nullptr;
        b = nullptr;

        EXPECT(!a);
        EXPECT(!b);
    };

    run_tests<void()>(test);
}

TEST(MoveOnlyFunction, ConvertingAssignment)
{
    struct move_only { move_only() = default; move_only(const move_only&) = delete; move_only(move_only&&) = default; };

    move_only nocopy;

    auto basic_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // A noexcept, non-ref-qualified, const lambda should be acceptable for all signature types.
        F f;
        f = [] noexcept { return 0; };

        EXPECT(f);
    };

    struct memberdata { int x; };
    auto memberdata_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // A member data pointer should be acceptable for all signature types.
        F f1;
        f1 = &memberdata::x;

        EXPECT(f1);

        // A null member data pointer should be acceptable but create an empty function.
        int memberdata::*ptr = nullptr;
        F f2;
        f2 = ptr;

        EXPECT(!f2);
    };

    struct memberfn { int fn() const noexcept { return 0; } };
    auto memberfn_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // A const, noexcept, non-ref-qualified member function pointer should be acceptable for all signature types.
        F f1;
        f1 = &memberfn::fn;

        EXPECT(f1);

        // A null member function pointer should be acceptable but create an empty function.
        int (memberfn::*ptr)() const noexcept = nullptr;
        F f2;
        f2 = ptr;

        EXPECT(!f2);
    };

    auto mof_conversion_test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        // Conversion from a more-restrictive to less-restrictive signature should be allowed.
        F f1;
        f1 = move_only_function<void() const noexcept>([] noexcept {});

        EXPECT(f1);

        // Construction from an empty different function type should leave the function empty.
        F f2;
        f2 = move_only_function<void() const noexcept>{};

        EXPECT(!f2);
    };

    run_tests<void()>(basic_test);
    run_tests<int(memberdata*)>(memberdata_test);
    run_tests<int(memberfn*)>(memberfn_test);
    run_tests<void()>(mof_conversion_test);

    // We should be able to construct with a move-only lambda.
    EXPECT((std::is_assignable_v<move_only_function<void()>&, decltype([x = std::move(nocopy)] {})>));

    // We shouldn't be able to construct a noexcept function with a non-noexcept lambda.
    EXPECT(!(std::is_assignable_v<move_only_function<void() noexcept>&, decltype([] noexcept(false) {})>));

    // We shouldn't be able to construct a const function with a mutable lambda.
    EXPECT(!(std::is_assignable_v<move_only_function<void() const>&, decltype([x = std::move(nocopy)] mutable {})>));

    // We shouldn't be able to construct an rvalue function from a function object with an lvalue ref call operator.
    struct lvalue_call { void operator()() &; };
    EXPECT(!(std::is_assignable_v<move_only_function<void() &&>&, lvalue_call>));
    EXPECT((std::is_assignable_v<move_only_function<void() &>&, lvalue_call>));

    // We shouldn't be able to construct an lvalue function from a function object with an rvalue ref call operator.
    struct rvalue_call { void operator()() &&; };
    EXPECT(!(std::is_assignable_v<move_only_function<void() &>&, rvalue_call>));
    EXPECT((std::is_assignable_v<move_only_function<void() &&>&, rvalue_call>));

    // We shouldn't be able to construct a non-ref-qualified function from a function object with a ref-qualified call operator.
    EXPECT(!(std::is_assignable_v<move_only_function<void()>&, lvalue_call>));
    EXPECT(!(std::is_assignable_v<move_only_function<void()>&, rvalue_call>));
}

TEST(MoveOnlyFunction, Destructor)
{
    EXPECT(std::is_destructible_v<move_only_function<void()>>);
}

TEST(MoveOnlyFunction, Invocation)
{
    // Tests that invocations work.
    auto test_results = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F f([](int x, int y) noexcept { return x + y; });

        if constexpr (std::is_invocable_v<F&, int, int>)
            EXPECT_EQ(f(35, 7), 42);
        else
            EXPECT_EQ(std::move(f)(35, 7), 42);
    };

    run_tests<int(int, int)>(test_results);


    EXPECT((std::is_invocable_v<move_only_function<void(int)>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int)>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int)>, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int)>, int>));

    EXPECT((std::is_invocable_v<move_only_function<void(int) noexcept>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) noexcept>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) noexcept>, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) noexcept>, int>));

    EXPECT((std::is_invocable_v<move_only_function<void(int) const>&, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) const>&&, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const>&&, int>));

    EXPECT((std::is_invocable_v<move_only_function<void(int) const noexcept>&, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const noexcept>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) const noexcept>, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const noexcept>, int>));

    EXPECT((std::is_invocable_v<move_only_function<void(int) &>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) &>&, int>));
    EXPECT(!(std::is_invocable_v<move_only_function<void(int) &>, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) &>, int>));

    EXPECT((std::is_invocable_v<move_only_function<void(int) & noexcept>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) & noexcept>&, int>));
    EXPECT(!(std::is_invocable_v<move_only_function<void(int) & noexcept>, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) & noexcept>, int>));

    EXPECT((std::is_invocable_v<move_only_function<void(int) const &>&, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const &>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) const &>, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const &>, int>));

    EXPECT((std::is_invocable_v<move_only_function<void(int) const & noexcept>&, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const & noexcept>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) const & noexcept>, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const & noexcept>, int>));

    EXPECT(!(std::is_invocable_v<move_only_function<void(int) &&>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) &&>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) &&>, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) &&>, int>));

    EXPECT(!(std::is_invocable_v<move_only_function<void(int) && noexcept>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) && noexcept>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) && noexcept>, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) && noexcept>, int>));

    EXPECT(!(std::is_invocable_v<move_only_function<void(int) const &&>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) const &&>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) const &&>, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const &&>, int>));

    EXPECT(!(std::is_invocable_v<move_only_function<void(int) const && noexcept>&, int>));
    EXPECT(!(std::is_invocable_v<const move_only_function<void(int) const && noexcept>&, int>));
    EXPECT((std::is_invocable_v<move_only_function<void(int) const && noexcept>, int>));
    EXPECT((std::is_invocable_v<const move_only_function<void(int) const && noexcept>, int>));
}

TEST(MoveOnlyFunction, Swap)
{
    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F a;
        F b([] noexcept {});

        a.swap(b);

        EXPECT(a);
        EXPECT(!b);

        swap(a, b);

        EXPECT(!a);
        EXPECT(b);
    };

    run_tests<void()>(test);
}

TEST(MoveOnlyFunction, Comparison)
{
    auto test = [&]<class Signature>(Signature)
    {
        using F = move_only_function<typename Signature::type>;

        F a;
        F b([] noexcept {});

        EXPECT_EQ(a, nullptr);
        EXPECT_NE(b, nullptr);
    };

    run_tests<void()>(test);
}
