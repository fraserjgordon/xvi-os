#include <System/ABI/Dwarf/CFI.hh>

#include <System/C/LanguageSupport/IntTypes.h>

#include <System/ABI/Dwarf/Decode.hh>
#include <System/ABI/Dwarf/Expression.hh>
#include <System/ABI/Dwarf/FDE.hh>
#include <System/ABI/Dwarf/LEB128.hh>


using namespace std;


namespace System::ABI::Dwarf
{


optional<dwarf_cfi_op>
CFIDecodeOne(const DwarfCIE& cie, const byte* ptr, size_t len)
{
    if (len <= 0)
        return {};

    // The decoded opcode.
    dwarf_cfi_op op = {.raw = ptr};

    // Read the first byte of the CFI instruction.
    auto opcode_ptr = ptr;
    auto opcode = *ptr;

    // What is the general opcode category?
    switch (dwarf_cfi_major(uint8_t(opcode) & kDwarfCFIMajorMask))
    {
        case dwarf_cfi_major::advance_loc:
        {
            // The instruction pointer is advanced by the encoded value multiplied by the code factor from the CIE.
            // This has the effect of creating a new row, containing the same values as the current row, for this
            // new IP value.
            ptr += 1;
            op.opcode = uint8_t(dwarf_cfi_major::advance_loc);
            op.op1 = (uint8_t(opcode) & kDwarfCFIMinorMask) * cie.getCodeFactor();
            break;
        }

        case dwarf_cfi_major::offset:
        {
            // Indicates that the register encoded into the opcode can be read at the ULEB128 encoded offset from
            // the CFA.
            ptr += 1;
            op.opcode = uint8_t(dwarf_cfi_major::offset);
            op.op1 = (uint8_t(opcode) & kDwarfCFIMinorMask);
            op.op2 = DecodeULEB128(ptr) * cie.getDataFactor();
            break;
        }

        case dwarf_cfi_major::restore:
        {
            // Changes the rule for the encoded register back to the rule from the CIE (i.e ignoring any changes
            // that instructions from the FDE have made).
            ptr += 1;
            op.opcode = uint8_t(dwarf_cfi_major::restore);
            op.op1 = (uint8_t(opcode) & kDwarfCFIMinorMask);
            break;
        }

        case dwarf_cfi_major::misc:
        {
            // The instruction is one of the "minor" opcodes. Examine the lower part of the opcode byte to find out
            // exactly what instruction this is.
            ptr += 1;
            op.opcode = uint8_t(opcode);
            switch (auto minor = dwarf_cfi_misc(uint8_t(opcode) & kDwarfCFIMinorMask); minor)
            {
                case dwarf_cfi_misc::nop:
                {
                    // This instruction does nothing but take up space.
                    // op()
                    break;
                }

                case dwarf_cfi_misc::set_loc:
                {
                    // Like the advance instructions but the instruction pointer is set to an absolute value (it is 
                    // stored using the FDE pointer encoding).
                    // op(encoded location)
                    DwarfPointerDecoderNative decoder = {};
                    auto loc = decoder.decode(cie.getFDEPointerEncoding(), ptr);
                    if (loc == 0)
                    {
                        // There was an error decoding the new location value.
                        return {};
                    }
                    op.op1 = loc;
                    break;
                }

                case dwarf_cfi_misc::advance_loc1:
                case dwarf_cfi_misc::advance_loc2:
                case dwarf_cfi_misc::advance_loc4:
                case dwarf_cfi_misc::mips_advance_loc8:
                {
                    // These are like the advance_loc major opcode but encoded differently. Read the parameter.
                    // op(delta)
                    if (minor == dwarf_cfi_misc::advance_loc1)
                    {
                        op.op1 = UnalignedRead<uint8_t>(ptr);
                        ptr += 1;
                    }
                    else if (minor == dwarf_cfi_misc::advance_loc2)
                    {
                        op.op1 = UnalignedRead<uint16_t>(ptr);
                        ptr += 2;
                    }
                    else if (minor == dwarf_cfi_misc::advance_loc4)
                    {
                        op.op1 = UnalignedRead<uint32_t>(ptr);
                        ptr += 4;
                    }
                    else // (minor == dwarf_cfi_misc::mips_advance_loc8)
                    {
                        //! @TODO: treat this opcode as an error for non-MIPS targets.
                        op.op1 = UnalignedRead<uint64_t>(ptr);
                        ptr += 8;
                    }

                    // Like dwarf_cfi_major::advance_loc, the delta is factored.
                    op.op1 *= cie.getCodeFactor();

                    break;
                }

                case dwarf_cfi_misc::offset_extended:
                {
                    // This has the same meaning as the offset major opcode but encodes the register differently.
                    // op(reg, factored offset)
                    op.op1 = DecodeULEB128(ptr);
                    op.op2 = DecodeULEB128(ptr) * cie.getDataFactor();
                    break;
                }

                case dwarf_cfi_misc::offset_extended_sf:
                {
                    // This has the same meaning as the offset major opcode but encodes both parameters differently.
                    // op(reg, signed factored offset)
                    op.op1 = DecodeULEB128(ptr);
                    op.op2 = DecodeSLEB128(ptr) * cie.getDataFactor();
                    break;
                }

                case dwarf_cfi_misc::restore_extended:
                {
                    // This has the same meaning as the retore major opcode but encodes the register differently.
                    // op(reg)
                    op.op1 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::undefined:
                {
                    // Indicates that the register cannot be restored.
                    // op(reg)
                    op.op1 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::same_value:
                {
                    // Indicates that the register still holds its old value.
                    // op(reg)
                    op.op1 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::reg:
                {
                    // Indicates that the value of the first register is available in a second register.
                    // op(reg, reg)
                    op.op1 = DecodeULEB128(ptr);
                    op.op2 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::remember_state:
                {
                    // Pushes the current row onto a stack.
                    // op()
                    break;
                }

                case dwarf_cfi_misc::restore_state:
                {
                    // Pops the top remembered state of the stack and replaces the current row with it.
                    // op()
                    break;
                }

                case dwarf_cfi_misc::def_cfa:
                {
                    // Defines the CFA to be given by a register and non-factored offset.
                    // op(reg, offset)
                    op.op1 = DecodeULEB128(ptr);
                    op.op2 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::def_cfa_sf:
                {
                    // Like def_cfa but the offset is signed and factored.
                    // op(reg, signed factored offset)
                    op.op1 = DecodeULEB128(ptr);
                    op.op2 = DecodeSLEB128(ptr) * cie.getDataFactor();
                    break;
                }

                case dwarf_cfi_misc::def_cfa_register:
                {
                    // Changes the register for the CFA rule but leaves the offset unchanged.
                    // op(reg)
                    op.op1 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::def_cfa_offset:
                {
                    // Changes the offset for the CFA rule but leaves the register unchanged.
                    // op(offset)
                    op.op1 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::def_cfa_offset_sf:
                {
                    // Like def_cfa_offset but the offset value is signed and factored.
                    // op(signed factored offset)
                    op.op1 = DecodeSLEB128(ptr) * cie.getDataFactor();
                    break;
                }

                case dwarf_cfi_misc::def_cfa_expression:
                {
                    // Defines the CFA to be given by the following DWARF expression.
                    // op(expression block ptr, expression block length)
                    auto len = DecodeULEB128(ptr);
                    op.op1 = uintptr_t(ptr);
                    op.op2 = len;
                    ptr += len;
                    break;
                }

                case dwarf_cfi_misc::expression:
                case dwarf_cfi_misc::val_expression:
                {
                    // Defines a register to be stored at the address given by the expression/have the value of the
                    // expression, respectively.
                    // op(expression block ptr, expression block length)
                    auto len = DecodeULEB128(ptr);
                    op.op1 = uintptr_t(ptr);
                    op.op2 = len;
                    ptr += len;
                    break;
                }

                case dwarf_cfi_misc::val_offset:
                {
                    // Defines a register to be given by the value of the CFA plus an offset.
                    // op(reg, factored offset)
                    op.op1 = DecodeULEB128(ptr);
                    op.op2 = DecodeULEB128(ptr) * cie.getDataFactor();
                    break;
                }

                case dwarf_cfi_misc::val_offset_sf:
                {
                    // Like val_offset but the offset is signed and factored.
                    op.op1 = DecodeULEB128(ptr);
                    op.op2 = DecodeSLEB128(ptr) * cie.getDataFactor();
                    break;
                }

                case dwarf_cfi_misc::gnu_window_save:
                {
                    // This opcode is a shortcut for encoding the behavior of the SPARC "save" instruction.
                    // op()
                    //
                    //! @TODO: this instruction should be treated as an error for non-SPARC targets.
                    break;
                }

                case dwarf_cfi_misc::gnu_args_size:
                {
                    // Encodes the size of the arguments pushed to the stack.
                    // op(arguments size)
                    op.op1 = DecodeULEB128(ptr);
                    break;
                }

                case dwarf_cfi_misc::gnu_negative_offset_extended:
                {
                    // Similar to (and obsoleted by) offset_extended_sf; the offset is unsigned but negated.
                    // op(negated factored offset)
                    op.op1 = -(DecodeULEB128(ptr) * cie.getDataFactor());
                    break;
                }

                default:
                {
                    // Unrecognised instruction.
                    return {};
                }
            }
        }
    }

    // Calculate the length of the instruction and verify we haven't exceeded the input length.
    size_t insn_length = ptr - opcode_ptr;
    if (insn_length > len)
    {
        // The instruction stream is malformed and we cannot continue decoding.
        return {};
    }
    op.raw_length = insn_length;
    len -= insn_length;

    return op;
}


bool
CFIDecode(const DwarfCIE& cie, const byte* ptr, size_t len, function<bool(const dwarf_cfi_op&)> callback)
{
    while (len > 0)
    {
        // Attempt to decode a single opcode.
        auto op = CFIDecodeOne(cie, ptr, len);
        if (!op)
        {
            // Failed to decode an op.
            return false;
        }

        // Pass the opcode to the callback.
        bool result = callback(*op);

        // Terminate if requested.
        if (!result)
            return true;

        // Move on to the next op.
        len -= op->raw_length;
        ptr = op->raw + op->raw_length;
    }

    /*for (auto op : cfi_decoder(cie, {ptr, len}))
    {
        // If there was a decoding error, exit.
        if (!op)
            return false;

        // Pass the opcode to the callback and terminate if requested.
        bool result = callback(*op);
        if (!result)
            return true;
    }*/

    // If we exited the processing loop cleanly, all instructions were decoded successfully.
    return true;
}


#if !__SYSTEM_ABI_DWARF_MINIMAL
// Formats the opcode bytes portion of a disassembled CFI opcode.
static string FormatOpcodeBytes(const byte* ptr, size_t len)
{
    // Preallocate space in the string.
    string str;
    str.reserve(3 * len);

    // Format the bytes.
    for (size_t offset = 0; offset < len; ++offset)
    {
        char op[4] = {};
        if (offset == 0)
            snprintf(op, sizeof(op), "%02hhx", ptr[offset]);
        else
            snprintf(op, sizeof(op), " %02hhx", ptr[offset]);
        str.append(op);
    }

    // Return the formatted bytes.
    return str;
}

// Returns the name of the given register.
static const char* RegName(uintptr_t r)
{
    return FrameTraitsNative::GetRegisterName(FrameTraitsNative::reg_enum_t(r));
}

// Decodes a non-expression CFI instruction into human-readable form.
static string FormatCFIOpcode(const dwarf_cfi_op& op)
{
    string str;
    string comment;
    switch (dwarf_cfi_misc(op.opcode))
    {
        case dwarf_cfi_misc::advance_loc:
        case dwarf_cfi_misc::advance_loc1:
        case dwarf_cfi_misc::advance_loc2:
        case dwarf_cfi_misc::advance_loc4:
        case dwarf_cfi_misc::mips_advance_loc8:
            str = Printf(". += %" PRIuPTR, op.op1);
            break;

        case dwarf_cfi_misc::set_loc:
            str = Printf(". = %" PRIxPTR, op.op1);
            break;

        case dwarf_cfi_misc::offset:
        case dwarf_cfi_misc::offset_extended:
        case dwarf_cfi_misc::offset_extended_sf:
        case dwarf_cfi_misc::gnu_negative_offset_extended:
            str = Printf(".cfi_offset %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::restore:
        case dwarf_cfi_misc::restore_extended:
            str = Printf(".cfi_restore %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::nop:
            str = ".cfi_nop";
            break;

        case dwarf_cfi_misc::undefined:
            str = Printf(".cfi_undefined %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::same_value:
            str = Printf(".cfi_same_value %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::reg:
            str = Printf(".cfi_register %s, %s", RegName(op.op1), RegName(op.op2));
            break;

        case dwarf_cfi_misc::remember_state:
            str = ".cfi_remember_state";
            break;

        case dwarf_cfi_misc::restore_state:
            str = ".cfi_restore_state";
            break;

        case dwarf_cfi_misc::def_cfa:
        case dwarf_cfi_misc::def_cfa_sf:
            str = Printf(".cfi_def_cfa %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::def_cfa_offset:
        case dwarf_cfi_misc::def_cfa_offset_sf:
            str = Printf(".cfi_def_cfa_offset %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::def_cfa_register:
            str = Printf(".cfi_def_cfa_register %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::val_offset:
        case dwarf_cfi_misc::val_offset_sf:
            str = Printf(".cfi_val_offset %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::gnu_window_save:
            str = ".cfi_window_save";
            break;

        case dwarf_cfi_misc::gnu_args_size:
            str = Printf(".cfi_args_size %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::expression:
        case dwarf_cfi_misc::val_expression:
        case dwarf_cfi_misc::def_cfa_expression:
        default:
            str = "<unknown opcode>";
            break;
    }

    switch (dwarf_cfi_misc(op.opcode))
    {
        case dwarf_cfi_misc::advance_loc:
            comment = Printf("DW_CFA_advance_loc %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::advance_loc1:
            comment = Printf("DW_CFA_advance_loc1 %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::advance_loc2:
            comment = Printf("DW_CFA_advance_loc2 %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::advance_loc4:
            comment = Printf("DW_CFA_advance_loc4 %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::mips_advance_loc8:
            comment = Printf("DW_MIPS_advance_loc8 %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::set_loc:
            comment = Printf("DW_CFA_set_loc %" PRIxPTR, op.op1);
            break;

        case dwarf_cfi_misc::offset:
            comment = Printf("DW_CFA_offset %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::offset_extended:
            comment = Printf("DW_CFA_offset_extended %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::offset_extended_sf:
            comment = Printf("DW_CFA_offset_extended_sf %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::gnu_negative_offset_extended:
            comment = Printf("DW_GNU_negative_offset_extended %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::restore:
            comment = Printf("DW_CFA_restore %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::restore_extended:
            comment = Printf("DW_CFA_restore_extended %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::nop:
            comment = "DW_CFA_nop";
            break;

        case dwarf_cfi_misc::undefined:
            comment = Printf("DW_CFA_undefined %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::same_value:
            comment = Printf("DW_CFA_same_value %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::reg:
            comment = Printf("DW_CFA_register %s, %s", RegName(op.op1), RegName(op.op2));
            break;

        case dwarf_cfi_misc::remember_state:
            comment = "DW_CFA_remember_state";
            break;

        case dwarf_cfi_misc::restore_state:
            comment = "DW_CFA_restore_state";
            break;

        case dwarf_cfi_misc::def_cfa:
            comment = Printf("DW_CFA_def_cfa %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::def_cfa_sf:
            comment = Printf("DW_CFA_def_cfa_sf %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::def_cfa_offset:
            comment = Printf("DW_CFA_def_cfa_offset %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::def_cfa_offset_sf:
            comment = Printf("DW_CFA_def_cfa_offset_sf %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::def_cfa_register:
            comment = Printf("DW_CFA_def_cfa_register %s", RegName(op.op1));
            break;

        case dwarf_cfi_misc::val_offset:
            comment = Printf("DW_CFA_val_offset %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::val_offset_sf:
            comment = Printf("DW_CFA_val_offset_sf %s, %" PRIdPTR, RegName(op.op1), op.op2);
            break;

        case dwarf_cfi_misc::gnu_window_save:
            comment = "DW_GNU_window_save";
            break;

        case dwarf_cfi_misc::gnu_args_size:
            comment = Printf("DW_GNU_args_size %" PRIdPTR, op.op1);
            break;

        case dwarf_cfi_misc::expression:
        case dwarf_cfi_misc::val_expression:
        case dwarf_cfi_misc::def_cfa_expression:
        default:
            break;
    }

    // Form the final string.
    auto bytes = FormatOpcodeBytes(op.raw, op.raw_length);
    return Printf("%-32s:  %-32s%s%s", bytes.c_str(), str.c_str(), (comment.empty() ? "" : " # "), comment.c_str());
}

string FormatDwarfExpressionOpcode(const dwarf_cfi_op& cfi_op)
{
    // Only format the bytes of the opcodes that correspond to the CFI opcode (i.e not the DWARF opcodes).
    auto bytes = FormatOpcodeBytes(cfi_op.raw, cfi_op.raw_length - cfi_op.op2);

    // What kind of expression are we dealing with?
    string str;
    string comment;
    auto opcode = dwarf_cfi_misc(cfi_op.opcode);
    if (opcode == dwarf_cfi_misc::expression)
    {
        str = ".cfi_expression";
        comment = "DW_CFA_expression";
    }
    else if (opcode == dwarf_cfi_misc::val_expression)
    {
        str = ".cfi_val_expression";
        comment = "DW_CFA_val_expression";
    }
    else // (opcode == dwarf_cfi_misc::def_cfa_expression)
    {
        str = ".cfi_def_cfa_expression";
        comment = "DW_CFA_def_cfa_expression";
    }

    // Format the first line.
    //str = Printf("%s [...] (%" PRIuPTR " bytes)", str.c_str(), op.op2);
    str = Printf("%s [...]", str.c_str());
    string first = Printf("%-32s:  %-32s # %s", bytes.c_str(), str.c_str(), comment.c_str());

    // Format the DWARF expression opcodes.
    string dwarf_ops;
    bool result = DwarfExpressionDecode(reinterpret_cast<const byte*>(cfi_op.op1), cfi_op.op2, [&](auto op)
    {
        // Format the address.
        auto address = Printf("(+%ju)", op.raw - cfi_op.raw);

        // Format the raw bytes of the opcode.
        auto bytes = FormatOpcodeBytes(op.raw, op.raw_length);

        // Format the opcode itself.
        auto formatted = op.toString();

        // Create the entire line for this expression opcode.
        int width = 2 * sizeof(uintptr_t);
        dwarf_ops.append(Printf("\n|%*s|: %-32s   -- %s", width, address.c_str(), bytes.c_str(), formatted.c_str()));

        // Continue parsing.
        return true;
    });
    if (!result)
    {
        // Failed to decode all of the DWARF opcodes.
        dwarf_ops.append(" *** decode failed ***");
    }

    return first + dwarf_ops;
}

string DisassembleCFI(const DwarfCIE& cie, const byte* ptr, size_t len)
{
    // The string we'll be accumulating the output into.
    string str;

    // Decode the instructions.
    bool result = CFIDecode(cie, ptr, len, [&](auto op)
    {
        // The format is:
        //  <address> <opcode bytes> <decoded instruction>

        // Format the address of the instruction.
        int width = (sizeof(uintptr_t) * 2);
        auto address = Printf("%*" PRIxPTR, width, uintptr_t(op.raw));

        // What type of instruction is this? Expression opcodes require special handling.
        switch (dwarf_cfi_misc(op.opcode))
        {
            case dwarf_cfi_misc::expression:
            case dwarf_cfi_misc::val_expression:
            case dwarf_cfi_misc::def_cfa_expression:
            {
                auto decoded = FormatDwarfExpressionOpcode(op);
                str.append(Printf("|%s|: %s\n", address.c_str(), decoded.c_str()));
                break;
            }

            default:
            {
                // Format the opcode normally.
                auto decoded = FormatCFIOpcode(op);
                str.append(Printf("|%s|: %s\n", address.c_str(), decoded.c_str()));
            }
        }

        // Keep consuming opcodes until all have been processed.
        return true;
    });

    // If decoding failed, append something to indicate that.
    if (!result)
    {
        str.append("\n  *** decode failed ***\n");
    }

    // Formatting complete.
    return str;
}
#endif // if !__SYSTEM_ABI_DWARF_MINIMAL


} // namespace System::ABI::Dwarf
