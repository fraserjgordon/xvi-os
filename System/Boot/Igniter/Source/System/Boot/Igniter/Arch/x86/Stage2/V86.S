.section realmode.text, "ax", @progbits
.code16

.global V86Exit
.hidden V86Exit
V86Exit:

    // Special attention sequence (invalid opcode), with operation 0 (exit).
    .byte 0xc4, 0xc4, 0x00
