#ifndef __SYSTEM_C_STDLIB_STRTONUM_HH
#define __SYSTEM_C_STDLIB_STRTONUM_HH


#include <System/C/StdLib/errno.h>

#include <System/C/StdLib/ctype.h>
#include <System/C/StdLib/wctype.h>

#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>


namespace System::C::StdLib
{


template <class T, class CharT = char>
T StrToNum(const CharT* __restrict str, std::type_identity_t<CharT>** __restrict end, int base)
{
    constexpr bool Signed = std::is_signed_v<T>;
    constexpr auto Max = std::numeric_limits<T>::max();
    constexpr auto Min = std::numeric_limits<T>::min();

    auto begin = str;

    // Check that the base is valid.
    if (base < 2 or base > 36)
    {
        if (end)
            *end = const_cast<CharT*>(str);

        // Setting errno in this case is not specified by the C standard but it makes sense.
        errno = EDOM;
        return 0;
    }

    auto ValueOf = [base](CharT c) -> T
    {
        // This function is only correct for character sets that are ASCII supersets. As this library only supports
        // UTF-8, this is fine.
        if (isdigit(c))
            return static_cast<T>(c - '0');
        else if (isupper(c))
            return static_cast<T>(c - 'A' + 10);
        else if (islower(c))
            return static_cast<T>(c - 'a' + 10);
        else
            return static_cast<T>(base);
    };

    // Skip any leading whitespace in the input.
    auto IsSpace = [](CharT c)
    {
        if constexpr (std::is_same_v<CharT, char>)
            return isspace(c);
        else if constexpr (std::is_same_v<CharT, wchar_t>)
            return iswspace(static_cast<wint_t>(c));
        else
            return;
    };
    while (*str != '\0' and IsSpace(*str))
        ++str;

    // Read any leading sign character.
    bool negative = false;
    if (*str == '+' or *str == '-')
    {
        negative = (*str == '-');
        ++str;
    }

    // If the base is 16, we can skip a leading "0x" or "0X", if present.
    if (base == 16 and str[0] == '0' and (str[1] == 'x' or str[1] == 'X'))
        str += 2;

    // Attempt to autodetect the base if it hasn't been specified.
    if (base == 0)
    {
        if (*str == '0')
        {
            // We'll either detect octal or hexadecimal, depending on the next character.
            ++str;
            if (*str == 'x' || *str == 'X')
            {
                // Auto-detected hexadecimal.
                ++str;
                base = 16;
            }
            else
            {
                // Auto-detected octal.
                base = 8;
            }
        }
        else
        {
            // We'll just assume the number is decimal.
            base = 10;
        }
    }

    // Begin consuming the number.
    T val = 0;
    bool overflow = false;
    bool valid = false;
    while (*str != '\0')
    {
        // Convert this character to the corresponding numeric value.
        auto digit = ValueOf(*str);
        if (digit >= static_cast<T>(base))
        {
            // Not a valid digit in this base; we've reached the end of the number.
            break;
        }

        // We've read at least one valid digit.
        valid = true;

        // Will we overflow by adding another digit?
        if (Max / static_cast<T>(base) < val) [[unlikely]]
        {
            // We're going to overflow when we multiply by the base.
            overflow = true;

            // Continue to the end of the number so the end pointer is correct.
        }

        // Shift all the digits up a place.
        if (!overflow) [[likely]]
            val *= static_cast<T>(base);

        // Will adding this particular digit cause an overflow?
        if (Max - digit < val) [[unlikely]]
        {
            // We're going to overflow by adding this digit.
            overflow = true;

            // Continue to the end of the number so the end pointer is correct.
        }

        // Add the digit.
        if (!overflow) [[likely]]
            val += digit;
    }

    // If the input wasn't of the correct form, reject the whole string (including any signs and prefixes).
    if (!valid)
    {
        if (end)
            *end = const_cast<CharT*>(begin);

        // Not an error. Just a zero-length input.
        return 0;
    }

    // All returns beyond this point (including errors) will set the end pointer to the same value.
    if (end)
        *end = const_cast<CharT*>(str);

    // Did we overflow during conversion?
    if (overflow)
    {
        // The return value is determined by the sign of the value and signedness of the type.
        if (Signed)
            val = (negative) ? Min : Max;
        else
            val = Max;

        errno = ERANGE;
        return val;
    }

    // Number has been converted successfully. Now apply the sign.
    if constexpr (Signed)
    {
        // As we assume two's-complement, any representable positive value is also representable when negated (though
        // the converse is not true!) so we don't need to check the validity of this negation.
        if (negative)
            val = -val;
    }
    else
    {
        // The wording of the C standard indicates that it is an error if the converted value is not representable.
        // Unfortunately, this seems to imply that "-0" should convert successfully as an unsigned value...
        if (negative and val != 0)
        {
            errno = ERANGE;
            return Max;
        }
    }

    // All the checks have passed!
    return val;
}


} // namespace System::C::StdLib


#endif /* ifndef __SYSTEM_C_STDLIB_STRTONUM_HH */
