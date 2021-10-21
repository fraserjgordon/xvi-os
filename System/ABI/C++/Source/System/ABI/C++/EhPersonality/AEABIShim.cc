// The ARM EHABI causes functions with unwind tables to generate "NONE" relocations that reference the AEABI personality
// routine(s) that will parse those tables. (These relocations just create a reference without actually modifying
// the location to which the relocation is applied). Because of this, any code that has unwind tables needs to have a
// definition of the personality routine available.
//
// Normally, the EhPersonality library will be linked. However, that library depends on a small number of other ABI
// libraries and those libraries may contain unwind tables. For those libraries, this file provides useless definitions
// of the personality routine symbols in order to satisfy the linker. Because the NONE relocations do not modify
// anything, it doesn't cause any problems for these to be the wrong symbol definitions. The right symbols will get
// used at runtime.

// Symbols are not exported; add this file as a source for all affected libraries.
extern "C"
{

void __aeabi_unwind_cpp_pr0();
void __aeabi_unwind_cpp_pr1();
void __aeabi_unwind_cpp_pr2();

void __aeabi_unwind_cpp_pr0() {}
void __aeabi_unwind_cpp_pr1() {}
void __aeabi_unwind_cpp_pr2() {}

}
