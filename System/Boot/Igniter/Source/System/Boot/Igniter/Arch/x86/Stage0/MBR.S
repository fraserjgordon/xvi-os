.arch i8086
.code16
.text

// Global variables
//  (or their addresses anyway)

relocate_to             = 0x0600

block_list_temp         = 0x0800    // 512 bytes - storage for block list

next_stage_start_addr   = 0x0a00    // seg:offset - first block of next stage
next_stage_load_ptr     = 0x0a04    // seg:offset - where the next block goes
block_number_ptr        = 0x0a08    // offset - ptr to next block number to load
boot_drive              = 0x0a0a    // byte - boot drive number


// The MBR generally contains a BIOS Parameter Block (BPB) or similar structure
// that is important to FAT-family file systems. Jump over it.
//
// This intro needs to be exactly three bytes long.
//
// Interesting registers at entry:
//
//  %dl     BIOS boot drive number
//
start:

    jmp.d8      mbr_begin
    nop

.org 0x0003

// OEM signature.
.space 8, 0xCC

// BIOS Parameter Block. Contents unspecified here.
.space 0x4f, 0xCC

// Hard-coded parameters are written into the bootsector here during setup. The
// main purpose is to locate the next stage of the loader.
next_stage_block_list_block:    .long 0xCCCCCCCC


// The MBR proper begins here. The goal is to read the next stage into memory.
// This is done in stages: first, a block is read that contains the list of
// other blocks to load. Then, those blocks are read into memory and executed.
mbr_begin:

    // Ensure interrupts are disabled.
    cli

    // Relocate this bootsector in case we're going to chainload. Set up the stack pointer too.
    cld
    xorw    %bx, %bx
    movw    $0x7c00, %si
    movw    $relocate_to, %di
    movw    %bx, %ds
    movw    %bx, %es
    movw    %bx, %ss
    movw    %si, %sp
    movw    $512, %cx
    rep movsw

    // Jump to the relocated code.
    ljmpw   $0, $1f
1: 

    // Save the boot drive.
    movb    %dl, boot_drive

    movb    $'$', %cl
    call    print_char

    // Chainload, if we have a partition table that tells us to.
    //
    // Note: jmp instead of call as it saves a byte here.
    jmp     maybe_chainload

 not_chainload:
    // Load the (first) block of the next stage block list.
    movw    next_stage_block_list_block, %ax
    movw    next_stage_block_list_block+2, %cx
    movw    $block_list_temp, %di
    pushw   %di
    call    load_block

    // Read the load address of the next stage.
    popw    %si
    movw    $next_stage_start_addr, %di
    movsw
    movsw

    // And copy the same value to the current load pointer.
    subw    $4, %si
    movsw
    movsw

// Loop for processing the block list blocks.
block_list_loop:

    // Update the block list source pointer too (in case the block list itself
    // needs more than one block of storage).
    movw    $next_stage_block_list_block, %di
    movsw
    movsw

    // Next within the block list is the first block to load.
    movw    %di, block_number_ptr

    movb    $':', %cl
    call    print_char

    // Main block loading loop.
block_loop:
    
    // Load the block number.
    movw    block_number_ptr, %si
    movw    0(%si), %ax
    movw    2(%si), %cx

    // Load the block destination info.
    movw    next_stage_load_ptr, %di
    movw    next_stage_load_ptr+2, %es

    // Load the block.
    call    load_block

    movb    $'.', %cl
    call    print_char

    // Are we at the end of this block list block?
    movw    block_number_ptr, %ax
    addw    $4, %ax
    cmpw    $(block_list_temp+512), %ax
    je      1f
    movw    %ax, block_number_ptr
    addw    $(512 / 16), next_stage_load_ptr+2
    jmp     block_loop

// More fixed (non-code) data goes here.
.skip ((start + 0x00DA) - .), 0x90
.space 6, 0xCC

1:  // Have we reached the end of the list of block list blocks?
    movw    next_stage_block_list_block, %ax
    movw    next_stage_block_list_block+2, %cx
    movw    %ax, %dx
    orw     %cx, %dx    // Zero iff 32-bit value cx:ax == 0.   
    jz      2f

    // No - load the next block list block.
    movw    block_list_temp, %di
    call    load_block

    // Prepare to loop through a block list again.
    movw    $block_list_temp, %si
    jmp     block_list_loop

2:  // Loading is complete. Prepare to execute the next stage.

    movb    $'!', %cl
    call    print_char

    // Make the jump.
    ljmpw   *next_stage_start_addr
    

