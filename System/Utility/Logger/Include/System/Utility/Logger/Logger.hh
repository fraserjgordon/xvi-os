#ifndef __SYSTEM_UTILITY_LOGGER_LOGGER_H
#define __SYSTEM_UTILITY_LOGGER_LOGGER_H


#include <System/Utility/Logger/Private/Config.hh>

#if defined(__XVI_NO_STDLIB)
#  include <System/C++/LanguageSupport/InitializerList.hh>
#  include <System/C++/LanguageSupport/Limits.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#  include <System/C++/Utility/Array.hh>
#  include <System/C++/Utility/String.hh>
#  include <System/C++/Utility/StringView.hh>
#else
#  include <array>
#  include <cstdint>
#  include <initializer_list>
#  include <limits>
#  include <string>
#  include <string_view>
#  include <utility>
#endif

#if defined(__SYSTEM_UTILITY_LOGGER_USE_LIBFMT)
#  include <fmt/format.h>
#else
#  if defined(__XVI_NO_STDLIB)
#    include <System/C++/Format/Format.hh>
#  else
#    include <format>
#  endif
#endif


namespace System::Utility::Logger
{


#if defined(__SYSTEM_UTILITY_LOGGER_USE_LIBFMT)
namespace fmt = ::fmt;
#else
namespace fmt = std;
#endif


enum class facility : std::uint32_t {};

inline constexpr facility DefaultFacility = facility{0};

enum class priority : std::int32_t
{
    all         = std::numeric_limits<std::int32_t>::min(),
    
    trace       = -3000,
    debug       = -2000,
    verbose     = -1000,
    info        = 0,
    notice      = 1000,
    warning     = 2000,
    error       = 3000,
    critical    = 4000,
    alert       = 5000,
    emergency   = 6000,

    none        = std::numeric_limits<std::int32_t>::max(),
};

enum class var_type : std::uint32_t
{
    string              = 0,
    signed_integer      = 1,
    unsigned_integer    = 2,
    binary              = 3,
};

constexpr auto operator<=>(const priority& x, const priority& y) noexcept
{
    return static_cast<std::int32_t>(x) <=> static_cast<std::int32_t>(y);
}


using log_variable_fmt = std::pair<std::string_view, std::string_view>;
using log_variable_raw = std::pair<std::pair<const char*, std::size_t>, std::pair<const char*, std::size_t>>;


__SYSTEM_UTILITY_LOGGER_EXPORT
facility registerFacility(const char* name, std::size_t name_len) __SYSTEM_UTILITY_LOGGER_SYMBOL(RegisterFacility);

__SYSTEM_UTILITY_LOGGER_EXPORT
void setMinimumPriority(priority) __SYSTEM_UTILITY_LOGGER_SYMBOL(SetMinimumPriority);

__SYSTEM_UTILITY_LOGGER_EXPORT
void setFacilityMinimumPriority(facility, priority) __SYSTEM_UTILITY_LOGGER_SYMBOL(SetFacilityMinimumPriority);

__SYSTEM_UTILITY_LOGGER_EXPORT
priority minimumLogPriority(facility) __SYSTEM_UTILITY_LOGGER_SYMBOL(MinimumLogPriority);

__SYSTEM_UTILITY_LOGGER_EXPORT
void log(facility, priority, const char* msg, std::size_t msg_len, const log_variable_raw* vars, std::size_t nvars) __SYSTEM_UTILITY_LOGGER_SYMBOL(Log);

__SYSTEM_UTILITY_LOGGER_EXPORT
void enableLogToStderr(bool = true);

__SYSTEM_UTILITY_LOGGER_EXPORT
std::string_view priorityString(priority);


namespace detail
{

// Note: these functions make heavy use of the fact that temporaries created during evaluation of function call
//       arguments don't get destroyed until after the function call completes.

template <class... RawPairs>
    requires (std::is_same_v<RawPairs, log_variable_raw> && ...)
void doLogRawPairs(facility f, priority p, std::string_view msg, RawPairs&&... var_pairs)
{
    using list = std::initializer_list<log_variable_raw>;

    log(f, p, msg.data(), msg.length(), list{std::forward<RawPairs>(var_pairs)...}.begin(), sizeof...(RawPairs));
}

template <class... Pairs>
    requires (std::is_same_v<Pairs, std::pair<std::string_view, std::string_view>> && ...)
void doLogPairs(facility f, priority p, std::string_view msg, Pairs&&... var_pairs)
{
    doLogRawPairs(f, p, msg, log_variable_raw{{var_pairs.first.data(), var_pairs.first.length()}, {var_pairs.second.data(), var_pairs.second.length()}}...);
}

template <class... Vars>
    requires (std::is_same_v<Vars, log_variable_fmt> && ...)
void doLog(facility f, priority p, std::string_view msg_fmt, fmt::format_args args, Vars&&... vars)
{
    using sv = std::string_view;
    using pair = std::pair<sv, sv>;

    auto msg = fmt::vformat(msg_fmt, args);

    doLogPairs(f, p, msg, pair{vars.first, fmt::vformat(vars.second, args)}...);
}

} // namespace detail

template <std::size_t N, class... Args>
void log(facility f, priority p, std::string_view msg_fmt, const log_variable_fmt (&var_fmts)[N], const Args&... args)
{
    if (minimumLogPriority(f) > p)
        return;

    auto makeLogFn = [&f, &p, &msg_fmt](auto&&... fmt)
    {
        return [&f, &p, &msg_fmt, &fmt...](fmt::format_args fargs)
        {
           detail::doLog(f, p, msg_fmt, fargs, std::forward<decltype(fmt)>(fmt)...);
        };
    };

    std::apply(makeLogFn, std::to_array(var_fmts))(fmt::make_format_args(args...));
}

template <class... Args>
void log(facility f, priority p, std::string_view msg_fmt, const Args&... args)
{
    if (minimumLogPriority(f) > p)
        return;

    auto logFn = [&f, &p, &msg_fmt](fmt::format_args fargs)
    {
        detail::doLog(f, p, msg_fmt, fargs);
    };

    logFn(fmt::make_format_args(args...));
}


class logger
{
public:

    constexpr logger() = default;

    constexpr logger(facility f, priority p = priority::all) :
        m_facility(f),
        m_minPriority(p)
    {
    }

    logger(std::string_view facility_name, priority p = priority::all) :
        m_facility(registerFacility(facility_name.data(), facility_name.length())),
        m_minPriority(p)
    {
    }

    constexpr void setMinimumPriority(priority p)
    {
        m_minPriority = p;
    }

    template <std::size_t N, class... Args>
    void log(priority p, std::string_view msg_fmt, const log_variable_fmt (&var_fmts)[N], const Args&... args)
    {
        if (p >= m_minPriority)
            Logger::log(m_facility, p, msg_fmt, var_fmts, args...);
    }

    template <class... Args>
    void log(priority p, std::string_view msg_fmt, const Args&... args)
    {
        if (p >= m_minPriority)
            Logger::log(m_facility, p, msg_fmt, args...);
    }

private:

    facility    m_facility;
    priority    m_minPriority   = priority::all;
};


} // namespace System::Utility::Logger


#endif /* ifndef __SYSTEM_UTILITY_LOGGER_LOGGER_H */
