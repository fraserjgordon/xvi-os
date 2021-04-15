#include <System/C++/Locale/WChar.hh>

#include <System/Unicode/UTF/EncodeState.hh>


namespace __XVI_STD_LOCALE_NS
{


namespace UTF = System::Unicode::UTF;


int mbsinit(const mbstate_t * __restrict state)
{
    return (state == nullptr) || UTF::EncodeStateIsInitial(reinterpret_cast<const UTF::encode_state*>(state));
}

wint_t btowc(int c)
{
    if (0 <= c && c <= 0x7f)
        return static_cast<wint_t>(c);
    else
        return WEOF;
}

int wctob(wint_t wc)
{
    if (wc <= 0x7f)
        return static_cast<int>(wc);
    else
        return EOF;
}


} // namespace __XVI_STD_LOCALE_NS
