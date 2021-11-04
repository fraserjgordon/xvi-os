// Work-around for a ppc64 linker bug - it doesn't handle .so files without an
// ABI version tag very well.
#ifdef _ARCH_PPC64
#  ifdef _CALL_ELF
#    define ABI_VER_(x) asm(".abiversion " #x);
#    define ABI_VER(x) ABI_VER_(x)
ABI_VER(_CALL_ELF)
#  endif
#endif
