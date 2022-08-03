#.arch i386
.code32
.text

.global BIOSCall
.type BIOSCall @function
BIOSCall:

    // Frame setup.
    pushal
    leal    32(%esp), %ebp

    // Ensure we have the current GDT info saved in case the BIOS alters it.
    subl    $8, %esp
    sgdtl   (%esp)

    // Switch to 16-bit protected mode as the first step of getting back into real mode.
    ljmpl   $0x0018, $.LBIOSCallPM16

.LBIOSCallReturn:

    // Reconfigure the data and stack segments.
    movl    $0x0010, %eax
    movl    $0x0020, %ecx
    xorl    %edx, %edx
    movl    %eax, %ds
    movl    %eax, %es
    movl    %ecx, %fs
    movl    %ecx, %gs

    // Call complete.
    popal
    ret

.code16
.section realmode, "ax", @progbits

.LBIOSCallPM16:

    // Reload the stack segment to be a 16-bit segment covering the first 64k of RAM (as that's where our shared stack
    // lives). There's no need to reload the data segments quite yet - we can continue to use the 32-bit ones.
    movw    $0x0028, %ax
    movw    %ax, %ss

    // Switch into real mode.
    movl    %cr0, %eax
    andb    $0xFE, %al
    movl    %eax, %cr0

    // Long jump to complete the transition.
    ljmpw   $0x1000, $.LBIOSCallRM

.LBIOSCallRM:

    // Set up the stack segment for real mode.
    movw    $0x0000, %ax
    movw    %ax, %ss

    // Create a fake interrupt return frame.
    pushfw
    pushw   %cs
    pushw   $.LBIOSCallRMReturn

    // And also use the stack to store the interrupt destination.
    movl    4(%bp), %eax
    pushl   %ss:0(, %eax, 4)

    // Temporarily point the stack pointer at the register save area and load the registers.
    movw    8(%bp), %bp
    movw    %sp, 12(%bp)    // Stash the true stack pointer as we're about to trash %bp.
    movw    %bp, %sp
    popal
    popl    %ds
    popl    %es
    subw    $28, %sp
    popw    %sp

    // We're finished playing games with the stack so it is now safe to re-enable interrupts.
    sti

    // Perform a long return to pop the interrupt destination off the stack but leave the interrupt return frame in
    // place for when the BIOS call returns.
    lretw

.LBIOSCallRMReturn:

    // Store all of the registers as they are now onto the stack.
    pushfl
    pushw   $0
    pushw   %es
    pushw   $0
    pushw   %ds
    pushal

    // Disable interrupts as we're going to be adjusting the stack. Also clear the direction flag so we maintain the
    // correct ABI when we return to C++ code.
    cli
    cld

    // Ensure all data segments refer to the same segment as the stack.
    movw    %ss, %ax
    movw    %ax, %ds
    movw    %ax, %es

    // Copy this to the register save area.
    movw    %sp, %si
    movw    92(%si), %di
    movw    $44, %cx
    rep     movsb
    addw    $44, %sp

    // Restore the GDT.
    movw    %sp, %bp
    lgdtl   (%bp)
    addw    $8, %sp

    // Return to 32-bit protected mode.
    movl    %cr0, %eax
    orl     $0x01, %eax
    movl    %eax, %cr0
    ljmpl   $0x0008, $.LBIOSCallReturn
