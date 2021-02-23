#ifndef __SYSTEM_C_STDLIB_LOCALE_H
#define __SYSTEM_C_STDLIB_LOCALE_H


#include <System/C/Private/Config.h>

#include <System/C/BaseHeaders/Null.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define LC_COLLATE      0x0001
#define LC_CTYPE        0x0002
#define LC_MONETARY     0x0004
#define LC_NUMERIC      0x0008
#define LC_TIME         0x0010

#define LC_ALL          0


struct lconv
{
    char* decimal_point;
    char* thousands_sep;
    char* grouping;
    char* mon_decimal_point;
    char* mon_thousands_sep;
    char* mon_grouping;
    char* positive_sign;
    char* negative_sign;
    char* currency_symbol;
    char* int_currency_symbol;

    char frac_digits;
    char p_cs_precedes;
    char n_cs_precedes;
    char p_sep_by_space;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
    char int_frac_digits;
    char int_p_cs_precedes;
    char int_n_cs_precedes;
    char int_p_sep_by_space;
    char int_n_sep_by_space;
    char int_p_sign_posn;
    char int_n_sign_posn;
};


__SYSTEM_C_STDLIB_EXPORT char* setlocale(int, const char*);

__SYSTEM_C_STDLIB_EXPORT struct lconv* localeconv(void);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_LOCALE_H */
