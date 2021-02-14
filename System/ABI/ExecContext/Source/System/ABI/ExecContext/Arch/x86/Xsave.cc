#include <System/ABI/ExecContext/Arch/x86/Xsave.hh>

#include <System/ABI/ExecContext/Arch/x86/FloatingPoint.hh>


namespace System::ABI::ExecContext
{


using namespace std;


//! @todo requires __cxa_guard_acquire etc support.
/*xsave_info_t getXsaveInfo()
{
    using namespace System::HW::CPU::CPUID;
    
    // Don't bother trying to probe anything if xsave isn't implemented (but do continue if it is just disabled).
    if (!isXsaveSupported())
        return {};
    
    // This should never fail but check anyway.
    if (!SupportsLeaf(LeafXsaveInfo))
        return {};

    // Get the information that all xsave-supporting CPUs provide.
    xsave_info_t info = {};
    auto results = GetLeaf(LeafXsaveInfo, 0);
    info.supported_user_features = static_cast<xsave_flag_t>((uint64_t(results.edx) << 32) | results.eax);
    info.max_size = results.ecx;
    info.current_user_size = results.ebx;
    
    // Calling the next subfunction will return information about extended xsave capabilities. If the CPU doesn't
    // implement any of these capabilities, this will return zeroes for all fields.
    results = GetLeaf(LeafXsaveInfo, 1);
    info.supported_supervisor_features = static_cast<xsave_flag_t>((uint64_t(results.edx) << 32) | results.ecx);
    info.capabilities.as_u32 = results.eax;
    info.current_supervisor_size = results.ebx;

    // The first two state types (x87 FPU and SSE) are stored in a 512 byte block at offset zero, using the same in-
    // memory format as the fxsave/fxrstor instructions.
    auto x87_index = static_cast<size_t>(XsaveFeature::X87);
    auto sse_index = static_cast<size_t>(XsaveFeature::SSE);
    info.offset[x87_index] = info.offset[sse_index] = 0;
    info.size[x87_index] = info.size[sse_index] = sizeof(fxsave_t);
    info.align_64[x87_index] = info.align_64[sse_index] = false;

    // Call the subleaves for each of the supported state types to query their information.
    for (size_t i = 0; i < XsaveFeatureCount; ++i)
    {
        // Does the CPU support this feature (at either user or supervisor level)?
        if (auto flag = static_cast<xsave_flag_t>(uint64_t(1) << i);
            ((info.supported_user_features | info.supported_supervisor_features) & flag) != XsaveFlag::None)
        {
            // The CPU supports this flag so request its particulars.
            results = GetLeaf(LeafXsaveInfo, i);
            info.size[i] = results.eax;
            info.offset[i] = static_cast<std::ptrdiff_t>(results.ebx);
            info.align_64[i] = (results.ecx & 0x02);
        }
        else
        {
            // This state type isn't supported.
            info.size[i] = 0;
            info.offset[i] = 0;
            info.align_64[i] = false;
        }
    }

    // Return the information that has been gathered.
    return info;
}

bool _isXsaveSupported()
{
    return isXsaveSupported();
}

bool _isXsaveAvailable()
{
    return isXsaveAvailable();
}*/


uint64_t _xgetbv(uint32_t index)
{
    return xgetbv(index);
}

void _xsetbv(uint32_t index, uint64_t value)
{
    xsetbv(index, value);
}


void _xrstor(uint64_t flags, const void* save_area)
{
    xrstor(static_cast<xsave_flag_t>(flags), reinterpret_cast<const fxsave_t*>(save_area));
}

void _xrstors(uint64_t flags, const void* save_area)
{
    xrstors(static_cast<xsave_flag_t>(flags), reinterpret_cast<const fxsave_t*>(save_area));
}

void _xsave(uint64_t flags, void* save_area)
{
    xsave(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}

void _xsavec(uint64_t flags, void* save_area)
{
    xsavec(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}

void _xsaveopt(uint64_t flags, void* save_area)
{
    xsaveopt(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}

void _xsaves(uint64_t flags, void* save_area)
{
    xsaves(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}

#if defined(__x86_64__)
void _xrstor64(uint64_t flags, const void* save_area)
{
    xrstor64(static_cast<xsave_flag_t>(flags), reinterpret_cast<const fxsave_t*>(save_area));
}

void _xrstors64(uint64_t flags, const void* save_area)
{
    xrstors64(static_cast<xsave_flag_t>(flags), reinterpret_cast<const fxsave_t*>(save_area));
}

void _xsave64(uint64_t flags, void* save_area)
{
    xsave64(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}

void _xsavec64(uint64_t flags, void* save_area)
{
    xsavec64(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}

void _xsaveopt64(uint64_t flags, void* save_area)
{
    xsaveopt64(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}

void _xsaves64(uint64_t flags, void* save_area)
{
    xsaves64(static_cast<xsave_flag_t>(flags), reinterpret_cast<fxsave_t*>(save_area));
}
#endif


} // namespace System::ABI::ExecContext
