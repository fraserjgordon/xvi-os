#include <System/ABI/ExecContext/Arch/x86/SysV_x64.h>


namespace System::ABI::ExecContext
{


using namespace std;


__SYSTEM_ABI_EXECCONTEXT_VERSIONED_SYMBOL
sysv_x64_frame_t*
CreateContextWithData__v1_0(void* stack, size_t stack_size, create_context_fn_t fn, const void* data, size_t data_size)
{
    // The extra data is copied to the top of the stack, keeping the required 16-byte stack alignment.
    size_t adjusted_data_size = (data_size + 15) & ~size_t(15);
    stack_size -= adjusted_data_size;

    // Copy the data into place.
    void* copy_to = reinterpret_cast<void*>(uintptr_t(stack) + stack_size);
    __builtin_memcpy(copy_to, data, data_size);

    // Perform the normal context creation but using the copied data as the bound parameter.
    return CreateContext(stack, stack_size, fn, uintptr_t(copy_to));
}

__SYSTEM_ABI_EXECCONTEXT_SYMVER_LATEST(CreateContextWithData__v1_0, CreateContextWithData, 1.0);


} // namespace System::ABI::ExecContext
