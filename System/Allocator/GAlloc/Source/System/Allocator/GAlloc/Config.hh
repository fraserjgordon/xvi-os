#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_CONFIG_H
#define __SYSTEM_ALLOCATOR_GALLOC_CONFIG_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Bit.hh>


#define __GALLOC_NS         System::Allocator::GAlloc
#define __GALLOC_ABI_TAG    __attribute__((abi_tag("galloc_v0")))
#define __GALLOC_NS_DECL    __GALLOC_NS::inline abi_v0


// Extra namespace declaration to apply the ABI tag.
namespace __GALLOC_NS
{
inline namespace abi_v0 __GALLOC_ABI_TAG {}
}


namespace __GALLOC_NS_DECL
{


// ---------- User settings ---------- //

constexpr bool ErrorChecking = true;

constexpr bool KernelMode = false;

constexpr std::size_t PageTableCollapseTopLevels = 0;

// The threshold where pools switch from being multiples of the size of a pointer to being spaced (approximately)
// logarithmically.
//
// This value is given in bytes and must be a multiple of sizeof(void*).
constexpr unsigned MaxSmallObjectSize = 256;

// The maximum size of block to manage using pools, expressed as a left-shift of the page size.
constexpr unsigned MaxPooledBlockSizeOrder = 0;

// The maximum allocation size to handle internally.
//
// Requests for memory with a size equal to or greater than this will be allocated directly via a system call rather
// than being managed as part of the heap.
//
// This value must be at least as large as MaxPooledBlockSizeOrder.
constexpr unsigned DirectThresholdOrder = 7;

// The maximum number of pages to allow in a single pool.
//
// The largest pool size that the allocator will consider when creating a pool is 2**MaxPoolOrder * PageSize. If this is
// set too low (currently, less than 2) the allocator may not be able to meet the overhead guarantees.
constexpr unsigned MaxPoolOrder = MaxPooledBlockSizeOrder + 4;


// ---------- Derived settings ---------- //

// The settings below this point should not be edited directly.


constexpr std::size_t PageOrder = 12;
constexpr std::size_t PageSize = 1 << PageOrder;

constexpr std::size_t PointerSize = sizeof(std::uintptr_t);
constexpr std::size_t PointerBits = PointerSize * 8;
constexpr std::size_t PointersPerPage = PageSize / PointerSize;
constexpr std::size_t AddressBitsPerPage = std::bit_width(PointersPerPage) - 1;

constexpr std::size_t LargePageOrder(int order)
    { return PageOrder + order * AddressBitsPerPage; }
constexpr std::size_t LargePageSize(int order)
    { return 1 << LargePageOrder(order); }


}


#endif // ifndef __SYSTEM_ALLOCATOR_GALLOC_CONFIG_H
