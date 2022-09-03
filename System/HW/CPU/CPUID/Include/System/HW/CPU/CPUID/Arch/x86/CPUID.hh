#pragma once
#ifndef __SYSTEM_HW_CPU_CPUID_ARCH_X86_CPUID_H
#define __SYSTEM_HW_CPU_CPUID_ARCH_X86_CPUID_H


#if __XVI_NO_STDLIB
#  include <System/C++/LanguageSupport/StdInt.hh>
#else
#  include <cstdint>
#endif


namespace System::HW::CPU::CPUID
{


enum class Vendor
{
    // Silicon manufacturers.
    AMD,                    // "AuthenticAMD" or "AMDisbetter!"
    Centaur,                // "CentaurHauls"
    Cyrix,                  // "CyrixInstead"
    Hygon,                  // "HygonGenuine"
    Intel,                  // "GenuineIntel"
    Transmeta,              // "TransmetaCPU" or "GenuineTMx86"
    NationalSemiconductor,  // "Geode by NSC"
    NexGen,                 // "NexGenDriven"
    Rise,                   // "RiseRiseRise"
    SiS,                    // "SiS SiS SiS "
    UMC,                    // "UMC UMC UMC "
    VIA,                    // "VIA VIA VIA "
    Vortex,                 // "Vortex86 SoC"

    // Virtual machines and hypervisors.
    Bhyve,                  // "bhyve bhyve "
    LinuxKVM,               // "KVMKVMKVM\0\0\0"
    MicrosoftHyperV,        // "Microsoft Hv"
    Parallels,              // " lrpepyh vr" ??? - too short!
    VMWare,                 // "VMwareVMware"
    Xen,                    // "XenVMMXenVMM"

