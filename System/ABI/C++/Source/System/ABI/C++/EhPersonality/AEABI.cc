#include <System/ABI/C++/Private/Config.hh>

#include <System/ABI/C++/TypeInfo.hh>
#include <System/ABI/C++/Unwinder.hh>

// Forward declarations of the three ARM EHABI personality routines.
extern "C" __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT _Unwind_Reason_Code __aeabi_unwind_cpp_pr0(_Unwind_State, _Unwind_Control_Block*, _Unwind_Context*);
extern "C" __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT _Unwind_Reason_Code __aeabi_unwind_cpp_pr1(_Unwind_State, _Unwind_Control_Block*, _Unwind_Context*);
extern "C" __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT _Unwind_Reason_Code __aeabi_unwind_cpp_pr2(_Unwind_State, _Unwind_Control_Block*, _Unwind_Context*);


// Scope types. The enum values match the encodings of these types in the unwind tables.
enum class ScopeType
{
    Cleanup         = 0b00,
    Catch           = 0b01,
    ExceptionSpec   = 0b10,
    Invalid         = 0b11,
};

// Catch types.
enum class CatchType
{
    NonReference,       // Non-reference catch.
    Reference,          // Catch by reference.
    Any,                // catch(...)
    None,               // Triggers an immediate unwind failure.
};

// Cleanup descriptor.
struct CleanupScope
{
    std::uint32_t           landingPad;     // Absolute address of cleanup landingpad.
};

// Catch descriptor.
struct CatchScope
{
    CatchType               catchType;      // Sub-type of this catch.
    std::uint32_t           landingPad;     // Absolute address of catch landingpad.
    const std::type_info*   typeInfo;       // Pointer to the type info for the caught type.
};

// Exception specification descriptor.
struct ExceptionSpecScope
{
    std::uint32_t           typeListSize;   // Size of the list of type_info pointers.
    const std::type_info**  typeList;       // List of type info pointers for the permitted exception types.
    std::uint32_t           landingpad;     // Optional landingpad for when exception doesn't match the type list.
};

// Union of all exception scope types.
struct Scope
{
    ScopeType           type;
    std::uint32_t       offset;
    std::uint32_t       length;
    union
    {
        CleanupScope        cleanupScope;
        CatchScope          catchScope;
        ExceptionSpecScope  exceptionSpecScope;
    };
};


// Decodes a "prel31" offset.
/*static inline std::uint32_t decodePrel31(const std::uint32_t* ptr)
{

}

// Parses a scope descriptor, either as a 16-bit or 32-bit descriptor (depending on personality routine).
template <class ScopeT>
static Scope parseScopeDescriptor(const ScopeT*& ptr)
{

}*/
