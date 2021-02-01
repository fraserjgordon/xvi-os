#include <System/Text/Encoding/InfoTable.hh>

#include <System/Text/Encoding/UTF-8/UTF-8.hh>
#include <System/Text/Encoding/UTF-16/UTF-16.hh>
#include <System/Text/Encoding/UTF-32/UTF-32.hh>

#include <algorithm>
#include <array>


namespace System::Text::Encoding
{


constexpr std::initializer_list<EncodingInfo> EncodingInfoTableRaw =
{
#if __SYSTEM_TEXT_ENCODING_ENABLE_UTF8
    EncodingInfoUTF8,
    EncodingInfoCESU8,
    EncodingInfoModifiedUTF8,
    EncodingInfoWTF8,
#endif
#if __SYSTEM_TEXT_ENCODING_ENABLE_UTF16
    EncodingInfoUTF16,
    EncodingInfoUTF16LE,
    EncodingInfoUTF16BE,
    EncodingInfoUCS2,
#endif
#if __SYSTEM_TEXT_ENCODING_ENABLE_UTF32
    EncodingInfoUTF32,
    EncodingInfoUTF32LE,
    EncodingInfoUTF32BE,
    EncodingInfoUCS4,
#endif
};


// Comparator for encoding names. It assumes the input consists of [a-zA-Z0-9] plus dash and underscore. Comparisons
// ignore case and any characters other than a-z0-9.
constexpr int encodingNameCompare(std::string_view a, std::string_view b)
{
    auto casefold = [](char c) -> int
    {
        if (c >= 'A' && c <= 'Z')
            return c - 'A';
        return c;
    };

    auto skip = [](char c) -> bool
    {
        if ('0' <= c && c <= '9')
            return false;
        else if ('a' <= c && c <= 'z')
            return false;
        else
            return true;
    };
    
    auto i = a.begin();
    auto j = b.begin();
    while (i != a.end() && j != b.end())
    {
        auto ci = casefold(*i);
        auto cj = casefold(*j);
        
        if (skip(ci))
        {
            ++i;
            continue;
        }

        if (skip(cj))
        {
            ++j;
            continue;
        }

        if (ci != cj)
            return cj - ci;

        ++i;
        ++j;
    }

    return (i == a.end()) ? ((j == b.end()) ? 0 : -1)
                          : ((j == b.end()) ? 1 :  0);
}


// Encoding info table as an array.
constexpr auto EncodingInfoTable = []()
{
    constexpr auto Count = EncodingInfoTableRaw.size();
    using Array = std::array<EncodingInfo, Count>;

    Array arr = {};
    std::copy_n(EncodingInfoTableRaw.begin(), Count, arr.begin());
    return arr;
}();


// Encoding info table sorted by encoding ID.
constexpr auto EncodingInfoByID = []()
{
    auto arr = EncodingInfoTable;
    std::sort(arr.begin(), arr.end(), [](const EncodingInfo& a, const EncodingInfo& b)
        -> bool
    {
        return a.id < b.id;
    });

    return arr;
}();


// Encoding info table sorted by encoding name.
constexpr auto EncodingInfoByName = []()
{
    constexpr std::size_t Count = []()
    {
        std::size_t n = 0;
        for (const auto& e : EncodingInfoTable)
            n += e.names.size();
        return n;
    }();

    using Pair = std::pair<const EncodingInfo*, std::string_view>;
    using Array = std::array<Pair, Count>;

    Array arr = {};
    std::size_t index = 0;
    for (const auto& e : EncodingInfoTable)
        for (const char* p : e.names)
            arr[index++] = {&e, p};

    std::sort(arr.begin(), arr.end(), [](const Pair& a, const Pair& b)
    {
        return encodingNameCompare(a.second, b.second) < 0;
    });

    return arr;
}();


const EncodingInfo* getEncodingById(EncodingID id)
{
    auto iter = std::lower_bound(EncodingInfoTable.begin(), EncodingInfoTable.end(), id, [](const EncodingInfo& a, EncodingID b)
    {
        return a.id < b;
    });

    return (iter == EncodingInfoTable.end()) ? nullptr : iter;
}

const EncodingInfo* getEncodingByName(std::string_view name)
{
    auto iter = std::lower_bound(EncodingInfoByName.begin(), EncodingInfoByName.end(), name, [](const auto& p, std::string_view sv)
    {
        return encodingNameCompare(p.second, sv) < 0;
    });

    return (iter == EncodingInfoByName.end()) ? nullptr : iter->first;
}


} // namespace System::Text::Encoding