    // Other.
    Unknown = -1,
};

enum class Register : unsigned char
{
    EAX = 0,
    EBX = 1,
    ECX = 2,
    EDX = 3,
};

struct results_t
{
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
};

struct feature_flag_t
{
    unsigned int leaf;
    unsigned int subleaf = 0;
    bool use_subleaf = false;
    Register     where;
    unsigned int mask;
    signed int   shift = 0;
};

struct cpuid_support_t
{

};


constexpr results_t VendorID(const char *ID)
{   
    auto first = static_cast<unsigned int>(ID[0] | (ID[1] << 8) | (ID[2] << 16) | (ID[3] << 24));
    auto second = static_cast<unsigned int>(ID[4] | (ID[5] << 8) | (ID[6] << 16) | (ID[7] << 24));
    auto third = static_cast<unsigned int>(ID[8] | (ID[9] << 8) | (ID[10] << 16) | (ID[11] << 24));
    return { .eax = 0, .ebx = first, .ecx = third, .edx = second };
};

constexpr bool VendorMatches(results_t a, results_t b)
{
    // EAX is ignored.
    return a.ebx == b.ebx && a.ecx == b.ecx && a.edx == b.edx;
}

constexpr Vendor DecodeVendor(results_t r)
{
    // These are arranged in approximate likelyhood order (i.e AMD and Intel first and then everything else).
    
    // The two main x86-compatible CPU vendors.
    if (VendorMatches(r, VendorID("AuthenticAMD")))
        return Vendor::AMD;
    if (VendorMatches(r, VendorID("GenuineIntel")))
        return Vendor::Intel;

    // The virtual machine vendors.
    if (VendorMatches(r, VendorID("bhyve bhyve ")))
        return Vendor::Bhyve;
    if (VendorMatches(r, VendorID("KVMKVMKVM\0\0\0")))
        return Vendor::LinuxKVM;
    if (VendorMatches(r, VendorID("Microsoft Hv")))
        return Vendor::MicrosoftHyperV;
    if (VendorMatches(r, VendorID(" lrpepyh rv")))
        return Vendor::Parallels;
    if (VendorMatches(r, VendorID("VMwareVMware")))
        return Vendor::VMWare;
    if (VendorMatches(r, VendorID("XenVMMXenVMM")))
        return Vendor::Xen;

    // Less common harware manufacturers.
    if (VendorMatches(r, VendorID("AMDisbetter!")))
        return Vendor::AMD;
    if (VendorMatches(r, VendorID("CentaurHauls")))
        return Vendor::Centaur;
    if (VendorMatches(r, VendorID("CyrixInstead")))
        return Vendor::Cyrix;
    if (VendorMatches(r, VendorID("HygonGenuine")))
        return Vendor::Hygon;
    if (VendorMatches(r, VendorID("TransmetaCPU")) || VendorMatches(r, VendorID("GenuineTMx86")))
        return Vendor::Transmeta;
    if (VendorMatches(r, VendorID("Geode by NSC")))
        return Vendor::NationalSemiconductor;
    if (VendorMatches(r, VendorID("NexGenDriven")))
        return Vendor::NexGen;
    if (VendorMatches(r, VendorID("RiseRiseRise")))
        return Vendor::Rise;
    if (VendorMatches(r, VendorID("SiS SiS SiS ")))
        return Vendor::SiS;
    if (VendorMatches(r, VendorID("UMC UMC UMC ")))
        return Vendor::UMC;
    if (VendorMatches(r, VendorID("VIA VIA VIA ")))
        return Vendor::VIA;
    if (VendorMatches(r, VendorID("Vortex86 SoC")))
        return Vendor::Vortex;

    // Unknown vendor.
    return Vendor::Unknown;
}

// Sanity checks.
//static_assert(DecodeVendor(VendorID("AuthenticAMD")) == Vendor::AMD);
//static_assert(DecodeVendor(VendorID("GenuineIntel")) == Vendor::Intel);


// CPUID leaves.
static constexpr unsigned int LeafManufacturer      = 0;    //!< Basic CPU identification.
static constexpr unsigned int LeafModelAndFeatures  = 1;
static constexpr unsigned int LeafCacheInfo         = 2;
static constexpr unsigned int LeafSerialNumber      = 3;
static constexpr unsigned int LeafExtendedFeatures  = 7;
static constexpr unsigned int LeafXsaveInfo         = 13;

static constexpr unsigned int HypervisorLeafStart   = 0x40000000;
static constexpr unsigned int HypervisorLeafEnd     = 0x400000FF;

static constexpr unsigned int XLeafManufacturer     = 0x80000000;
static constexpr unsigned int XLeafModelAndFeatures = 0x80000001;
static constexpr unsigned int XLeafBrandString1     = 0x80000002;
static constexpr unsigned int XLeafBrandString2     = 0x80000003;
static constexpr unsigned int XLeafBrandString3     = 0x80000004;
static constexpr unsigned int XLeafL1CacheTLBInfo   = 0x80000005;
static constexpr unsigned int XLeafL2L3CacheTLBInfo = 0x80000006;
static constexpr unsigned int XLeafSVMInfo          = 0x8000000A;


namespace Feature
{

// Commonly-implemented feature flags. These are (relatively) safe to check without validaing the CPU vendor first.
inline namespace Generic
{
}

// Features defined by Intel (though not necessarily exclusive to Intel CPUs).
namespace Intel
{
}

// Features defined by AMD (though not necessarily exclusive to AMD CPUs).
namespace AMD
{
}

// EAX for EAX=1: CPU model identification fields.
static constexpr feature_flag_t Stepping  = {.leaf = 1, .where = Register::EAX, .mask = 0x0000000f, .shift = 0};
static constexpr feature_flag_t Model     = {.leaf = 1, .where = Register::EAX, .mask = 0x000000f0, .shift = 4};
static constexpr feature_flag_t Family    = {.leaf = 1, .where = Register::EAX, .mask = 0x00000f00, .shift = 8};
static constexpr feature_flag_t CpuType   = {.leaf = 1, .where = Register::EAX, .mask = 0x00003000, .shift = 12};
static constexpr feature_flag_t ExtModel  = {.leaf = 1, .where = Register::EAX, .mask = 0x000f0000, .shift = 16};
static constexpr feature_flag_t ExtFamily = {.leaf = 1, .where = Register::EAX, .mask = 0x0ff00000, .shift = 20};

// EBX for EAX=1
static constexpr feature_flag_t BrandIndex         = {.leaf = 1, .where = Register::EBX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t ClflushSize        = {.leaf = 1, .where = Register::EBX, .mask = 0x0000ff00, .shift = 5};  //!< Note: raw value is bytecount/8
static constexpr feature_flag_t LogicalCPUCount    = {.leaf = 1, .where = Register::EBX, .mask = 0x00ff0000, .shift = 16};
static constexpr feature_flag_t LocalAcpiId        = {.leaf = 1, .where = Register::EBX, .mask = 0xff000000, .shift = 24};

// EDX for EAX=1
static constexpr feature_flag_t Fpu                = {.leaf = 1, .where = Register::EDX, .mask = (1<<0)};
static constexpr feature_flag_t Vme                = {.leaf = 1, .where = Register::EDX, .mask = (1<<1)};
static constexpr feature_flag_t DebugExtensions    = {.leaf = 1, .where = Register::EDX, .mask = (1<<2)};
static constexpr feature_flag_t PageSizeExtension  = {.leaf = 1, .where = Register::EDX, .mask = (1<<3)};
static constexpr feature_flag_t TimeStampCounter   = {.leaf = 1, .where = Register::EDX, .mask = (1<<4)};
static constexpr feature_flag_t ModelSpecificRegisters = {.leaf = 1, .where = Register::EDX, .mask = (1<<5)};
static constexpr feature_flag_t PhysicalAddressExtension = {.leaf = 1, .where = Register::EDX, .mask = (1<<6)};
static constexpr feature_flag_t MachineCheckException = {.leaf = 1, .where = Register::EDX, .mask = (1<<7)};
static constexpr feature_flag_t CompareExchange8B  = {.leaf = 1, .where = Register::EDX, .mask = (1<<8)};
static constexpr feature_flag_t Apic               = {.leaf = 1, .where = Register::EDX, .mask = (1<<9)};
static constexpr feature_flag_t Sysenter           = {.leaf = 1, .where = Register::EDX, .mask = (1<<11)};
static constexpr feature_flag_t Mtrr               = {.leaf = 1, .where = Register::EDX, .mask = (1<<12)};
static constexpr feature_flag_t GlobalPages        = {.leaf = 1, .where = Register::EDX, .mask = (1<<13)};
static constexpr feature_flag_t MachineCheckArch   = {.leaf = 1, .where = Register::EDX, .mask = (1<<14)};
static constexpr feature_flag_t ConditionalMove    = {.leaf = 1, .where = Register::EDX, .mask = (1<<15)};
static constexpr feature_flag_t PageAttributeTable = {.leaf = 1, .where = Register::EDX, .mask = (1<<16)};
static constexpr feature_flag_t Pse36              = {.leaf = 1, .where = Register::EDX, .mask = (1<<17)};
static constexpr feature_flag_t HasSerialNumber    = {.leaf = 1, .where = Register::EDX, .mask = (1<<18)};
static constexpr feature_flag_t Clflush            = {.leaf = 1, .where = Register::EDX, .mask = (1<<19)};
static constexpr feature_flag_t DebugStore         = {.leaf = 1, .where = Register::EDX, .mask = (1<<21)};
static constexpr feature_flag_t AcpiThermalMsrs    = {.leaf = 1, .where = Register::EDX, .mask = (1<<22)};
static constexpr feature_flag_t MMX                = {.leaf = 1, .where = Register::EDX, .mask = (1<<23)};
static constexpr feature_flag_t Fxsave             = {.leaf = 1, .where = Register::EDX, .mask = (1<<24)};
static constexpr feature_flag_t SSE                = {.leaf = 1, .where = Register::EDX, .mask = (1<<25)};
static constexpr feature_flag_t SSE2               = {.leaf = 1, .where = Register::EDX, .mask = (1<<26)};
static constexpr feature_flag_t CacheSelfSnoop     = {.leaf = 1, .where = Register::EDX, .mask = (1<<27)};
static constexpr feature_flag_t HyperThreading     = {.leaf = 1, .where = Register::EDX, .mask = (1<<28)};
static constexpr feature_flag_t ThermalMonitor     = {.leaf = 1, .where = Register::EDX, .mask = (1<<29)};
static constexpr feature_flag_t IsItanium          = {.leaf = 1, .where = Register::EDX, .mask = (1<<30)};
static constexpr feature_flag_t PendingBreakPin    = {.leaf = 1, .where = Register::EDX, .mask = (1U<<31)};

// ECX for EAX=1
static constexpr feature_flag_t SSE3               = {.leaf = 1, .where = Register::ECX, .mask = (1<<0)};
static constexpr feature_flag_t Pclmulqdq          = {.leaf = 1, .where = Register::ECX, .mask = (1<<1)};
static constexpr feature_flag_t DebugStore64       = {.leaf = 1, .where = Register::ECX, .mask = (1<<2)};
static constexpr feature_flag_t MonitorMwait       = {.leaf = 1, .where = Register::ECX, .mask = (1<<3)};
static constexpr feature_flag_t DebugStoreCpl      = {.leaf = 1, .where = Register::ECX, .mask = (1<<4)};
static constexpr feature_flag_t Vmx                = {.leaf = 1, .where = Register::ECX, .mask = (1<<5)};
static constexpr feature_flag_t Smx                = {.leaf = 1, .where = Register::ECX, .mask = (1<<6)};
static constexpr feature_flag_t EnhancedSpeedstep  = {.leaf = 1, .where = Register::ECX, .mask = (1<<7)};
static constexpr feature_flag_t ThermalMonitor2    = {.leaf = 1, .where = Register::ECX, .mask = (1<<8)};
static constexpr feature_flag_t SSSE3              = {.leaf = 1, .where = Register::ECX, .mask = (1<<9)};
static constexpr feature_flag_t L1ContextId        = {.leaf = 1, .where = Register::ECX, .mask = (1<<10)};
static constexpr feature_flag_t SiDebugInterface   = {.leaf = 1, .where = Register::ECX, .mask = (1<<11)};
static constexpr feature_flag_t FusedMultiplyAdd   = {.leaf = 1, .where = Register::ECX, .mask = (1<<12)};
static constexpr feature_flag_t CompareExchange16B = {.leaf = 1, .where = Register::ECX, .mask = (1<<13)};
static constexpr feature_flag_t DisableTprMsg      = {.leaf = 1, .where = Register::ECX, .mask = (1<<14)};
static constexpr feature_flag_t Perfmon            = {.leaf = 1, .where = Register::ECX, .mask = (1<<15)};
static constexpr feature_flag_t ProcessContextId   = {.leaf = 1, .where = Register::ECX, .mask = (1<<17)};
static constexpr feature_flag_t DirectCacheAccess  = {.leaf = 1, .where = Register::ECX, .mask = (1<<18)};
static constexpr feature_flag_t SSE4_1             = {.leaf = 1, .where = Register::ECX, .mask = (1<<19)};
static constexpr feature_flag_t SSE4_2             = {.leaf = 1, .where = Register::ECX, .mask = (1<<20)};
static constexpr feature_flag_t X2Apic             = {.leaf = 1, .where = Register::ECX, .mask = (1<<21)};
static constexpr feature_flag_t Movbe              = {.leaf = 1, .where = Register::ECX, .mask = (1<<22)};
static constexpr feature_flag_t Popcnt             = {.leaf = 1, .where = Register::ECX, .mask = (1<<23)};
static constexpr feature_flag_t DeadlineTsc        = {.leaf = 1, .where = Register::ECX, .mask = (1<<24)};
static constexpr feature_flag_t AES                = {.leaf = 1, .where = Register::ECX, .mask = (1<<25)};
static constexpr feature_flag_t Xsave              = {.leaf = 1, .where = Register::ECX, .mask = (1<<26)};
static constexpr feature_flag_t XsaveEnabled       = {.leaf = 1, .where = Register::ECX, .mask = (1<<27)};
static constexpr feature_flag_t AVX                = {.leaf = 1, .where = Register::ECX, .mask = (1<<28)};
static constexpr feature_flag_t Float16            = {.leaf = 1, .where = Register::ECX, .mask = (1<<29)};
static constexpr feature_flag_t Rdrand             = {.leaf = 1, .where = Register::ECX, .mask = (1<<30)};
static constexpr feature_flag_t HypervisorRunning  = {.leaf = 1, .where = Register::ECX, .mask = (1U<<31)};

// EAX for EAX=5
static constexpr feature_flag_t MonitorLineSizeMin = {.leaf = 5, .where = Register::EAX, .mask = (0x0000ffff)};

// EBX for EAX=5
static constexpr feature_flag_t MonitorLineSizeMax = {.leaf = 5, .where = Register::EBX, .mask = (0x0000ffff)};

// ECX for EAX=5
static constexpr feature_flag_t MonitorMwaitEMX    = {.leaf = 5, .where = Register::ECX, .mask = (1<<0)};
static constexpr feature_flag_t MonitorMwaitIBE    = {.leaf = 5, .where = Register::ECX, .mask = (1<<1)};

// EAX for EAX=6
static constexpr feature_flag_t APICTimerARAT      = {.leaf = 6, .where = Register::EAX, .mask = (1<<1)};

// ECX for EAX=6
static constexpr feature_flag_t ACPITimerEffFreq   = {.leaf = 6, .where = Register::ECX, .mask = (1<<0)};

// EAX for EAX=7
static constexpr feature_flag_t MaxExtFeatureSubId = {.leaf = 7, .where = Register::EAX, .mask = ~0U};

// EBX for EAX=7,ECX=0
static constexpr feature_flag_t FsGsBase           = {.leaf = 7, .where = Register::EBX, .mask = (1<<0)};
static constexpr feature_flag_t TscAdjust          = {.leaf = 7, .where = Register::EBX, .mask = (1<<1)};
static constexpr feature_flag_t SoftwareGuard      = {.leaf = 7, .where = Register::EBX, .mask = (1<<2)};
static constexpr feature_flag_t BitManipulation1   = {.leaf = 7, .where = Register::EBX, .mask = (1<<3)};
static constexpr feature_flag_t TransactionalHLE   = {.leaf = 7, .where = Register::EBX, .mask = (1<<4)};
static constexpr feature_flag_t AVX2               = {.leaf = 7, .where = Register::EBX, .mask = (1<<5)};
static constexpr feature_flag_t SvExecProtection   = {.leaf = 7, .where = Register::EBX, .mask = (1<<7)};
static constexpr feature_flag_t BitManipulation2   = {.leaf = 7, .where = Register::EBX, .mask = (1<<8)};
static constexpr feature_flag_t EnhancedRepMovsb   = {.leaf = 7, .where = Register::EBX, .mask = (1<<9)};
static constexpr feature_flag_t Invpcid            = {.leaf = 7, .where = Register::EBX, .mask = (1<<10)};
static constexpr feature_flag_t TransactionalRTM   = {.leaf = 7, .where = Register::EBX, .mask = (1<<11)};
static constexpr feature_flag_t PlatformQoSM       = {.leaf = 7, .where = Register::EBX, .mask = (1<<12)};
static constexpr feature_flag_t FpuSegDeprecated   = {.leaf = 7, .where = Register::EBX, .mask = (1<<13)};
static constexpr feature_flag_t IntelMpx           = {.leaf = 7, .where = Register::EBX, .mask = (1<<14)};
static constexpr feature_flag_t PlatformQoSE       = {.leaf = 7, .where = Register::EBX, .mask = (1<<15)};
static constexpr feature_flag_t Avx512             = {.leaf = 7, .where = Register::EBX, .mask = (1<<16)};
static constexpr feature_flag_t Avx512DQ           = {.leaf = 7, .where = Register::EBX, .mask = (1<<17)};
static constexpr feature_flag_t Rdseed             = {.leaf = 7, .where = Register::EBX, .mask = (1<<18)};
static constexpr feature_flag_t AddCarryExtensions = {.leaf = 7, .where = Register::EBX, .mask = (1<<19)};
static constexpr feature_flag_t SvAccessProtection = {.leaf = 7, .where = Register::EBX, .mask = (1<<20)};
static constexpr feature_flag_t AvxIntegerFma      = {.leaf = 7, .where = Register::EBX, .mask = (1<<21)};
static constexpr feature_flag_t Pcommit            = {.leaf = 7, .where = Register::EBX, .mask = (1<<22)};
static constexpr feature_flag_t Clflushopt         = {.leaf = 7, .where = Register::EBX, .mask = (1<<23)};
static constexpr feature_flag_t Clwb               = {.leaf = 7, .where = Register::EBX, .mask = (1<<24)};
static constexpr feature_flag_t ProcessorTrace     = {.leaf = 7, .where = Register::EBX, .mask = (1<<25)};
static constexpr feature_flag_t Avx512Prefetch     = {.leaf = 7, .where = Register::EBX, .mask = (1<<26)};
static constexpr feature_flag_t Avx512ER           = {.leaf = 7, .where = Register::EBX, .mask = (1<<27)};
static constexpr feature_flag_t Avx512CD           = {.leaf = 7, .where = Register::EBX, .mask = (1<<28)};
static constexpr feature_flag_t SHA                = {.leaf = 7, .where = Register::EBX, .mask = (1<<29)};
static constexpr feature_flag_t Avx512BW           = {.leaf = 7, .where = Register::EBX, .mask = (1<<30)};
static constexpr feature_flag_t Avx512VL           = {.leaf = 7, .where = Register::EBX, .mask = (1U<<31)};

// ECX for EAX=7,ECX=0
static constexpr feature_flag_t Prefetchwt1        = {.leaf = 7, .where = Register::ECX, .mask = (1<<0)};
static constexpr feature_flag_t Avx512VBMI         = {.leaf = 7, .where = Register::ECX, .mask = (1<<1)};
static constexpr feature_flag_t Umip               = {.leaf = 7, .where = Register::ECX, .mask = (1<<2)};
static constexpr feature_flag_t UserMemoryKeys     = {.leaf = 7, .where = Register::ECX, .mask = (1<<3)};
static constexpr feature_flag_t UserMemoryKeysOS   = {.leaf = 7, .where = Register::ECX, .mask = (1<<4)};
static constexpr feature_flag_t Avx512VBMI2        = {.leaf = 7, .where = Register::ECX, .mask = (1<<6)};
static constexpr feature_flag_t GaloisField        = {.leaf = 7, .where = Register::ECX, .mask = (1<<8)};
static constexpr feature_flag_t VectorAes          = {.leaf = 7, .where = Register::ECX, .mask = (1<<9)};
static constexpr feature_flag_t Vpclmulqdq         = {.leaf = 7, .where = Register::ECX, .mask = (1<<10)};
static constexpr feature_flag_t Avx512VNNI         = {.leaf = 7, .where = Register::ECX, .mask = (1<<11)};
static constexpr feature_flag_t Avx512BitAlg       = {.leaf = 7, .where = Register::ECX, .mask = (1<<12)};
static constexpr feature_flag_t Avx512PopcountDQ   = {.leaf = 7, .where = Register::ECX, .mask = (1<<14)};
static constexpr feature_flag_t MpxWidthAdjust     = {.leaf = 7, .where = Register::ECX, .mask = 0x003e0000, .shift = 17};
static constexpr feature_flag_t Rdpid              = {.leaf = 7, .where = Register::ECX, .mask = (1<<22)};
static constexpr feature_flag_t SgxLaunchConfig    = {.leaf = 7, .where = Register::ECX, .mask = (1<<30)};

// EDX for EAX=7,ECX=0
static constexpr feature_flag_t Avx512VNNIW4       = {.leaf = 7, .where = Register::EDX, .mask = (1<<2)};
static constexpr feature_flag_t Avx512Fmaps4       = {.leaf = 7, .where = Register::EDX, .mask = (1<<3)};
static constexpr feature_flag_t PlatformConfig     = {.leaf = 7, .where = Register::EDX, .mask = (1<<18)};
static constexpr feature_flag_t IbrsIbpbControl    = {.leaf = 7, .where = Register::EDX, .mask = (1<<26)};
static constexpr feature_flag_t StibpControl       = {.leaf = 7, .where = Register::EDX, .mask = (1<<27)};
static constexpr feature_flag_t Ia32ArchCapsMsr    = {.leaf = 7, .where = Register::EDX, .mask = (1<<29)};
static constexpr feature_flag_t Ssbd               = {.leaf = 7, .where = Register::EDX, .mask = (1U<<31)};

// Registers for EAX=0x0D,ECX=0
static constexpr feature_flag_t XFeatureSupportedMaskLow = {.leaf = 0x0D, .subleaf = 0, .use_subleaf = true, .where = Register::EAX, .mask = ~0U};
static constexpr feature_flag_t XFeatureEnabledSize = {.leaf = 0x0D, .subleaf = 0, .use_subleaf = true, .where = Register::EBX, .mask = ~0U};
static constexpr feature_flag_t XFeatureMaxSize    = {.leaf = 0x0D, .subleaf = 0, .use_subleaf = true, .where = Register::ECX, .mask = ~0U};
static constexpr feature_flag_t XFeatureSupposedMaskHigh = {.leaf = 0x0D, .subleaf = 0, .use_subleaf = true, .where = Register::EDX, .mask = ~0U};

// EAX for EAX=0x0D,ECX=1
static constexpr feature_flag_t XSaveOpt           = {.leaf = 0x0D, .subleaf = 1, .use_subleaf = true, .where = Register::EAX, .mask = (1<<0)};

// Registers for EAX=0x0D,ECX=2
static constexpr feature_flag_t XFeatureYmmSaveSize = {.leaf = 0x0D, .subleaf = 2, .use_subleaf = true, .where = Register::EAX, .mask = ~0U};
static constexpr feature_flag_t XFeatureYmmSaveOffset = {.leaf = 0x0D, .subleaf = 2, .use_subleaf = true, .where = Register::EBX, .mask = ~0U};

// Registers for EAX=0x0D,ECX=0x3E
static constexpr feature_flag_t XFeatureLwpSaveSize = {.leaf = 0x0D, .subleaf = 0x3E, .use_subleaf = true, .where = Register::EAX, .mask = ~0U};
static constexpr feature_flag_t XFeatureLwpSaveOffset = {.leaf = 0x0D, .subleaf = 0x3E, .use_subleaf = true, .where = Register::EBX, .mask = ~0U};

// Registers for EAX=0x80000000
static constexpr feature_flag_t MaxExtLeaf         = {.leaf = 0x80000000, .where = Register::EAX, .mask = ~0U};
static constexpr feature_flag_t ExtVendorId1       = {.leaf = 0x80000000, .where = Register::EBX, .mask = ~0U};
static constexpr feature_flag_t ExtVendorId2       = {.leaf = 0x80000000, .where = Register::ECX, .mask = ~0U};
static constexpr feature_flag_t ExtVendorId3       = {.leaf = 0x80000000, .where = Register::EDX, .mask = ~0U};

// EDX for EAX=0x80000001
static constexpr feature_flag_t Syscall            = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<11)};
static constexpr feature_flag_t Multiprocessor     = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<19)};
static constexpr feature_flag_t NoExecute          = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<20)};
static constexpr feature_flag_t ExtendedMMX        = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<22)};
static constexpr feature_flag_t FxsaveOptimisation = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<25)};
static constexpr feature_flag_t Page1G             = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<26)};
static constexpr feature_flag_t Rdtscp             = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<27)};
static constexpr feature_flag_t LongMode           = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<29)};
static constexpr feature_flag_t Extended3DNow      = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1<<30)};
static constexpr feature_flag_t Amd3DNow           = {.leaf = 0x80000001, .where = Register::EDX, .mask = (1U<<31)};

