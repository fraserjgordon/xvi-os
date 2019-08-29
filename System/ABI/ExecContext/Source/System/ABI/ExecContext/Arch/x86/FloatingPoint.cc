#include <System/ABI/ExecContext/Arch/x86/FloatingPoint.h>


using namespace System::ABI::ExecContext;


void $System$ABI$ExecContext$fnstenv(struct $System$ABI$ExecContext$fenv_t* ptr)
{
    fnstenv(*reinterpret_cast<fenv_t*>(ptr));
}

void $System$ABI$ExecContext$fnstenv_fldenv(struct $System$ABI$ExecContext$fenv_t* ptr)
{
    fnstenv_fldenv(*reinterpret_cast<fenv_t*>(ptr));
}

void $System$ABI$ExecContext$fldenv(const struct $System$ABI$ExecContext$fenv_t* ptr)
{
    fldenv(*reinterpret_cast<const fenv_t*>(ptr));
}

void $System$ABI$ExecContext$fnsave(struct $System$ABI$ExecContext$fsave_t* ptr)
{
    fnsave(*reinterpret_cast<fsave_t*>(ptr));
}

void $System$ABI$ExecContext$fnsave_fldenv(struct $System$ABI$ExecContext$fsave_t* ptr)
{
    fnsave_fldenv(*reinterpret_cast<fsave_t*>(ptr));
}

void $System$ABI$ExecContext$fninit()
{
    fninit();
}

void $System$ABI$ExecContext$fnclex()
{
    fnclex();
}

void $System$ABI$ExecContext$frstor(const struct $System$ABI$ExecContext$fsave_t* ptr)
{
    frstor(*reinterpret_cast<const fsave_t*>(ptr));
}

void $System$ABI$ExecContext$fxsave(struct $System$ABI$ExecContext$fxsave_t* ptr)
{
    fxsave(*reinterpret_cast<fxsave_t*>(ptr));
}

void $System$ABI$ExecContext$fxrstor(const struct $System$ABI$ExecContext$fxsave_t* ptr)
{
    fxrstor(*reinterpret_cast<const fxsave_t*>(ptr));
}
