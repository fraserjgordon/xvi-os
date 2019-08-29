#include <System/HW/CPU/CPUID/Arch/x86/CPUID.hh>


extern int foo;
using namespace System::HW::CPU::CPUID;

void test()
{
	if (HasFeature(Feature::AVX))
		foo = 4;
	else if (HasFeature(Feature::SSE))
		foo = 3;
	else if (HasFeature(Feature::MMX))
		foo = 2;
	else if (HasFeature(Feature::Fpu))
		foo = 1;
	else
		foo = 0;
}