// ECX for EAX=0x80000001
static constexpr feature_flag_t LahfLM             = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<0)};
static constexpr feature_flag_t CmpLegacy          = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<1)};
static constexpr feature_flag_t Svm                = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<2)};
static constexpr feature_flag_t ExtendedApic       = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<3)};
static constexpr feature_flag_t Cr8Legacy          = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<4)};
static constexpr feature_flag_t AdvancedBitManip   = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<5)};
static constexpr feature_flag_t SSE4a              = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<6)};
static constexpr feature_flag_t MisalignedSSE      = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<7)};
static constexpr feature_flag_t Prefetch3DNow      = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<8)};
static constexpr feature_flag_t OsVisibleWorkaround= {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<9)};
static constexpr feature_flag_t InsnBasedSampling  = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<10)};
static constexpr feature_flag_t Xop                = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<11)};
static constexpr feature_flag_t Skinit             = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<12)};
static constexpr feature_flag_t WatchdogTimer      = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<13)};
static constexpr feature_flag_t LightweightProfile = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<15)};
static constexpr feature_flag_t Fma4               = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<16)};
static constexpr feature_flag_t XlateCacheExt      = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<17)};
static constexpr feature_flag_t NodeIdMsr          = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<19)};
static constexpr feature_flag_t TrailingBitManip   = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<21)};
static constexpr feature_flag_t TopologyExt        = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<22)};
static constexpr feature_flag_t CorePerfmon        = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<23)};
static constexpr feature_flag_t NbPerfmon          = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<24)};
static constexpr feature_flag_t DataBreakpoint     = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<26)};
static constexpr feature_flag_t PerformanceTsc     = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<27)};
static constexpr feature_flag_t L2IPerfmon         = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<28)};
static constexpr feature_flag_t MonitorxMwaitx     = {.leaf = 0x80000001, .where = Register::ECX, .mask = (1<<29)};

