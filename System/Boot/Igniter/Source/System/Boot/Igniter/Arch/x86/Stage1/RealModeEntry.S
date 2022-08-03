#.arch i386
.code16
.section rmentry, "ax", @progbits


.org 0x0000

.global RealModeEntry
RealModeEntry:

    // Interrupts should already be disabled but make sure.
    cli

    // Some info is passed from the Stage0 to Stage1 loader via the stack.
    popl    %ecx

    // Set up the data segments.
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es

    // Save some parameters passed from the Stage0 loader.
    movb    %dl, boot_drive
    movl    %ecx, id_sector

    // Get the current cursor position.
    movw    $0x0300, %ax
    movb    $0, %bh
    int     $0x10

    // Print the welcome message, but at the start of the line (erasing any output from the Stage0 loader).
    movw    $0x1301, %ax
    movw    $0x0007, %bx
    movw    $boot_message_len, %cx
    movb    $0, %dl
    movw    $boot_message, %bp
    int     $0x10

    // Set up a minimal GDT so that we can switch to protected mode.
    lgdtl   GDT

    // Set the protected mode bit then perform a jump to ensure it takes.
    movl    %cr0, %eax
    orl     $1, %eax
    movl    %eax, %cr0
    ljmpl   $0x0008, $begin_32bit


boot_message: .ascii "XVI Igniter starting..."
boot_message_len = . - boot_message

boot_drive: .byte 0
id_sector: .long 0

GDT:
.word 47; .long GDT+0x10000; .word 0
.long 0x0000ffff; .long 0x00cf9a00
.long 0x0000ffff; .long 0x00cf9200
.long 0x0000ffff; .long 0x00009a01
.long 0x0000ffff; .long 0x00009201
.long 0x0000ffff; .long 0x00009200


.text

// 32-bit entry point once the GDT has been set up.
begin_32bit: .code32

    // Set up all of the segment registers as flat 32-bit segments.
    movl    $0x0010, %eax
    movl    $0x0020, %ecx
    xorl    %edx, %edx
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss
    movl    %ecx, %fs   // Data segment accessible from real mode.
    movl    %edx, %gs   // Null segment.

    // Set the stack pointer so that we have a stack. We place it in the lower 64kB of memory.
    movl    $0x10000, %esp
    xorl    %ebp, %ebp

    // Before executing any C++ code, ensure that the BSS memory is cleared.
    movl    $BSS_Start, %edi
    movl    $BSS_Size, %ecx
    movb    $0, %al
    rep stosb

    // Call the main function.
    movzxb  %fs:boot_drive, %eax
    pushl   %eax
    call    Stage1

halt:

    cli
    hlt
    jmp     halt
