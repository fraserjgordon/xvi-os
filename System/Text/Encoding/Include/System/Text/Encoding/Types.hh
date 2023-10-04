#ifndef __SYSTEM_TEXT_ENCODING_TYPES_HH
#define __SYSTEM_TEXT_ENCODING_TYPES_HH


#include <cstddef>


namespace System::Text::Encoding
{


constexpr std::size_t MaxMCElements = 8;
constexpr std::size_t MaxMWCElements = 2;
constexpr std::size_t MaxC8Elements = 8;
constexpr std::size_t MaxC16Elements = 4;
constexpr std::size_t MaxC32Elements = 2;


enum class conversion_status
{
    ok,                 // No error.
    incomplete,         // Input is not a complete sequence.
    invalid,            // Input is not a valid sequence.
    output_too_small,   // Output buffer is not big enough.
};


enum class open_status
{
    ok,                 // No error.
    no_conversion_path, // Could not find a conversion path between encodings.
    output_too_small,   // Output buffer is not big enough.
    invalid_parameter,  // Invalid parameter.
    allocation_error,   // Error allocating memory.
};


//! @struct conversion_state
//!
//! @brief  Holds inter-character state for stateful encodings.
//!
//! @note   The purpose of this structure is to hold state that needs to be
//!         preserved between characters (e.g. shift states). It is not used to
//!         accumulate data for multi-codeunit encodings for UTF-x encodings
//!         (but may for other encoding implementations, if convenient).
//!
//! @warning    The fields of this structure are implementation details and do
//!             not have any defined semantics to users of this library. The
//!             meaning of the fields can change at any time.
//!
struct conversion_state
{
    using completion_fn_t = bool (*)(const conversion_state*);

    unsigned int    indicator = 0;
    unsigned int    assume_valid = 0;
    completion_fn_t completion_fn = nullptr;
    void*           data[3] = {};

    bool isComplete() const;
    void setAssumeValid(bool assume = true);
    bool isAssumingValid() const;
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_TYPES_HH */
