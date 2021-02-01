#include <System/ABI/Dwarf/Expression.hh>

#include <System/ABI/Dwarf/Arch.hh>
#include <System/ABI/Dwarf/Decode.hh>
#include <System/ABI/Dwarf/LEB128.hh>

#if !__SYSTEM_ABI_DWARF_MINIMAL
#  include <System/C/LanguageSupport/IntTypes.h>
#endif


using namespace std;


namespace System::ABI::Dwarf
{


bool DwarfExpressionDecode(const byte* ptr, size_t len, bool (*callback)(void*, const dwarf_expr_op&), void* callback_context)
{
    // Process the instruction stream.
    while (len > 0)
    {
        // Keep a pointer to the beginning of the instruction.
        auto original_ptr = ptr;

        // What is the instruction?
        dwarf_expr_op op = {};
        switch (auto opcode = dwarf_op(*ptr++); opcode)
        {
            case dwarf_op::addr:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uintptr_t>(ptr);
                ptr += sizeof(uintptr_t);
                break;

            case dwarf_op::deref:
                op.opcode = opcode;
                break;

            case dwarf_op::const1u:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint8_t>(ptr);
                ptr += sizeof(uint8_t);
                break;

            case dwarf_op::const1s:
                op.opcode = opcode;
                op.op1 = UnalignedRead<int8_t>(ptr);
                ptr += sizeof(int8_t);
                break;

            case dwarf_op::const2u:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint16_t>(ptr);
                ptr += sizeof(uint16_t);
                break;

            case dwarf_op::const2s:
                op.opcode = opcode;
                op.op1 = UnalignedRead<int16_t>(ptr);
                ptr += sizeof(int16_t);
                break;

            case dwarf_op::const4u:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint32_t>(ptr);
                ptr += sizeof(uint32_t);
                break;

            case dwarf_op::const4s:
                op.opcode = opcode;
                op.op1 = UnalignedRead<int32_t>(ptr);
                ptr += sizeof(int32_t);
                break;

            case dwarf_op::const8u:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint64_t>(ptr);
                ptr += sizeof(uint64_t);
                break;

            case dwarf_op::const8s:
                op.opcode = opcode;
                op.op1 = UnalignedRead<int64_t>(ptr);
                ptr += sizeof(int64_t);
                break;

            case dwarf_op::constu:
                op.opcode = opcode;
                op.op1 = DecodeULEB128(ptr);
                break;

            case dwarf_op::consts:
                op.opcode = opcode;
                op.op1 = DecodeSLEB128(ptr);
                break;

            case dwarf_op::dup:
            case dwarf_op::drop:
            case dwarf_op::over:
            case dwarf_op::swap:
            case dwarf_op::rot:
            case dwarf_op::xderef:
            case dwarf_op::abs:
            case dwarf_op::_and:
            case dwarf_op::div:
            case dwarf_op::minus:
            case dwarf_op::mod:
            case dwarf_op::mul:
            case dwarf_op::neg:
            case dwarf_op::_not:
            case dwarf_op::_or:
            case dwarf_op::plus:
            case dwarf_op::shl:
            case dwarf_op::shr:
            case dwarf_op::shra:
            case dwarf_op::_xor:
            case dwarf_op::eq:
            case dwarf_op::ge:
            case dwarf_op::gt:
            case dwarf_op::le:
            case dwarf_op::lt:
            case dwarf_op::ne:
            case dwarf_op::nop:
            case dwarf_op::push_object_address:
            case dwarf_op::form_tls_address:
            case dwarf_op::call_frame_cfa:
            case dwarf_op::stack_value:
                op.opcode = opcode;
                break;

            case dwarf_op::pick:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint8_t>(ptr);
                ptr += sizeof(uint8_t);
                break;

            case dwarf_op::plus_uconst:
                op.opcode = opcode;
                op.op1 = DecodeULEB128(ptr);
                break;

            case dwarf_op::skip:
            case dwarf_op::bra:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint16_t>(ptr);
                ptr += sizeof(uint16_t);
                break;

            case dwarf_op::lit0:
            case dwarf_op::lit1:
            case dwarf_op::lit2:
            case dwarf_op::lit3:
            case dwarf_op::lit4:
            case dwarf_op::lit5:
            case dwarf_op::lit6:
            case dwarf_op::lit7:
            case dwarf_op::lit8:
            case dwarf_op::lit9:
            case dwarf_op::lit10:
            case dwarf_op::lit11:
            case dwarf_op::lit12:
            case dwarf_op::lit13:
            case dwarf_op::lit14:
            case dwarf_op::lit15:
            case dwarf_op::lit16:
            case dwarf_op::lit17:
            case dwarf_op::lit18:
            case dwarf_op::lit19:
            case dwarf_op::lit20:
            case dwarf_op::lit21:
            case dwarf_op::lit22:
            case dwarf_op::lit23:
            case dwarf_op::lit24:
            case dwarf_op::lit25:
            case dwarf_op::lit26:
            case dwarf_op::lit27:
            case dwarf_op::lit28:
            case dwarf_op::lit29:
            case dwarf_op::lit30:
            case dwarf_op::lit31:
                // These are all reduced to a lit0 opcode with a literal equal to the operand.
                op.opcode = dwarf_op::lit0;
                op.op1 = uint8_t(opcode) - uint8_t(dwarf_op::lit0);
                break;

            case dwarf_op::reg0:
            case dwarf_op::reg1:
            case dwarf_op::reg2:
            case dwarf_op::reg3:
            case dwarf_op::reg4:
            case dwarf_op::reg5:
            case dwarf_op::reg6:
            case dwarf_op::reg7:
            case dwarf_op::reg8:
            case dwarf_op::reg9:
            case dwarf_op::reg10:
            case dwarf_op::reg11:
            case dwarf_op::reg12:
            case dwarf_op::reg13:
            case dwarf_op::reg14:
            case dwarf_op::reg15:
            case dwarf_op::reg16:
            case dwarf_op::reg17:
            case dwarf_op::reg18:
            case dwarf_op::reg19:
            case dwarf_op::reg20:
            case dwarf_op::reg21:
            case dwarf_op::reg22:
            case dwarf_op::reg23:
            case dwarf_op::reg24:
            case dwarf_op::reg25:
            case dwarf_op::reg26:
            case dwarf_op::reg27:
            case dwarf_op::reg28:
            case dwarf_op::reg29:
            case dwarf_op::reg30:
            case dwarf_op::reg31:
                // These are all reduced to a regx opcode with a register number equal stored as the operand.
                op.opcode = dwarf_op::regx;
                op.op1 = uint8_t(opcode) - uint8_t(dwarf_op::reg0);
                break;

            case dwarf_op::breg0:
            case dwarf_op::breg1:
            case dwarf_op::breg2:
            case dwarf_op::breg3:
            case dwarf_op::breg4:
            case dwarf_op::breg5:
            case dwarf_op::breg6:
            case dwarf_op::breg7:
            case dwarf_op::breg8:
            case dwarf_op::breg9:
            case dwarf_op::breg10:
            case dwarf_op::breg11:
            case dwarf_op::breg12:
            case dwarf_op::breg13:
            case dwarf_op::breg14:
            case dwarf_op::breg15:
            case dwarf_op::breg16:
            case dwarf_op::breg17:
            case dwarf_op::breg18:
            case dwarf_op::breg19:
            case dwarf_op::breg20:
            case dwarf_op::breg21:
            case dwarf_op::breg22:
            case dwarf_op::breg23:
            case dwarf_op::breg24:
            case dwarf_op::breg25:
            case dwarf_op::breg26:
            case dwarf_op::breg27:
            case dwarf_op::breg28:
            case dwarf_op::breg29:
            case dwarf_op::breg30:
            case dwarf_op::breg31:
                // These are all reduced to a bregx opcode with a register number equal stored as the operand.
                op.opcode = dwarf_op::bregx;
                op.op1 = uint8_t(opcode) - uint8_t(dwarf_op::breg0);
                op.op2 = DecodeSLEB128(ptr);
                break;

            case dwarf_op::regx:
                op.opcode = opcode;
                op.op1 = DecodeULEB128(ptr);
                break;

            case dwarf_op::fbreg:
                op.opcode = opcode;
                op.op1 = DecodeSLEB128(ptr);
                break;

            case dwarf_op::bregx:
                op.opcode = opcode;
                op.op1 = DecodeULEB128(ptr);
                op.op2 = DecodeSLEB128(ptr);
                break;

            case dwarf_op::piece:
                op.opcode = opcode;
                op.op1 = DecodeULEB128(ptr);
                break;

            case dwarf_op::deref_size:
            case dwarf_op::xderef_size:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint8_t>(ptr);
                ptr += sizeof(uint8_t);
                break;

            case dwarf_op::call2:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint16_t>(ptr);
                ptr += sizeof(uint16_t);
                break;

            case dwarf_op::call4:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uint32_t>(ptr);
                ptr += sizeof(uint32_t);
                break;

            case dwarf_op::call_ref:
                op.opcode = opcode;
                op.op1 = UnalignedRead<uintptr_t>(ptr);
                ptr += sizeof(uintptr_t);
                break;

            case dwarf_op::bit_piece:
                op.opcode = opcode;
                op.op1 = DecodeULEB128(ptr);
                op.op2 = DecodeULEB128(ptr);
                break;

            case dwarf_op::implicit_value:
            {
                op.opcode = opcode;
                auto len = DecodeULEB128(ptr);
                op.op1 = uintptr_t(ptr);
                op.op2 = len;
                ptr += len;
                break;
            }

            default:
                // Unrecognised opcode.
                return false;
        }

        // Update the raw pointer and length fields.
        size_t length = ptr - original_ptr;
        op.raw = original_ptr;
        op.raw_length = length;

        // Have we exceeded the size of the input?
        if (length > len)
        {
            // We've hit some sort of decoding error.
            return false;
        }

        // Call the callback function with the decoded instruction.
        if (!callback(callback_context, op))
        {
            // Callback has requested the end of processing so exit successfully.
            return true;
        }

        // Adjust the length.
        len -= length;
    }

    // All instructions have been decoded successfully.
    return true;
}


#if 0 && !__SYSTEM_ABI_DWARF_MINIMAL
string dwarf_expr_op::toString() const
{
    switch (opcode)
    {
        case dwarf_op::addr:
            return Printf("DW_OP_addr %#" PRIxPTR, op1);

        case dwarf_op::deref:
            return "DW_OP_deref";

        case dwarf_op::const1u:
            return Printf("DW_OP_const1u %" PRIuPTR, op1);

        case dwarf_op::const1s:
            return Printf("DW_OP_const1s %" PRIdPTR, op1);

        case dwarf_op::const2u:
            return Printf("DW_OP_const2u %" PRIuPTR, op1);

        case dwarf_op::const2s:
            return Printf("DW_OP_const2s %" PRIdPTR, op1);

        case dwarf_op::const4u:
            return Printf("DW_OP_const4u %" PRIuPTR, op1);

        case dwarf_op::const4s:
            return Printf("DW_OP_const4s %" PRIdPTR, op1);

        case dwarf_op::const8u:
            return Printf("DW_OP_const8u %" PRIuPTR, op1);

        case dwarf_op::const8s:
            return Printf("DW_OP_const8s %" PRIdPTR, op1);

        case dwarf_op::constu:
            return Printf("DW_OP_constu %" PRIuPTR, op1);

        case dwarf_op::consts:
            return Printf("DW_OP_consts %" PRIdPTR, op1);

        case dwarf_op::dup:
            return "DW_op_dup";

        case dwarf_op::drop:
            return "DW_op_drop";

        case dwarf_op::over:
            return "DW_op_over";

        case dwarf_op::swap:
            return "DW_op_swap";

        case dwarf_op::rot:
            return "DW_op_rot";

        case dwarf_op::xderef:
            return "DW_OP_xderef";

        case dwarf_op::abs:
            return "DW_OP_abs";

        case dwarf_op::_and:
            return "DW_OP_and";

        case dwarf_op::div:
            return "DW_OP_div";

        case dwarf_op::minus:
            return "DW_OP_minus";

        case dwarf_op::mod:
            return "DW_OP_mod";

        case dwarf_op::mul:
            return "DW_OP_mul";

        case dwarf_op::neg:
            return "DW_OP_neg";

        case dwarf_op::_not:
            return "DW_OP_not";

        case dwarf_op::_or:
            return "DW_OP_or";

        case dwarf_op::plus:
            return "DW_OP_plus";

        case dwarf_op::shl:
            return "DW_OP_shl";

        case dwarf_op::shr:
            return "DW_OP_shr";

        case dwarf_op::shra:
            return "DW_OP_shra";

        case dwarf_op::_xor:
            return "DW_OP_xor";

        case dwarf_op::eq:
            return "DW_OP_eq";

        case dwarf_op::ge:
            return "DW_OP_ge";

        case dwarf_op::gt:
            return "DW_OP_gt";

        case dwarf_op::le:
            return "DW_OP_le";

        case dwarf_op::lt:
            return "DW_OP_lt";

        case dwarf_op::ne:
            return "DW_OP_ne";

        case dwarf_op::nop:
            return "DW_OP_nop";

        case dwarf_op::push_object_address:
            return "DW_OP_push_object_address";

        case dwarf_op::form_tls_address:
            return "DW_OP_form_tls_address";

        case dwarf_op::call_frame_cfa:
            return "DW_OP_call_frame_cfa";

        case dwarf_op::stack_value:
            return "DW_OP_stack_value";

        case dwarf_op::pick:
            return Printf("DW_OP_pick %" PRIuPTR, op1);

        case dwarf_op::plus_uconst:
            return Printf("DW_OP_plus_uconst %" PRIuPTR, op1);

        case dwarf_op::skip:
            return Printf("DW_OP_skip +%" PRIuPTR, op1);
            break;

        case dwarf_op::bra:
            return Printf("DW_OP_bra +%" PRIuPTR, op1);

        case dwarf_op::lit0:
        case dwarf_op::lit1:
        case dwarf_op::lit2:
        case dwarf_op::lit3:
        case dwarf_op::lit4:
        case dwarf_op::lit5:
        case dwarf_op::lit6:
        case dwarf_op::lit7:
        case dwarf_op::lit8:
        case dwarf_op::lit9:
        case dwarf_op::lit10:
        case dwarf_op::lit11:
        case dwarf_op::lit12:
        case dwarf_op::lit13:
        case dwarf_op::lit14:
        case dwarf_op::lit15:
        case dwarf_op::lit16:
        case dwarf_op::lit17:
        case dwarf_op::lit18:
        case dwarf_op::lit19:
        case dwarf_op::lit20:
        case dwarf_op::lit21:
        case dwarf_op::lit22:
        case dwarf_op::lit23:
        case dwarf_op::lit24:
        case dwarf_op::lit25:
        case dwarf_op::lit26:
        case dwarf_op::lit27:
        case dwarf_op::lit28:
        case dwarf_op::lit29:
        case dwarf_op::lit30:
        case dwarf_op::lit31:
            return Printf("DW_OP_lit%" PRIuPTR, op1);

        case dwarf_op::reg0:
        case dwarf_op::reg1:
        case dwarf_op::reg2:
        case dwarf_op::reg3:
        case dwarf_op::reg4:
        case dwarf_op::reg5:
        case dwarf_op::reg6:
        case dwarf_op::reg7:
        case dwarf_op::reg8:
        case dwarf_op::reg9:
        case dwarf_op::reg10:
        case dwarf_op::reg11:
        case dwarf_op::reg12:
        case dwarf_op::reg13:
        case dwarf_op::reg14:
        case dwarf_op::reg15:
        case dwarf_op::reg16:
        case dwarf_op::reg17:
        case dwarf_op::reg18:
        case dwarf_op::reg19:
        case dwarf_op::reg20:
        case dwarf_op::reg21:
        case dwarf_op::reg22:
        case dwarf_op::reg23:
        case dwarf_op::reg24:
        case dwarf_op::reg25:
        case dwarf_op::reg26:
        case dwarf_op::reg27:
        case dwarf_op::reg28:
        case dwarf_op::reg29:
        case dwarf_op::reg30:
        case dwarf_op::reg31:
        case dwarf_op::regx:
            if (raw_length == 1)
                return Printf("DW_OP_reg%" PRIuPTR, op1);
            else
                return Printf("DW_OP_regx %" PRIuPTR, op1);

        case dwarf_op::breg0:
        case dwarf_op::breg1:
        case dwarf_op::breg2:
        case dwarf_op::breg3:
        case dwarf_op::breg4:
        case dwarf_op::breg5:
        case dwarf_op::breg6:
        case dwarf_op::breg7:
        case dwarf_op::breg8:
        case dwarf_op::breg9:
        case dwarf_op::breg10:
        case dwarf_op::breg11:
        case dwarf_op::breg12:
        case dwarf_op::breg13:
        case dwarf_op::breg14:
        case dwarf_op::breg15:
        case dwarf_op::breg16:
        case dwarf_op::breg17:
        case dwarf_op::breg18:
        case dwarf_op::breg19:
        case dwarf_op::breg20:
        case dwarf_op::breg21:
        case dwarf_op::breg22:
        case dwarf_op::breg23:
        case dwarf_op::breg24:
        case dwarf_op::breg25:
        case dwarf_op::breg26:
        case dwarf_op::breg27:
        case dwarf_op::breg28:
        case dwarf_op::breg29:
        case dwarf_op::breg30:
        case dwarf_op::breg31:
        case dwarf_op::bregx:
        {
            const char* reg_name = FrameTraitsNative::GetRegisterName(FrameTraitsNative::reg_enum_t(op1));
            if (op1 < 32)
                return Printf("DW_OP_breg%" PRIuPTR " %" PRIdPTR " /* %s%+" PRIdPTR " */", op1, op2, reg_name, op2);
            else
                return Printf("DW_OP_bregx %" PRIuPTR ", %" PRIdPTR " /* %s%+" PRIdPTR " */", op1, op2, reg_name, op2);
        }

        case dwarf_op::fbreg:
            return Printf("DW_OP_fbreg %" PRIdPTR, op1);

        case dwarf_op::piece:
            return Printf("DW_OP_piece %" PRIuPTR, op1);

        case dwarf_op::deref_size:
            return Printf("DW_OP_deref_size %" PRIuPTR, op1);

        case dwarf_op::xderef_size:
            return Printf("DW_OP_xderef_size %" PRIuPTR, op1);

        case dwarf_op::call2:
            return Printf("DW_OP_call2 %#" PRIxPTR, op1);

        case dwarf_op::call4:
            return Printf("DW_OP_call4 %#" PRIxPTR, op1);

        case dwarf_op::call_ref:
            return Printf("DW_OP_call_ref %#" PRIxPTR, op1);

        case dwarf_op::bit_piece:
            return Printf("DW_OP_bit_piece %" PRIuPTR ", %" PRIuPTR, op1, op2);

        case dwarf_op::implicit_value:
            return Printf("DW_OP_implicit_value %#" PRIxPTR "+%" PRIdPTR, op1, op2);

        default:
            return "<unknown>";
    }
}
#endif // if !__SYSTEM_ABI_DWARF_MINIMAL


} // namespace System::ABI::Dwarf
