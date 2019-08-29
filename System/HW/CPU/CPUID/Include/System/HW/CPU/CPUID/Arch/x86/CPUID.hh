#pragma once
#ifndef __SYSTEM_HW_CPU_CPUID_ARCH_X86_CPUID_H
#define __SYSTEM_HW_CPU_CPUID_ARCH_X86_CPUID_H


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


static constexpr results_t VendorID(const char *ID)
{   
    unsigned int first = ID[0] | (ID[1] << 8) | (ID[2] << 16) | (ID[3] << 24);
    unsigned int second = ID[4] | (ID[5] << 8) | (ID[6] << 16) | (ID[7] << 24);
    unsigned int third = ID[8] | (ID[9] << 8) | (ID[10] << 16) | (ID[11] << 24);
    return { .eax = 0, .ebx = first, .ecx = third, .edx = second };
};

static constexpr bool VendorMatches(results_t a, results_t b)
{
    // EAX is ignored.
    return a.ebx == b.ebx && a.ecx == b.ecx && a.edx == b.edx;
}

static constexpr Vendor DecodeVendor(results_t r)
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
static_assert(DecodeVendor(VendorID("AuthenticAMD")) == Vendor::AMD);
static_assert(DecodeVendor(VendorID("GenuineIntel")) == Vendor::Intel);


// CPUID leaves.
static constexpr unsigned int LeafManufacturer      = 0;    //!< Basic CPU identification.
static constexpr unsigned int LeafModelAndFeatures  = 1;
static constexpr unsigned int LeafCacheInfo         = 2;
static constexpr unsigned int LeafSerialNumber      = 3;
static constexpr unsigned int LeafExtendedFeatures  = 7;
static constexpr unsigned int LeafXsaveInfo         = 13;

static constexpr unsigned int XLeafManufacturer     = 0x80000000;
static constexpr unsigned int XLeafModelAndFeatures = 0x80000001;
static constexpr unsigned int XLeafBrandString1     = 0x80000002;
static constexpr unsigned int XLeafBrandString2     = 0x80000003;
static constexpr unsigned int XLeafBrandString3     = 0x80000004;


namespace Feature
{

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
static constexpr feature_flag_t PackageAcpiIdCount = {.leaf = 1, .where = Register::EBX, .mask = 0x00ff0000, .shift = 16};
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

} // namespace Feature


#if defined(__amd64__)
static constexpr bool SupportsCPUID()
{
    return true;
}
#else
[[gnu::const]]
static inline bool SupportsCPUID()
{
    // See if the ID flag (bit 17) of the %eflags register can be modified.
    unsigned int result;
    asm
    (
        "pushfl     \n\t"
        "xorl       $(1<<17), (%%esp)\n\t"
        "popfl      \n\t"
        "movl       4(%esp), %[result]\n\t"
        "pushfl     \n\t"
        "xorl       (%%esp), %[result]\n\t"
        "addl       $4, %%esp\n\t"
        : [result] "=r" (result)
    );
    return (result & 0x00020000);
}
#endif


[[gnu::const]]
static inline results_t GetLeaf(unsigned int leaf)
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
static inline results_t GetLeaf(unsigned int leaf, unsigned int subleaf)
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


static constexpr unsigned int TestFeature(feature_flag_t feature, results_t results)
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
static inline bool SupportsLeaf(unsigned int leaf)
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
static inline unsigned int GetFeature(feature_flag_t feature)
{
    // Check that the leaf containing this feature is available.
    if (!SupportsLeaf(feature.leaf))
        return 0;

    // Get the value of the requested leaf and subleaf.
    auto results = feature.use_subleaf ? GetLeaf(feature.leaf, feature.subleaf) : GetLeaf(feature.leaf);

    return TestFeature(feature, results);
}

[[gnu::const]]
static inline bool HasFeature(feature_flag_t feature)
{
    return GetFeature(feature) != 0;
}


} // namespace System::CPUID


#endif /* ifndef __SYSTEM_CPUID_X86_H */
