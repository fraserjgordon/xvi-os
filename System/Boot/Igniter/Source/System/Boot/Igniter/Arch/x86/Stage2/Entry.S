.text

.global _IgniterStage2MultibootEntry
_IgniterStage2MultibootEntry:

    // On entry from a Multiboot loader, %eax and %ebx contain values we need to preserve.
    //  %eax    - magic number indicating a Multiboot-compliant loader
    //  %ebx    - physical address of Multiboot information structure

    // The Multiboot spec requires interrupts to be disabled upon entry but there's no harm in making sure!
    cli

    // Clear the direction flag as the ABI for x86 requires it to be clear before calling any functions.
    cld

    // Calculate the load address of this executable. We don't have a stack set up yet so we pick a location in memory
    // that we know exists (in this case, the address where the boot sector gets loaded) and then we use it as a
    // temporary one-entry stack so we can get the instruction pointer. The contents of that word are preserved as there
    // the Multiboot loader could have put something important there.
    movl    0x7c00, %ebp
    movl    $0x7c04, %esp
    call    1f
1:  movl    (%esp), %ecx
    movl    %ebp, (%ebp)
    subl    $1b, %ecx

    // %ecx now contains the delta between the load address and the expected (link) address. We need to set up the
    // segment registers such that we can execute this image without processing any of the relocations.
    movl    %ecx, %edi
    addl    $InitGDT, %edi

    // Flat code segment descriptor.
    movl    $0x0000ffff, 8+0(%edi)
    movl    $0x00cf9a00, 8+4(%edi)

    // Flat data segment descriptor.
    movl    $0x0000ffff, 16+0(%edi)
    movl    $0x00cf9200, 16+4(%edi)

    // Adjusted code segment descriptor.
    movl    %ecx, 24+4(%edi)
    movl    %ecx, 24+2(%edi)
    movw    $0xffff, 24+0(%edi)
    movw    $0xcf9a, 24+5(%edi)

    // Adjusted data segment descriptor.
    movl    %ecx, 32+4(%edi)
    movl    %ecx, 32+2(%edi)
    movw    $0xffff, 32+0(%edi)
    movw    $0xcf92, 32+5(%edi)

    // Write a GDT descriptor into the unused space at the beginning of the GDT.
    movl    $(InitGDTSize - 1), 0(%edi)
    movl    %edi, 2(%edi)

    // Make our temporary GDT active then reload all of the segment descriptors. The code segment is reloaded by
    // performing a long jump; the rest can be reloaded directly.
    lgdtl   (%edi)
    ljmpl   $0x0018, $2f
2:  movl    $0x0020, %esi
    xorl    %edx, %edx
    movl    %esi, %ds
    movl    %esi, %es
    movl    %esi, %ss
    movl    %edx, %fs
    movl    %edx, %gs

    // Store the load offset into a global variable as later parts of the loader will need it.
    movl    %ecx, g_loadOffset

    // We can now set up a small, initial stack so that C++ code can be called.
    movl    $InitStackTop, %esp
    xorl    %ebp, %ebp

    // And now that we have a stack, we can stack the Multiboot information and free up some registers.
    pushl   %ebx
    pushl   %eax

    // Everything is ready to call into C++ code now.
    call    multibootEntry

    // The above call shouldn't return. If it does, halt.
3:  cli
    hlt
    jmp 3b


.bss

InitGDTSize = (8 * 5)
InitGDT = g_GDT

InitStackBottom: .space 0x10000; InitStackTop:
