#ifndef __SYSTEM_TEXT_ENCODING_REGISTRY_HH
#define __SYSTEM_TEXT_ENCODING_REGISTRY_HH


#include <expected>
#include <memory>
#include <new>
#include <string_view>
#include <type_traits>

#include <System/Text/Encoding/Heap.hh>
#include <System/Text/Encoding/Types.hh>


namespace System::Text::Encoding
{


class ConversionRegistry
{
public:

    enum class options
    {
        none = 0,       // No special options.
        empty,
        defaults,
    };

    struct transcode_functions
    {

    };

    using open_result = std::expected<std::unique_ptr<ConversionRegistry>, open_status>;
    using enumerate_fn_mc_t = void (*)(void*, std::string_view, std::string_view, const transcode_functions&);
    using enumerate_fn_c8_t = void (*)(void*, std::u8string_view, std::u8string_view, const transcode_functions&);


    open_status add(std::string_view from_encoding, std::string_view to_encoding, const transcode_functions&);
    open_status add(std::u8string_view from_encoding, std::u8string_view to_encoding, const transcode_functions&);

    const conversion_heap* heap() const noexcept;

    void enumerate(void* user_data, enumerate_fn_mc_t enumfn) const;
    void enumerate(void* user_data, enumerate_fn_c8_t enumfn) const;

    template <class Fn>
        requires std::invocable<Fn, std::string_view, std::string_view, const transcode_functions&>
    void enumerate(Fn&& fn) const
    {
        auto wrapper = [](void* user_data, std::string_view from, std::string_view to, const transcode_functions& fns)
        {
            (*reinterpret_cast<Fn*>(user_data))(from, to, fns);
        };

        enumerate(std::addressof(fn), &wrapper);
    }

    template <class Fn>
        requires std::invocable<Fn, std::u8string_view, std::u8string_view, const transcode_functions&>
    void enumerate(Fn&& fn) const
    {
        auto wrapper = [](void* user_data, std::u8string_view from, std::u8string_view to, const transcode_functions& fns)
        {
            (*reinterpret_cast<Fn*>(user_data))(from, to, fns);
        };

        enumerate(std::addressof(fn), &wrapper);
    }


    static void operator delete(ConversionRegistry*, std::destroying_delete_t);

    static open_result create();
    static open_result create(options);
    static open_result create(conversion_heap*);
    static open_result create(conversion_heap*, options);

    static const ConversionRegistry* defaultRegistry() noexcept;

protected:

    constexpr ConversionRegistry() = default;
    ConversionRegistry(const ConversionRegistry&) = delete;
    ConversionRegistry(ConversionRegistry&&) = delete;

    ~ConversionRegistry() = default;

    void operator=(const ConversionRegistry&) = delete;
    void operator=(ConversionRegistry&&) = delete;
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_REGISTRY_HH */
