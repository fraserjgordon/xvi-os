###
### System.Boot.Igniter
###

cmake_minimum_required(VERSION 3.12)

project(System.Boot.Igniter)
enable_language(ASM)

set(SOURCES_STAGE0
)

set(SOURCES_STAGE0_x86

)

set(SOURCES_STAGE1
)

set(SOURCES_STAGE1_x86

)

set(SOURCES_STAGE2
    Memory/Bitmap.hh
    Multiboot/v1.hh
    Multiboot/v2.hh

    Memory/Bitmap.cc
)

set(SOURCES_STAGE2_x86
    Arch/x86/Stage2/Logging.hh
    Arch/x86/Stage2/Interrupts.hh
    Arch/x86/Stage2/Probe.hh
    Arch/x86/Stage2/V86.hh
    Arch/x86/Stage2/VGAConsole.hh

    Arch/x86/Stage2/Entry.S
    Arch/x86/Stage2/InterruptEntry.S

    Arch/x86/Stage2/Allocator.cc
    Arch/x86/Stage2/Entry.cc
    Arch/x86/Stage2/Interrupts.cc
    Arch/x86/Stage2/MultibootHeader.cc
    Arch/x86/Stage2/Probe.cc
    Arch/x86/Stage2/V86.cc
    Arch/x86/Stage2/VGAConsole.cc

    Arch/x86/Stage2/Stage2.ldscript
)

list(TRANSFORM SOURCES_STAGE2 PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Boot/Igniter/)
list(TRANSFORM SOURCES_STAGE2_x86 PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Boot/Igniter/)


xvi_executable(System.Boot.Igniter.Stage2  VERSION 0.1.0    BOOTLOADER)

target_include_directories(System.Boot.Igniter.Stage2
    PRIVATE     Source
)

target_sources(System.Boot.Igniter.Stage2
    PRIVATE     ${SOURCES_STAGE2}
                ${SOURCES_STAGE2_${XVI_ARCH}}
)

target_link_libraries(System.Boot.Igniter.Stage2
    PRIVATE     Kernel.Allocator.Classic
                Kernel.Firmware.EFI
                Kernel.Utility.Logger
)

target_link_options(System.Boot.Igniter.Stage2
    PRIVATE     -nostdlib
)

if(XVI_ARCH STREQUAL x86)

    target_link_libraries(System.Boot.Igniter.Stage2
        PRIVATE     Kernel.ABI.ExecContext
                    Kernel.Firmware.Arch.X86.BIOS
                    Kernel.HW.CPU.Arch.X86.IO
                    Kernel.HW.CPU.Arch.X86.MMU
                    Kernel.HW.CPU.Arch.X86.Segmentation
                    Kernel.HW.GPU.VGA
    )

    target_link_options(System.Boot.Igniter.Stage2
        PRIVATE     "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/Source/System/Boot/Igniter/Arch/x86/Stage2/Stage2.ldscript"
                    #"-Wl,-znocombreloc"
                    #"-Wl,-znotext"
                    #"-Wl,-znorelro"
                    #"-Wl,--emit-relocs"
                    "-Wl,--gc-sections"
                    "-Wl,-u,_ENTRY_MULTIBOOT"
                    "-Wl,-u,_ENTRY_EFI32"
                    "-Wl,-u,_ENTRY_EFI64"
                    "-Wl,-Bsymbolic"
    )

    add_custom_command(TARGET System.Boot.Igniter.Stage2 POST_BUILD
        COMMAND     ${CMAKE_OBJCOPY} ARGS   $<TARGET_FILE:System.Boot.Igniter.Stage2>
                                            $<TARGET_FILE_DIR:System.Boot.Igniter.Stage2>/Igniter.efi
                                            --target efi-app-i386
                                            --file-alignment 0x1000
                                            #--add-section .reloc=/dev/null
                                            #--change-addresses -0x00200000
                                            -j .text
                                            -j .rodata
                                            -j .eh_frame_hdr
                                            -j .eh_frame
                                            -j .gcc_except_table
                                            -j .data
                                            #-j .reloc
                                            #-j .rel
        COMMENT     "Creating Igniter.efi"
        VERBATIM
    )

endif()