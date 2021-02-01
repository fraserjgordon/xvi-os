#include <System/ABI/Dwarf/Decode.hh>


namespace System::ABI::Dwarf
{


#if !__SYSTEM_ABI_DWARF_MINIMAL
std::string toString(dw_ptr_type type)
{
    switch (type)
    {
        case dw_ptr_type::uleb128:
            return "ULEB128";

        case dw_ptr_type::udata2:
            return "udata2";

        case dw_ptr_type::udata4:
            return "udata4";

        case dw_ptr_type::udata8:
            return "udata8";

        case dw_ptr_type::sleb128:
            return "SLEB128";

        case dw_ptr_type::sdata2:
            return "sdata2";

        case dw_ptr_type::sdata4:
            return "sdata4";

        case dw_ptr_type::sdata8:
            return "sdata8";

        default:
            return "unknown";
    }
}

std::string toString(dw_ptr_rel rel)
{
    switch (rel)
    {
        case dw_ptr_rel::abs:
            return "absolute";

        case dw_ptr_rel::pc:
            return "PC-relative";

        case dw_ptr_rel::text:
            return "text-relative";

        case dw_ptr_rel::data:
            return "data-relative";

        case dw_ptr_rel::func:
            return "function-relative";

        default:
            return "unknown";
    }
}

std::string PointerEncodingName(std::uint8_t enc)
{
    if (enc == kDwarfPtrAbs)
        return "absolute pointer";
    else if (enc == kDwarfPtrOmit)
        return "omitted";

    auto type = static_cast<dw_ptr_type>(enc & kDwarfPtrTypeMask);
    auto rel = static_cast<dw_ptr_rel>(enc & kDwarfPtrRelMask);
    bool indirect = (enc & kDwarfPtrIndirect);

    std::string output = {};
    if (indirect)
        output = "indirect ";

    output += toString(rel);
    output += " ";
    output += toString(type);

    return output;
}
#endif


} // namespace System::ABI::Dwarf