// EBX for EAX=0x80000001
static constexpr feature_flag_t BrandId            = {.leaf = 0x80000001, .where = Register::EBX, .mask = 0x0000ffff};
static constexpr feature_flag_t PackageType        = {.leaf = 0x80000001, .where = Register::EBX, .mask = 0xf0000000};

// EAX for EAX=0x80000005
static constexpr feature_flag_t L1ITLB2M4MSize     = {.leaf = 0x80000005, .where = Register::EAX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t L1ITLB2M4MAssoc    = {.leaf = 0x80000005, .where = Register::EAX, .mask = 0x0000ff00, .shift = 8};
static constexpr feature_flag_t L1DTLB2M4MSize     = {.leaf = 0x80000005, .where = Register::EAX, .mask = 0x00ff0000, .shift = 16};
static constexpr feature_flag_t L1DTLB2M4MAssoc    = {.leaf = 0x80000005, .where = Register::EAX, .mask = 0xff000000, .shift = 24};

// EBX for EAX=0x80000005
static constexpr feature_flag_t L1ITLB4kSize       = {.leaf = 0x80000005, .where = Register::EBX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t L1ITLB4kAssoc      = {.leaf = 0x80000005, .where = Register::EBX, .mask = 0x0000ff00, .shift = 8};
static constexpr feature_flag_t L1DTLB4kSize       = {.leaf = 0x80000005, .where = Register::EBX, .mask = 0x00ff0000, .shift = 16};
static constexpr feature_flag_t L1DTLB4kAssoc      = {.leaf = 0x80000005, .where = Register::EBX, .mask = 0xff000000, .shift = 24};

// ECX for EAX=0x80000005
static constexpr feature_flag_t L1DataLineSize     = {.leaf = 0x80000005, .where = Register::ECX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t L1DataLinesPerTag  = {.leaf = 0x80000005, .where = Register::ECX, .mask = 0x0000ff00, .shift = 8};
static constexpr feature_flag_t L1DataAssoc        = {.leaf = 0x80000005, .where = Register::ECX, .mask = 0x00ff0000, .shift = 16};
static constexpr feature_flag_t L1DataSize         = {.leaf = 0x80000005, .where = Register::ECX, .mask = 0xff000000, .shift = 14}; // Raw value is in kB.

// EDX for EAX=0x80000005
static constexpr feature_flag_t L1InsnLineSize     = {.leaf = 0x80000005, .where = Register::EDX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t L1InsnLinesPerTag  = {.leaf = 0x80000005, .where = Register::EDX, .mask = 0x0000ff00, .shift = 8};
static constexpr feature_flag_t L1InsnAssoc        = {.leaf = 0x80000005, .where = Register::EDX, .mask = 0x00ff0000, .shift = 16};
static constexpr feature_flag_t L1InsnSize         = {.leaf = 0x80000005, .where = Register::EDX, .mask = 0xff000000, .shift = 14}; // Raw value is in kB.

// EAX for EAX=0x80000006
static constexpr feature_flag_t L2ITLB2M4MSize     = {.leaf = 0x80000006, .where = Register::EAX, .mask = 0x00000fff, .shift = 0};
static constexpr feature_flag_t L2ITLB2M4MAssoc    = {.leaf = 0x80000006, .where = Register::EAX, .mask = 0x0000f000, .shift = 12}; // Note: non-trivial encoding.
static constexpr feature_flag_t L2DTLB2M4MSize     = {.leaf = 0x80000006, .where = Register::EAX, .mask = 0x0fff0000, .shift = 16};
static constexpr feature_flag_t L2DTLB2M4MAssoc    = {.leaf = 0x80000006, .where = Register::EAX, .mask = 0xf0000000, .shift = 28}; // Note: non-trivial encoding.

// EBX for EAX=0x80000006
static constexpr feature_flag_t L2ITLB4kSize       = {.leaf = 0x80000006, .where = Register::EBX, .mask = 0x00000fff, .shift = 0};
static constexpr feature_flag_t L2ITLB4kAssoc      = {.leaf = 0x80000006, .where = Register::EBX, .mask = 0x0000f000, .shift = 12}; // Note: non-trivial encoding.
static constexpr feature_flag_t L2DTLB4kSize       = {.leaf = 0x80000006, .where = Register::EBX, .mask = 0x0fff0000, .shift = 16};
static constexpr feature_flag_t L2DTLB4kAssoc      = {.leaf = 0x80000006, .where = Register::EBX, .mask = 0xf0000000, .shift = 28}; // Note: non-trivial encoding.

// ECX for EAX=0x80000006
static constexpr feature_flag_t L2CacheLineSize    = {.leaf = 0x80000006, .where = Register::ECX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t L2CacheLinesPerTag = {.leaf = 0x80000006, .where = Register::ECX, .mask = 0x00000f00, .shift = 8};
static constexpr feature_flag_t L2CacheAssoc       = {.leaf = 0x80000006, .where = Register::ECX, .mask = 0x0000f000, .shift = 12}; // Note: non-trivial encoding.
static constexpr feature_flag_t L2CacheSize        = {.leaf = 0x80000006, .where = Register::ECX, .mask = 0xffff0000, .shift = 6}; // Raw value is in kB.

// EDX for EAX=0x80000006
static constexpr feature_flag_t L3CacheLineSize    = {.leaf = 0x80000006, .where = Register::EDX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t L3CacheLinesPerTag = {.leaf = 0x80000006, .where = Register::EDX, .mask = 0x00000f00, .shift = 8};
static constexpr feature_flag_t L3CacheAssoc       = {.leaf = 0x80000006, .where = Register::EDX, .mask = 0x0000f000, .shift = 12}; // Note: non-trivial encoding.
static constexpr feature_flag_t L3CacheSize        = {.leaf = 0x80000006, .where = Register::EDX, .mask = 0xfffc0000, .shift = 0}; // Raw value is in 512kB.

// EAX for EAX=0x80000008
static constexpr feature_flag_t PhysicalAddrBits   = {.leaf = 0x80000008, .where = Register::EAX, .mask = 0x000000ff, .shift = 0};
static constexpr feature_flag_t LinearAddrBits     = {.leaf = 0x80000008, .where = Register::EAX, .mask = 0x0000ff00, .shift = 8};
static constexpr feature_flag_t GuestPhysicalAddrBits = {.leaf = 0x80000008, .where = Register::EAX, .mask = 0x00ff0000, .shift = 16};

// EBX for EAX=0x80000008
static constexpr feature_flag_t Clzero             = {.leaf = 0x80000008, .where = Register::EBX, .mask = (1<<0)};
static constexpr feature_flag_t InsnRetiredMSR     = {.leaf = 0x80000008, .where = Register::EBX, .mask = (1<<1)};
static constexpr feature_flag_t FPErrorPtrXrstor   = {.leaf = 0x80000008, .where = Register::EBX, .mask = (1<<2)};

// ECX for EAX=0x80000008
static constexpr feature_flag_t PhysicalCoreCount  = {.leaf = 0x80000008, .where = Register::ECX, .mask = 0x000000ff, .shift = 0}; // Number of cores minus 1.
static constexpr feature_flag_t ACPIIDCoreIDSize   = {.leaf = 0x80000008, .where = Register::ECX, .mask = 0x0000f000, .shift = 12};
static constexpr feature_flag_t PerformanceTscSize = {.leaf = 0x80000008, .where = Register::ECX, .mask = 0x00030000, .shift = 16}; // Note: non-trivial encoding.

// EAX for EAX=0x8000000A
static constexpr feature_flag_t SvmRevision        = {.leaf = 0x8000000A, .where = Register::EAX, .mask = 0x000000ff, .shift = 0};

// EBX for EAX=0x8000000A
static constexpr feature_flag_t NumberASID         = {.leaf = 0x8000000A, .where = Register::EBX, .mask = ~0U};

// EDX for EAX=0x8000000A
static constexpr feature_flag_t SvmNestedPaging    = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<0)};
static constexpr feature_flag_t SvmVirtLBR         = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<1)};
static constexpr feature_flag_t SvmLock            = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<2)};
static constexpr feature_flag_t SvmNRIPSave        = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<3)};
static constexpr feature_flag_t SvmTscRateMSR      = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<4)};
static constexpr feature_flag_t SvmVCMBClean       = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<5)};
static constexpr feature_flag_t SvmFlushByASID     = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<6)};
static constexpr feature_flag_t SvmDecodeAssists   = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<7)};
static constexpr feature_flag_t SvmPauseFilter     = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<10)};
static constexpr feature_flag_t SvmPauseFilterLimit= {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<12)};
static constexpr feature_flag_t SvmAVIC            = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<13)};
static constexpr feature_flag_t SvmVirtVMSave      = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<15)};
static constexpr feature_flag_t SvmVGIF            = {.leaf = 0x8000000A, .where = Register::EDX, .mask = (1<<16)};    

} // namespace Feature


