#include <System/HW/CPU/Arch/x86/Segmentation/Descriptor.hh>


using namespace System::HW::CPU::X86;

SegmentDescriptor test = SegmentDescriptor::createUser(SegmentType::X);
