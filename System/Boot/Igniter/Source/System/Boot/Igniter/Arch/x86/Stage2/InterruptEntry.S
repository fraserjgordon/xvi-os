.code32

.altmacro
.macro interrupt index
    .global interruptEntry\index
    .type interruptEntry\index, "function"
    .size interruptEntry\index, 1f - interruptEntry\index
    interruptEntry\index&:

    // Push a zero error code for exceptions that don't have one pushed by the CPU.
    .if (\index == 8 || \index == 17 || ((10 <= \index) && (\index <= 14)))
        // Two byte no-op to keep all interrupt entry paths the same length.
        data16 nop
    .else
        pushl  $0
    .endif

    // Push the interrupt number.
    //
    // This is manually coded to prevent the assembler from optimising the size of the op.
    .byte 0x68; .long \index

    // Jump to the common interrupt handler code.
    jmp.d32 interrupt_common
1:
.endm

.macro generate_interrupt
    interrupt %((\@-1)/2)
.endm

.macro generate_interrupts
    .rept 256
        generate_interrupt
    .endr
.endm

generate_interrupts


.type interrupt_common, "function"
.size interrupt_common, interrupt_common_end - interrupt_common
interrupt_common:

    // Push all general-purpose registers.
    pushal

    // Push and reload all segment registers.
    movl    %ss, %eax
    movl    $0x30, %ecx
    movl    $0x38, %edx
    pushl   %ds
    pushl   %es
    pushl   %fs
    pushl   %gs
    movl    %eax, %ds
    movl    %eax, %es
    movl    %ecx, %fs
    movl    %edx, %gs

    // Call into the main interrupt handler. It takes a pointer to the interrupt context as a parameter.
    pushl   %esp
    call    handleInterrupt
    addl    $4, %esp

    // Restore the previous context.
    popl    %gs
    popl    %fs
    popl    %es
    popl    %ds
    popal
    addl    $8, %esp    // Vector and error code.
    iretl

interrupt_common_end:
