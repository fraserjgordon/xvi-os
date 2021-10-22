#include <System/ABI/C++/Unwind/Unwinder.hh>

#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::CXX
{


struct index_entry
{
    // Special data value to indicate that a frame cannot be unwound.
    static constexpr std::uintptr_t CannotUnwind    = 0x1;

    std::uintptr_t  offset;
    std::uintptr_t  data;
};


class EHABIUnwinder final :
    public Unwinder
{

};


} // namespace System::ABI::CXX


_Unwind_VRS_Result _Unwind_VRS_Get(_Unwind_Context* context, _Unwind_VRS_RegClass regclass, std::uint32_t reg, _Unwind_VRS_DataRepresentation datarep, void* value)
{
    switch (regclass)
    {
        case _UVRSC_CORE:
            // Check that the register number and type is valid.
            if (reg > 15 || datarep != _UVRSD_UINT32)
                return _UVRSR_FAILED;

            *reinterpret_cast<std::uint32_t*>(value) = System::ABI::CXX::Unwinder::Context::from(context)->getGR(reg);
            return _UVRSR_OK;

        case _UVRSC_VFP:
        case _UVRSC_WMMXC:
        case _UVRSC_WMMXD:
            // Not yet supported.
            //! @todo: implement
            return _UVRSR_NOT_IMPLEMENTED;

        default:
            // Unknown register class.
            return _UVRSR_NOT_IMPLEMENTED;
    }
}

_Unwind_VRS_Result _Unwind_VRS_Set(_Unwind_Context* context, _Unwind_VRS_RegClass regclass, std::uint32_t reg, _Unwind_VRS_DataRepresentation datarep, void* value)
{
    switch (regclass)
    {
        case _UVRSC_CORE:
            // Check that the register number and type is valid.
            if (reg > 15 || datarep != _UVRSD_UINT32)
                return _UVRSR_FAILED;

            System::ABI::CXX::Unwinder::Context::from(context)->setGR(reg, *reinterpret_cast<std::uint32_t*>(value));
            return _UVRSR_OK;

        case _UVRSC_VFP:
        case _UVRSC_WMMXC:
        case _UVRSC_WMMXD:
            // Not yet supported.
            //! @todo: implement.
            return _UVRSR_NOT_IMPLEMENTED;

        default:
            // Unknown register class.
            return _UVRSR_NOT_IMPLEMENTED;
    }
}
