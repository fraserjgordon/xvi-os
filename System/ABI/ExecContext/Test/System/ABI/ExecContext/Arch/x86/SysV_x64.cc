#include <System/ABI/ExecContext/Arch/x86/SysV_x64.h>


#include <exception>
#include <memory>

#include <gtest/gtest.h>


namespace System::ABI::ExecContext
{


TEST(CaptureContextTest, CountReturns)
{
    // Counter variable.
    volatile unsigned int return_count = 0;

    // Number of test iterations.
    constexpr unsigned int ResumeCount = 10;

    {
        // Capture the context of this method.
        sysv_x64_frame_t frame;
        if (auto count = CaptureContext(&frame, 0).second; count < ResumeCount)
        {
            // Increment the variable and return again.
            return_count += 1;
            ResumeContext(&frame, count + 1);

            // Shouldn't get here!
            FAIL() << "ResumeContext() should not have returned";
        }

        // Increment the return count again.
        return_count += 1;
    }

    // Return the number of times we saw CaptureContext return.
    ASSERT_EQ(return_count, ResumeCount + 1);
}


class CreateContextTest :
    public ::testing::Test
{
protected:

    void SetUp() override
    {
        // Allocate memory for the stack.
        m_stack = std::make_unique<std::uint8_t[]>(StackSize);
    }

    // A secondary stack on which the created context lives.
    static constexpr std::size_t StackSize = 4096;
    std::unique_ptr<std::uint8_t[]> m_stack;
};

using CreateContextDeathTest = CreateContextTest;


TEST_F(CreateContextDeathTest, IllegalReturn)
{
    // Lambda that is executed in the new context. It should cause the test to die because it returns.
    auto fn = [](sysv_x64_frame_t*, std::uintptr_t, std::uintptr_t)
    {
        // Does nothing.
    };

    // Create a return context on the original stack. It will only (successfully) return once.
    sysv_x64_frame_t return_context;
    auto [prev_context, return_param] = CaptureContext(&return_context, 0);
    if (return_param == 0)
    {
        // Create a new context on the allocated stack which will execute the lambda.
        auto* context = CreateContext(m_stack.get(), StackSize, fn, 0);

        // Execute the newly-created context. This should cause an unrecoverable error.
        ASSERT_DEATH(ResumeContext(context, 0), ".*");
    }
}

TEST_F(CreateContextDeathTest, IllegalReturnViaException)
{
    // Lambda that is executed in the new context. Like any other return, return-via-exception is not allowed.
    auto fn = [](sysv_x64_frame_t*, std::uintptr_t, std::uintptr_t)
    {
        // Throw an arbitrary exception.
        throw 0;
    };

    // Create a return context on the original stack. It will only (successfully) return once.
    sysv_x64_frame_t return_context;
    auto [prev_context, return_param] = CaptureContext(&return_context, 0);
    if (return_param == 0)
    {
        // Create a new context on the allocated stack which will execute the lambda.
        auto* context = CreateContext(m_stack.get(), StackSize, fn, 0);

        // Execute the newly-created context. This should throw.
        ASSERT_DEATH(ResumeContext(context, 0), ".*");
    }
}

TEST_F(CreateContextTest, OneShotWithCallParam)
{
    // Lambda that is executed in the new context. It simply returns.
    auto fn = [](sysv_x64_frame_t* return_context, std::uintptr_t call_param, std::uintptr_t)
    {
        // RUNNING ON ANOTHER STACK - MUST NOT USE ASSERT_xxx()!
        
        // Called via ResumeContext, should not have a return context pointer.
        EXPECT_EQ(return_context, nullptr);

        // Get the context - it was passed as the call parameter.
        auto context = reinterpret_cast<sysv_x64_frame_t*>(call_param);
        
        // Return to the original context. This should not return.
        ResumeContext(context, 1);
        ADD_FAILURE() << "ResumeContext() returned (new context)";
    };

    // Create a return context on the original stack. This should return exactly twice.
    volatile int return_count = 0;
    sysv_x64_frame_t return_context;
    auto [prev_context, return_param] = CaptureContext(&return_context, 0);
    if (return_param == 0)
    {
        // This should be the first return. Validate that it is and that we got the right context pointer back.
        ASSERT_EQ(return_count, 0);
        ASSERT_EQ(prev_context, &return_context);

        // We have returned once so far.
        return_count += 1;

        // Create a new context on the allocated stack which will execute the lambda.
        auto context = CreateContext(m_stack.get(), StackSize, fn, 0);

        // Execute the newly-created context. This should not return.
        ResumeContext(context, std::uintptr_t(&return_context));
        FAIL() << "ResumeContext() returned (original context)";
    }
    else if (return_param == 1)
    {
        // This should be the second return, having executed the new context.
        ASSERT_EQ(return_count, 1);
        ASSERT_EQ(prev_context, nullptr);

        // We have returned twice.
        return_count += 1;
    }
    else
    {
        FAIL() << "CaptureContext() returned an invalid parameter: " << return_param;
    }

    // Should already be covered by other checks above but no harm in checking again.
    ASSERT_EQ(return_count, 2);
}

TEST_F(CreateContextTest, OneShotSwapWithBoundParams)
{
    // Lambda that represents a "normal" function taking parameters.
    auto function_with_params = [](int* variable, int value)
    {
        // All this lambda does is set the given variable to the given value.
        *variable = value;
    };

    // Indirection lambda: takes the parameters to bind and returns another (parameterless but stateful) lambda that'll
    // do some real work. Basically, a poor man's bind.
    auto make_bound_fn = [function_with_params](int* variable, int value)
    {
        return [function_with_params, variable, value]() { function_with_params(variable, value); };
    };

    // The type of lambda returned from the indirection lambda.
    using lambda_type = decltype(make_bound_fn(nullptr, 0));
    
    // Lambda that serves to execute a second lambda (passed as a void*).
    auto trampoline_lambda = [](sysv_x64_frame_t* return_context, std::uintptr_t, std::uintptr_t bound_param)
    {
        // RUNNING ON ANOTHER STACK - MUST NOT USE ASSERT_xxx()!

        // Executed via SwapContexts - return context provided.
        EXPECT_NE(return_context, nullptr);

        // Cast the given pointer to a pointer-to-indirection-lambda and call through it.
        auto real_work = reinterpret_cast<lambda_type*>(bound_param);
        EXPECT_NE(real_work, nullptr);
        (*real_work)();

        // Return to the caller.
        ResumeContext(return_context, -1);
    };

    // The variable we're going to manipulate.
    int test_var = 0;

    // Try changing the variable's value using a created context.
    auto bound_work = make_bound_fn(&test_var, 42);
    auto created_context = CreateContext(m_stack.get(), StackSize, trampoline_lambda, std::uintptr_t(&bound_work));
    auto [prev_context, return_value] = SwapContexts(created_context, 0);

    // There should be no return context and the return value should match.
    ASSERT_EQ(prev_context, nullptr);
    ASSERT_EQ(return_value, std::uintptr_t(-1));

    // Check that the variable was manipulated as requested.
    ASSERT_EQ(test_var, 42);
}

TEST_F(CreateContextTest, BasicCoroutine)
{
    // Number of times the coroutine will yield.
    static constexpr int MaxYieldCount = 5;
    
    // This lambda implements a basic coroutine, returning multiple times.
    auto coroutine_fn = [](sysv_x64_frame_t* yield_to, std::uintptr_t, std::uintptr_t)
    {
        // RUNNING ON ANOTHER STACK - MUST NOT USE ASSERT_xxx()!

        EXPECT_NE(yield_to, nullptr);

        // Count of the number of times we've yielded from this coroutine.
        int yield_count = 0;

        // Yield if we're not done yet.
        while (++yield_count < MaxYieldCount)
            SwapContexts(yield_to, yield_count);

        // Otherwise, we're done.
        ResumeContext(yield_to, yield_count);
    };

    // Create the initial context.
    auto coroutine = CreateContext(m_stack.get(), StackSize, coroutine_fn, 0);

    // Loop until we've yielded the expected number of times.
    int actual_yield_count = 0;
    while (actual_yield_count < MaxYieldCount)
    {
        // Execute the coroutine and validate the return value.
        int return_value = -1;
        std::tie(coroutine, return_value) = SwapContexts(coroutine, 0);
        actual_yield_count += 1;
        EXPECT_EQ(return_value, actual_yield_count);

        if (actual_yield_count < MaxYieldCount)
            EXPECT_NE(coroutine, nullptr);
    }

    // The last iteration of the coroutine shouldn't have returned a context.
    ASSERT_EQ(coroutine, nullptr);
}

TEST_F(CreateContextTest, ThrowAndCatchInNewContext)
{
    // Lambdas that catches a thrown exception.
    auto fn = [](sysv_x64_frame_t* return_context, std::uintptr_t, std::uintptr_t)
    {
        // Test exception handling on the new stack.
        try
        {
            throw std::runtime_error("test exception");
        }
        catch (...)
        {
            // Exception caught. Rethrow and make sure we get the right type.
            auto e = std::current_exception();
            try
            {
                // Re-throw the exception.
                std::rethrow_exception(e);
            }
            catch (std::runtime_error& e)
            {
                // Successfully caught the re-thrown exception.
                ResumeContext(return_context, 1);
            }

            // Shouldn't get here..
            ADD_FAILURE() << "Failed to catch exception as std::runtime_error";
            ResumeContext(return_context, 0);
        }

        // Shouldn't get here...
        ADD_FAILURE() << "Failed to catch or catch returned";
        ResumeContext(return_context, 0);
    };

    // Create the test context.
    sysv_x64_frame_t* context = CreateContext(m_stack.get(), StackSize, fn, 0);

    // Execute the test function and validate the return values.
    auto [prev_context, return_value] = SwapContexts(context, 0);
    ASSERT_EQ(prev_context, nullptr);
    ASSERT_EQ(return_value, 1U);
}


} // namespace System::ABI::ExecContext