// void maybe_chainload()
maybe_chainload:

    // Do we have exactly one boot partition?
    xorw    %si, %si
    movw    $partition_table, %bx
    movw    $4, %cx
1:  andb    $0x80, 0(%bx)
    jz      2f
    testw   %si, %si
    jnz     3f              // Two boot partitions. Invalid.
2:  addw    $16, %bx
    loopw   1b
    testw   %si, %si
    jz      3f              // No boot partitions.

    // Exactly one boot partition is in the table. Load it.
    // (only LBA is supported here).
    movw    $0x7c00, %di
    movw    8(%si), %ax
    movw    10(%si), %cx
    pushw   %si
    call    load_block
    popw    %si

    // Restore the boot drive number and jump to the new bootsector.
    //
    // Note that %ds:%si points to the partition entry we loaded as some bootsectors expect that.
    movb    boot_drive, %dl
    ljmp    $0x0000, $0x7c00

    // No valid chainload found.
3:  jmp     not_chainload

// void load_block(cx:ax : uint32_t lba, es:di : void *__far to)
load_block:

    movb    boot_drive, %dl

    // Is this a hard drive read request? If so, we use the LBA directly.
    testb   $0x80, %dl
    jnz     load_block_lba

    // Number of times we're going to try the read.
    movw    $3, %bx
    pushw   %bx

    // The "LBA" address is really in CHS format (it was converted at the time
    // of installation, taking the floppy disk geometry into account).
    movb    %ah, %dh
    movw    $0x0201, %ax    // Function 0x02, 1 sector to read
    movw    %di, %bx

1:  // Store the parameters on the stack so we can retry then execute.
    pushw   %ax
    pushw   %cx
    pushw   %dx
    int     $0x13
    jnc     2f

    // Read failed. Do we have any attempts remaining?
    popw    %dx
    popw    %cx
    popw    %ax
    movw    %sp, %bp
    decw    0(%bp)
    jz      load_block_failed

    // Try again.
    jmp     1b

2:  // Function succeeded.
    addw    $8, %sp
    ret

load_block_lba:

    // Generate the disk address packet
    xorw    %dx, %dx
    pushw   %dx     // Upper half of upper 32 bits of LBA (zero).
    pushw   %dx     // Lower half of upper 32 bits of LBA (zero).
    pushw   %cx     // Upper half of lower 32 bits of LBA.
    pushw   %ax     // Lower half of lower 32 bits of LBA.
    pushw   %es     // Destination segment.
    pushw   %di     // Destination offset.
    incw    %dx
    pushw   %dx     // Number of blocks to transfer (one).
    movw    $0x0010, %ax
    pushw   %ax     // Packet size.

    // Number of attempts remaining.
    addw    $2, %dx
    pushw   %dx
    
    // Load the boot drive number.
1:  movb    boot_drive, %dl

    // Configure the other parameters.
    movb    $0x42, %ah      // Function code 0x42.
    movw    %sp, %si        // Disk address packet is in %ds:%si

    // Invoke the read routine.
    int     $0x13

    // Did the read succeed?
    jnc     2f

    // Do we have any read attempts remaining?
    movw    %sp, %bp
    decb    (%bp)
    jz      load_block_failed

    // Try again.
    jmp     1b

2:  // Read complete. Restore the stack and return.
    addw    $0x0010, %sp
    retw

load_block_failed:
    // Read failed. Unable to continue.
    movb    $'?', %cl
    call    print_char

    // Wait for 30 seconds.
    movw    $(30000000 >> 16), %cx
    movw    $(30000000 & 0xFFFF), %dx
    movb    $0x86, %ah
    int     $0x15

    // Perform a warm reboot.
    movw    $0x1234, 0x472
    ljmpw   $0xFFFF, $0x0000

// void print_char(cl : char c)
print_char:

    movb    $0x0E, %ah
    movb    %cl, %al
    movw    $0x0007, %bx
    int     $0x10
    retw

// Disk signature.
.skip ((start + 0x01B8) - .), 0x90
.space 6, 0xCC

// Space reserved for partition table entries.
.skip ((start + 0x01BE) - .), 0x90
partition_table: .space 16*4, 0xCC

// Boot sector footer. Used to indicate that this bootsector is actually
// bootable. Without this, most BIOSes will refuse to execute this sector.
.word 0xAA55    // Magic signature marking this as bootable.