#if defined(__amd64__)
constexpr bool SupportsCPUIDRaw()
{
    return true;
}

constexpr bool SupportsCPUID()
{
    return true;
}
#else
[[gnu::const]]
inline bool SupportsCPUIDRaw()
{
    // See if the ID flag (bit 17) of the %eflags register can be modified.
    unsigned int before, after;
    asm
    (
        "pushfl     \n\t"
        "pushfl     \n\t"
        "xorl       $(1<<21), (%%esp)\n\t"
        "popfl      \n\t"
        "pushfl     \n\t"
        "popl       %[after]\n\t"
        "popl       %[before]\n\t"
        : [before] "=r" (before), [after] "=r" (after)
    );
    return ((before ^ after) & (1U<<21));
}

[[gnu::const]]
inline bool SupportsCPUID()
{
    static const bool supported = SupportsCPUIDRaw();
    return supported;
}
#endif


[[gnu::const]]
inline results_t GetLeaf(unsigned int leaf)
{
    results_t results;
    asm
    (
        "cpuid"
        : "=a" (results.eax),
          "=b" (results.ebx),
          "=c" (results.ecx),
          "=d" (results.edx)
        : "a" (leaf)
    );
    return results;
}

[[gnu::const]]
inline results_t GetLeaf(unsigned int leaf, unsigned int subleaf)
{
    results_t results;
    asm
    (
        "cpuid"
        : "=a" (results.eax),
          "=b" (results.ebx),
          "=c" (results.ecx),
          "=d" (results.edx)
        : "a" (leaf),
          "c" (subleaf)
    );
    return results;
}


