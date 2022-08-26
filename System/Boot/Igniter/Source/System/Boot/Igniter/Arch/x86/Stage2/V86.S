.section realmode.text, "x", @progbits
.code16

.global RMBIOSCallExit
RMBIOSCallExit:

    xorl    %eax, %eax
    int     $0x80
