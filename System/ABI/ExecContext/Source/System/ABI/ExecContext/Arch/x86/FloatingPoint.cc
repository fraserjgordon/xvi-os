#include <System/ABI/ExecContext/Arch/x86/FloatingPoint.hh>


namespace System::ABI::ExecContext
{


void _fnstenv(fenv_t* ptr)
{
    fnstenv(*ptr);
}

void _fnstenv_fldenv(fenv_t* ptr)
{
    fnstenv_fldenv(*ptr);
}

void _fldenv(const fenv_t* ptr)
{
    fldenv(*ptr);
}

void _fnsave(fsave_t* ptr)
{
    fnsave(*ptr);
}

void _fnsave_fldenv(fsave_t* ptr)
{
    fnsave_fldenv(*ptr);
}

void _fninit()
{
    fninit();
}

void _fnclex()
{
    fnclex();
}

void _frstor(const fsave_t* ptr)
{
    frstor(*ptr);
}

[[gnu::target("sse")]]
void _fxsave(fxsave_t* ptr)
{
    fxsave(*ptr);
}

[[gnu::target("sse")]]
void _fxrstor(const fxsave_t* ptr)
{
    fxrstor(*ptr);
}


} // namespace System::ABI::ExecContext