constexpr unsigned int TestFeature(feature_flag_t feature, results_t results)
{
    // Extract the relevant register.
    unsigned int value = 0;
    switch (feature.where)
    {
        case Register::EAX:
            value = results.eax;
            break;

        case Register::EBX:
            value = results.ebx;
            break;

        case Register::ECX:
            value = results.ecx;
            break;

        case Register::EDX:
            value = results.edx;
            break;

        default:
            __builtin_unreachable();
    }

    // And extract the appropriate bits from that register.
    value &= feature.mask;
    if (feature.shift > 0)
        value >>= feature.shift;
    else if (feature.shift < 0)
        value <<= -feature.shift;
    return value;
}

[[gnu::const]]
inline bool SupportsLeaf(unsigned int leaf)
{
    // Check that CPUID is supported.
    if (!SupportsCPUID())
        return false;
    
    // Is this a normal or extended leaf?
    if (leaf < 0x80000000)
    {
        // Test that the requested non-extended leaf is available.
        if (GetLeaf(0x00000000).eax < leaf)
            return false;
    }
    else
    {
        // Test that the requested extended leaf is available.
        if (GetLeaf(0x80000000).eax < leaf)
            return false;
    }

    // Leaf appears to be supported.
    return true;
}

[[gnu::const]]
inline unsigned int GetFeature(feature_flag_t feature)
{
    // Check that the leaf containing this feature is available.
    if (!SupportsLeaf(feature.leaf))
        return 0;

    // Get the value of the requested leaf and subleaf.
    auto results = feature.use_subleaf ? GetLeaf(feature.leaf, feature.subleaf) : GetLeaf(feature.leaf);

    return TestFeature(feature, results);
}

[[gnu::const]]
inline bool HasFeature(feature_flag_t feature)
{
    return GetFeature(feature) != 0;
}


} // namespace System::CPUID


#endif /* ifndef __SYSTEM_CPUID_X86_H */
