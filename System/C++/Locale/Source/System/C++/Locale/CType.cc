#include <System/C++/Locale/CType.hh>

#include <System/C++/LanguageSupport/StdInt.hh>


namespace __XVI_STD_LOCALE_NS
{


namespace
{


using std::uint8_t;


constexpr uint8_t CTypeLower    = 0x01;
constexpr uint8_t CTypeUpper    = 0x02;
constexpr uint8_t CTypeBlank    = 0x04;
constexpr uint8_t CTypeDigit    = 0x08;
constexpr uint8_t CTypePrint    = 0x10;
constexpr uint8_t CTypeSpace    = 0x20;
constexpr uint8_t CTypeXDigit   = 0x40;
constexpr uint8_t CTypeCntrl    = 0x80;

constexpr uint8_t CCControl     = CTypeCntrl;
constexpr uint8_t CCSpace       = CTypeBlank|CTypeSpace|CTypePrint;
constexpr uint8_t CCTab         = CTypeCntrl|CTypeBlank|CTypeSpace;
constexpr uint8_t CCFeed        = CTypeCntrl|CTypeSpace;
constexpr uint8_t CCPunct       = CTypePrint;
constexpr uint8_t CCDigit       = CTypeDigit|CTypeXDigit|CTypePrint;
constexpr uint8_t CCLowerX      = CTypeLower|CTypeXDigit|CTypePrint;
constexpr uint8_t CCUpperX      = CTypeUpper|CTypeXDigit|CTypePrint;
constexpr uint8_t CCLower       = CTypeLower|CTypePrint;
constexpr uint8_t CCUpper       = CTypeUpper|CTypePrint;

constexpr uint8_t AlnumAny      = CTypeLower|CTypeUpper|CTypeDigit;
constexpr uint8_t AlphaAny      = CTypeLower|CTypeUpper;
constexpr uint8_t BlankAny      = CTypeBlank;
constexpr uint8_t CntrlAny      = CTypeCntrl;
constexpr uint8_t DigitAny      = CTypeDigit;
constexpr uint8_t GraphAny      = CTypePrint;
constexpr uint8_t GraphNone     = CTypeSpace|CTypeBlank;
constexpr uint8_t LowerAny      = CTypeLower;
constexpr uint8_t PrintAny      = CTypePrint;
constexpr uint8_t PunctAny      = CTypePrint;
constexpr uint8_t PunctNone     = CTypeLower|CTypeUpper|CTypeDigit|CTypeBlank|CTypeSpace;
constexpr uint8_t SpaceAny      = CTypeSpace;
constexpr uint8_t UpperAny      = CTypeUpper;
constexpr uint8_t XDigitAny     = CTypeXDigit;


constexpr uint8_t CTypes[128] =
{
    /* 0x00 */ CCControl, CCControl, CCControl, CCControl, CCControl, CCControl, CCControl, CCControl,
    /* 0x08 */ CCControl, CCTab,     CCFeed,    CCFeed,    CCFeed,    CCFeed,    CCControl, CCControl,
    /* 0x10 */ CCControl, CCControl, CCControl, CCControl, CCControl, CCControl, CCControl, CCControl,
    /* 0x18 */ CCControl, CCControl, CCControl, CCControl, CCControl, CCControl, CCControl, CCControl,
    /* 0x20 */ CCSpace,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,
    /* 0x28 */ CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,
    /* 0x30 */ CCDigit,   CCDigit,   CCDigit,   CCDigit,   CCDigit,   CCDigit,   CCDigit,   CCDigit,
    /* 0x38 */ CCDigit,   CCDigit,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,
    /* 0x40 */ CCPunct,   CCUpperX,  CCUpperX,  CCUpperX,  CCUpperX,  CCUpperX,  CCUpperX,  CCUpper,
    /* 0x48 */ CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,
    /* 0x50 */ CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,   CCUpper,
    /* 0x58 */ CCUpper,   CCUpper,   CCUpper,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCPunct,
    /* 0x60 */ CCPunct,   CCLowerX,  CCLowerX,  CCLowerX,  CCLowerX,  CCLowerX,  CCLowerX,  CCLower,
    /* 0x68 */ CCLower,   CCLower,   CCLower,   CCLower,   CCLower,   CCLower,   CCLower,   CCLower,
    /* 0x70 */ CCLower,   CCLower,   CCLower,   CCLower,   CCLower,   CCLower,   CCLower,   CCLower,
    /* 0x78 */ CCLower,   CCLower,   CCLower,   CCPunct,   CCPunct,   CCPunct,   CCPunct,   CCControl,
};


static inline constexpr int is(int c, uint8_t any, uint8_t none = 0)
{
    if (c < 0 or c > 127)
        return 0;

    if (none == 0)
        return CTypes[c] & any;

    auto types = CTypes[c];
    if ((types & none) != 0)
        return 0;

    return (types & any);
}


} // namespace <anonymous>


int isalnum(int c)
{
    return is(c, AlnumAny);
}

int isalpha(int c)
{
    return is(c, AlphaAny);
}

int isblank(int c)
{
    return is(c, BlankAny);
}

int iscntrl(int c)
{
    return is(c, CntrlAny);
}

int isdigit(int c)
{
    return is(c, DigitAny);
}

int isgraph(int c)
{
    return is(c, GraphAny, GraphNone);
}

int islower(int c)
{
    return is(c, LowerAny);
}

int isprint(int c)
{
    return is(c, PrintAny);
}

int ispunct(int c)
{
    return is(c, PunctAny, PunctNone);
}

int isspace(int c)
{
    return is(c, SpaceAny);
}

int isupper(int c)
{
    return is(c, UpperAny);
}

int isxdigit(int c)
{
    return is(c, XDigitAny);
}

int tolower(int c)
{
    int mask = 0x20 & ((!is(c, UpperAny)) - 1);
    return c ^ mask;
}

int toupper(int c)
{
    int mask = 0x20 & ((!is(c, LowerAny)) - 1);
    return c ^ mask;
}


} // namespace __XVI_STD_LOCALE_NS
