#pragma once
#ifndef __SYSTEM_CXX_UTILITY_CHRONO_H
#define __SYSTEM_CXX_UTILITY_CHRONO_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Abs.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/Ratio.hh>
#include <System/C++/Utility/StringFwd.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace chrono
{

// Forward declarations.
template <class _Rep, class _Period = ratio<1>> class duration;
template <class _Clock, class _Duration = typename _Clock::duration> class time_point;

namespace __detail
{

template <class> struct __is_duration : false_type {};
template <class _Rep, class _Period> struct __is_duration<duration<_Rep, _Period>> : true_type {};

template <class _T> inline constexpr bool __is_duration_v = __is_duration<_T>::value;

}

} // namespace chrono


template <class _Rep1, class _Period1, class _Rep2, class _Period2>
struct common_type<chrono::duration<_Rep1, _Period1>, chrono::duration<_Rep2, _Period2>>
{
    using type = chrono::duration<common_type_t<_Rep1, _Rep2>,
                                  ratio<gcd(_Period1::num, _Period2::num),
                                        lcm(_Period1::den, _Period2::den)>>;
};

template <class _Clock, class _Duration1, class _Duration2>
struct common_type<chrono::time_point<_Clock, _Duration1>, chrono::time_point<_Clock, _Duration2>>
{
    using type = chrono::time_point<_Clock, common_type_t<_Duration1, _Duration2>>;
};


namespace chrono
{

template <class _Rep> struct treat_as_floating_point
    : is_floating_point<_Rep> {};

template <class _Rep> inline constexpr bool treat_as_floating_point_v = treat_as_floating_point<_Rep>::value;


template <class _Rep>
struct duration_values
{
    static constexpr _Rep zero() noexcept
    {
        return _Rep(0);
    }

    static constexpr _Rep min() noexcept
    {
        return numeric_limits<_Rep>::lowest();
    }
    
    static constexpr _Rep max() noexcept
    {
        return numeric_limits<_Rep>::max();
    }
};


template <class _T>
concept bool _Cpp17Clock =
    requires
    {
        typename _T::rep;
        typename _T::period;
        typename _T::duration;
        typename _T::time_point;
        { _T::is_steady };
        { _T::now() };
    };

template <class _T> struct is_clock
    : false_type {};

template <class _T>
    requires _Cpp17Clock<_T>
struct is_clock<_T>
    : true_type {};

template <class _T> inline constexpr bool is_clock_v = is_clock<_T>::value;


//! TODO: remove from overload resolution if _To isn't a specialisation of duration.
template <class _To, class _Rep, class _Period> constexpr _To duration_cast(const duration<_Rep, _Period>& __d);


template <class _Rep, class _Period>
class duration
{
public:

    using rep       = _Rep;
    using period    = typename _Period::type;

    constexpr duration() = default;

    template <class _Rep2,
              class = enable_if_t<is_assignable_v<_Rep&, _Rep2>
                                 && (treat_as_floating_point_v<_Rep> == true
                                     || treat_as_floating_point_v<_Rep2> == false), void>>
    constexpr explicit duration(const _Rep2& __r)
        : _M_rep(static_cast<rep>(__r))
    {
    }

    template <class _Rep2, class _Period2,
              class = enable_if_t<treat_as_floating_point_v<rep> == true
                                  || (ratio_divide<_Period2, period>::den == 1
                                      && treat_as_floating_point_v<_Rep2> == false), void>>
    constexpr duration(const duration<_Rep2, _Period2>& __d)
        : _M_rep(duration_cast<duration>(__d).count())
    {
    }

    ~duration() = default;

    duration(const duration&) = default;
    duration& operator=(const duration&) = default;

    constexpr rep count() const
    {
        return _M_rep;
    }

    constexpr common_type_t<duration> operator+() const
    {
        return common_type_t<duration>(*this);
    }

    constexpr common_type_t<duration> operator-() const
    {
        return common_type_t<duration>(-_M_rep);
    }

    constexpr duration& operator++()
    {
        ++_M_rep;
        return *this;
    }

    constexpr duration operator++(int)
    {
        return duration(_M_rep++);
    }

    constexpr duration& operator--()
    {
        --_M_rep;
        return *this;
    }

    constexpr duration operator--(int)
    {
        return duration(_M_rep--);
    }

    constexpr duration& operator+=(const duration& __d)
    {
        _M_rep += __d.count();
        return *this;
    }

    constexpr duration& operator-=(const duration& __d)
    {
        _M_rep -= __d.count();
        return *this;
    }

    constexpr duration& operator*=(const rep& __rhs)
    {
        _M_rep *= __rhs;
        return *this;
    }

    constexpr duration& operator/=(const rep& __rhs)
    {
        _M_rep /= __rhs;
        return *this;
    }

    constexpr duration& operator%=(const rep& __rhs)
    {
        _M_rep %= __rhs;
        return *this;
    }

    constexpr duration& operator%=(const duration& __rhs)
    {
        _M_rep %= __rhs.count();
        return *this;
    }

    static constexpr duration zero() noexcept
    {
        return duration(duration_values<_Rep>::zero());
    }

    static constexpr duration min() noexcept
    {
        return duration(duration_values<_Rep>::min());
    }

    static constexpr duration max() noexcept
    {
        return duration(duration_values<_Rep>::max());
    }

private:

    _Rep _M_rep;
};


template <class _Rep1, class _Period1, class _Rep2, class _Period2,
          class _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>>
constexpr _CD operator+(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return _CD(_CD(__lhs).count() + _CD(__rhs).count());
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2,
          class _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>>
constexpr _CD operator-(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return _CD(_CD(__lhs).count() - _CD(__rhs).count());
}

template <class _Rep1, class _Period, class _Rep2,
          class _CD = duration<common_type_t<_Rep1, _Rep2>, _Period>>
constexpr _CD operator*(const duration<_Rep1, _Period>& __d, const _Rep2& __s)
{
    return _CD(_CD(__d).count() * __s);
}

template <class _Rep1, class _Rep2, class _Period,
          class _CD = duration<common_type_t<_Rep1, _Rep2>, _Period>>
constexpr _CD operator*(const _Rep1& __s, const duration<_Rep2, _Period>& __d)
{
    return __d * __s;
}

template <class _Rep1, class _Period, class _Rep2,
          class _CD = duration<common_type_t<_Rep1, _Rep2>, _Period>>
constexpr _CD operator/(const duration<_Rep1, _Period>& __d, const _Rep2& __s)
{
    return _CD(_CD(__d).count() / __s);
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2>
constexpr common_type_t<_Rep1, _Rep2> operator/(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    using _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
    return _CD(__lhs).count() / _CD(__rhs).count();
}

template <class _Rep1, class _Period, class _Rep2,
          class _CD = duration<common_type_t<_Rep1, _Rep2>, _Period>>
constexpr _CD operator%=(const duration<_Rep1, _Period>& __d, const _Rep2& __s)
{
    return _CD(_CD(__d).count() % __s);
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2,
          class _CD = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>>
constexpr _CD operator%(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return _CD(_CD(__lhs).count() % _CD(__rhs).count());
}


template <class _Rep1, class _Period1, class _Rep2, class _Period2>
constexpr bool operator==(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    using _CT = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
    return _CT(__lhs).count() == _CT(__rhs).count();
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2>
constexpr bool operator!=(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return !(__lhs == __rhs);
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2>
constexpr bool operator<(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    using _CT = common_type_t<duration<_Rep1, _Period1>, duration<_Rep2, _Period2>>;
    return _CT(__lhs).count() < _CT(__rhs).count();
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2>
constexpr bool operator>(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return (__rhs < __lhs);
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2>
constexpr bool operator<=(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return !(__rhs < __lhs);
}

template <class _Rep1, class _Period1, class _Rep2, class _Period2>
constexpr bool operator>=(const duration<_Rep1, _Period1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return !(__lhs < __rhs);
}


template <class _To, class _Rep, class _Period>
constexpr _To duration_cast(const duration<_Rep, _Period>& __d)
{
    using _CF = ratio_divide<_Period, typename _To::period>;
    using _CR = common_type<typename _To::rep, _Rep, intmax_t>;

    if constexpr (_CF::num == 1 && _CF::den == 1)
        return _To(static_cast<typename _To::rep>(__d.count()));
    else if constexpr (_CF::num != 1 && _CF::den == 1)
        return _To(static_cast<typename _To::rep>(static_cast<_CR>(__d.count())
                                                  * static_cast<_CR>(_CF::num)));
    else if constexpr (_CF::num == 1 && _CF::den != 1)
        return _To(static_cast<typename _To::rep>(static_cast<_CR>(__d.count())
                                                  / static_cast<_CR>(_CF::den)));
    else
        return _To(static_cast<typename _To::rep>(static_cast<_CR>(__d.count())
                                                  * static_cast<_CR>(_CF::num)
                                                  / static_cast<_CR>(_CF::den)));
}

//! @TODO: floor, ceil, round for durations

using nanoseconds   = duration<int64_t, nano>;
using microseconds  = duration<int64_t, micro>;
using milliseconds  = duration<int64_t, milli>;
using seconds       = duration<int64_t>;
using minutes       = duration<int32_t, ratio<  60>>;
using hours         = duration<int32_t, ratio<3600>>;
using days          = duration<int32_t, ratio_multiply<ratio<24>, hours::period>>;
using weeks         = duration<int32_t, ratio_multiply<ratio<7>, days::period>>;
using years         = duration<int32_t, ratio_multiply<ratio<146097, 400>, days::period>>;
using months        = duration<int32_t, ratio_divide<years::period, ratio<12>>>;


template <class _Rep, class _Period>
constexpr duration<_Rep, _Period> abs(duration<_Rep, _Period> __d)
{
    return (__d >= __d.zero()) ? __d : -__d;
}

//! @TODO: implement.
template <class _CharT, class _Traits, class _Rep, class _Period>
basic_ostream<_CharT, _Traits>&
operator<<(basic_ostream<_CharT, _Traits>& __os, const duration<_Rep, _Period>& __d);

//! @TODO: implement.
template <class _CharT, class _Traits, class _Rep, class _Period>
basic_ostream<_CharT, _Traits>&
to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const duration<_Rep, _Period>& __d);

//! @TODO: implement.
template <class _CharT, class _Traits, class _Rep, class _Period, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>&
from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, duration<_Rep, _Period>& __d, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


template <class _Clock, class _Duration>
class time_point
{
public:

    using clock         = _Clock;
    using duration      = _Duration;
    using rep           = typename duration::rep;
    using period        = typename duration::period;

    constexpr time_point()
        : _M_d(duration::zero())
    {
    }

    constexpr explicit time_point(const duration& __d)
        : _M_d(__d)
    {
    }

    template <class _Duration2,
              class = enable_if_t<is_assignable_v<duration&, _Duration2>, void>>
    constexpr time_point(const time_point<_Clock, _Duration2>& __t)
        : _M_d(__t.time_since_epoch())
    {
    }

    constexpr duration time_since_epoch() const
    {
        return _M_d;
    }

    constexpr time_point& operator++()
    {
        ++_M_d;
        return *this;
    }

    constexpr time_point operator++(int)
    {
        return time_point{_M_d++};
    }

    constexpr time_point& operator--()
    {
        --_M_d;
        return *this;
    }

    constexpr time_point operator--(int)
    {
        return time_point{_M_d--};
    }

    constexpr time_point& operator+=(const duration& __d)
    {
        _M_d += __d;
        return *this;
    }

    constexpr time_point& operator-=(const duration& __d)
    {
        _M_d -= __d;
        return *this;
    }

    static constexpr time_point min() noexcept
    {
        return time_point(duration::min());
    }

    static constexpr time_point max() noexcept
    {
        return time_point(duration::max());
    }

private:

    duration _M_d;
};


template <class _Clock, class _Duration1, class _Rep2, class _Period2,
          class _CT = time_point<_Clock, common_type_t<_Duration1, duration<_Rep2, _Period2>>>>
constexpr _CT operator+(const time_point<_Clock, _Duration1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return _CT(__lhs.time_since_epoch() + __rhs);
}

template <class _Rep1, class _Period1, class _Clock, class _Duration2,
          class _CT = time_point<_Clock, common_type_t<_Duration2, duration<_Rep1, _Period1>>>>
constexpr _CT operator+(const duration<_Rep1, _Period1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return __rhs + __lhs;
}

template <class _Clock, class _Duration1, class _Rep2, class _Period2,
          class _CT = time_point<_Clock, common_type_t<_Duration1, duration<_Rep2, _Period2>>>>
constexpr _CT operator-(const time_point<_Clock, _Duration1>& __lhs, const duration<_Rep2, _Period2>& __rhs)
{
    return _CT(__lhs.time_since_epoch() - __rhs);
}

template <class _Clock, class _Duration1, class _Duration2>
constexpr common_type_t<_Duration1, _Duration2> operator-(const time_point<_Clock, _Duration1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return __lhs.time_since_epoch() - __rhs.time_since_epoch();
}


template <class _Clock, class _Duration1, class _Duration2>
constexpr bool operator==(const time_point<_Clock, _Duration1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return __lhs.time_since_epoch() == __rhs.time_since_epoch();
}

template <class _Clock, class _Duration1, class _Duration2>
constexpr bool operator!=(const time_point<_Clock, _Duration1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return !(__lhs == __rhs);
}

template <class _Clock, class _Duration1, class _Duration2>
constexpr bool operator<(const time_point<_Clock, _Duration1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return __lhs.time_since_epoch() < __rhs.time_since_epoch();
}

template <class _Clock, class _Duration1, class _Duration2>
constexpr bool operator>(const time_point<_Clock, _Duration1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return __rhs < __lhs;
}

template <class _Clock, class _Duration1, class _Duration2>
constexpr bool operator<=(const time_point<_Clock, _Duration1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return !(__rhs < __lhs);
}

template <class _Clock, class _Duration1, class _Duration2>
constexpr bool operator>=(const time_point<_Clock, _Duration1>& __lhs, const time_point<_Clock, _Duration2>& __rhs)
{
    return !(__lhs < __rhs);
}


template <class _ToDuration, class _Clock, class _Duration,
          class = enable_if_t<__detail::__is_duration_v<_ToDuration>, void>>
constexpr time_point<_Clock, _ToDuration> time_point_cast(const time_point<_Clock, _Duration>& __t)
{
    return time_point<_Clock, _ToDuration>(duration_cast<_ToDuration>(__t.time_since_epoch()));
}

template <class _ToDuration, class _Clock, class _Duration,
          class = enable_if_t<__detail::__is_duration_v<_ToDuration>, void>>
constexpr time_point<_Clock, _ToDuration> floor(const time_point<_Clock, _Duration>& __t)
{
    return time_point<_Clock, _ToDuration>(floor<_ToDuration>(__t.time_since_epoch()));
}

template <class _ToDuration, class _Clock, class _Duration,
          class = enable_if_t<__detail::__is_duration_v<_ToDuration>, void>>
constexpr time_point<_Clock, _ToDuration> ceil(const time_point<_Clock, _Duration>& __t)
{
    return time_point<_Clock, _ToDuration>(ceil<_ToDuration>(__t.time_since_epoch()));
}

template <class _ToDuration, class _Clock, class _Duration,
          class = enable_if_t<__detail::__is_duration_v<_ToDuration>, void>>
constexpr time_point<_Clock, _ToDuration> round(const time_point<_Clock, _Duration>& __t)
{
    return time_point<_Clock, _ToDuration>(round<_ToDuration>(__t.time_since_epoch()));
}


// Defined in the main chrono library.
class system_clock;
class utc_clock;
class tai_clock;
class gps_clock;
class file_clock;
class steady_clock;
class high_resolution_clock;

// Values are defined in the main chrono library.
class year_month_day;
constexpr year_month_day __system_clock_epoch();

template <class _Duration>
using sys_time = time_point<system_clock, _Duration>;
using sys_seconds = sys_time<seconds>;
using sys_days    = sys_time<days>;

template <class _Duration>
using utc_time = time_point<utc_clock, _Duration>;
using utc_seconds = utc_time<seconds>;

template <class _Duration>
using tai_time = time_point<tai_clock, _Duration>;
using tai_seconds = tai_time<seconds>;

template <class _Duration>
using gps_time = time_point<gps_clock, _Duration>;
using gps_seconds = gps_time<seconds>;

template <class _Duration>
using file_time = time_point<file_clock, _Duration>;


struct local_t {};

template <class _Duration>
using local_time = time_point<local_t, _Duration>;
using local_seconds = local_time<seconds>;
using local_days    = local_time<days>;


template <class _DestClock, class _SourceClock>
struct clock_time_conversion {};

template <class _Clock>
struct clock_time_conversion<_Clock, _Clock>
{
    template <class _Duration>
    time_point<_Clock, _Duration> operator()(const time_point<_Clock, _Duration>& __t) const
    {
        return __t;
    }
};

template <>
struct clock_time_conversion<system_clock, system_clock>
{
    template <class _Duration>
    sys_time<_Duration> operator()(const sys_time<_Duration>& __t) const
    {
        return __t;
    }
};

template <>
struct clock_time_conversion<utc_clock, utc_clock>
{
    template <class _Duration>
    utc_time<_Duration> operator()(const utc_time<_Duration>& __t) const
    {
        return __t;
    }
};

// Used to allow references to static methods inside utc_clock without a definition of utc_clock being available.
template <class>
using __deferred_utc_clock = utc_clock;

template <>
struct clock_time_conversion<utc_clock, system_clock>
{
    template <class _Duration>
    utc_time<_Duration> operator()(const sys_time<_Duration>& __t) const
    {
        return __deferred_utc_clock<_Duration>::from_sys(__t);
    }
};

template <>
struct clock_time_conversion<system_clock, utc_clock>
{
    template <class _Duration>
    sys_time<_Duration> operator()(const utc_time<_Duration>& __t) const
    {
        return __deferred_utc_clock<_Duration>::to_sys(__t);
    }
};

template <class _SourceClock>
struct clock_time_conversion<system_clock, _SourceClock>
{
    template <class _Duration>
        requires requires (const time_point<_SourceClock, _Duration>& __t) { _SourceClock::to_sys(__t); }
    auto operator()(const time_point<_SourceClock, _Duration>& __t) const
        -> decltype(_SourceClock::to_sys(__t))
    {
        return _SourceClock::to_sys(__t);
    }
};

template <class _DestClock>
struct clock_time_conversion<_DestClock, system_clock>
{
    template <class _Duration>
        requires requires (const sys_time<_Duration>& __t) { _DestClock::from_sys(__t); }
    auto operator()(const sys_time<_Duration>& __t) const
        -> decltype(_DestClock::from_sys(__t))
    {
        return _DestClock::from_sys(__t);
    }
};

template <class _SourceClock>
struct clock_time_conversion<utc_clock, _SourceClock>
{
    template <class _Duration>
        requires requires (const time_point<_SourceClock, _Duration>& __t) { _SourceClock::to_utc(__t); }
    auto operator()(const time_point<_SourceClock, _Duration>& __t) const
        -> decltype(_SourceClock::to_utc(__t))
    {
        return _SourceClock::to_utc(__t);
    }
};

template <class _DestClock>
struct clock_time_conversion<_DestClock, utc_clock>
{
    template <class _Duration>
        requires requires (const utc_time<_Duration>& __t) { _DestClock::from_utc(__t); }
    auto operator()(const utc_time<_Duration>& __t) const
        -> decltype(_DestClock::from_utc(__t))
    {
        return _DestClock::from_utc(__t);
    }
};


template <class _DC, class _SC, class _D>
concept bool _ClockTimeConversionDirect =
    requires(const time_point<_SC, _D>& __t)
    {
        clock_time_conversion<_DC, _SC>{}(__t);
    };

template <class _DC, class _SC, class _D>
concept bool _ClockTimeConversionViaSystem =
    !_ClockTimeConversionDirect<_DC, _SC, _D>
    && requires(const time_point<_SC, _D>& __t)
    {
        clock_time_conversion<_DC, system_clock>{}
            
            (clock_time_conversion<system_clock, _SC>{}(__t));
    };

template <class _DC, class _SC, class _D>
concept bool _ClockTimeConversionViaUtc =
    !_ClockTimeConversionDirect<_DC, _SC, _D>
    && requires(const time_point<_SC, _D>& __t)
    {
        clock_time_conversion<_DC, utc_clock>{}
            (clock_time_conversion<utc_clock, _SC>{}(__t));
    };

template <class _DC, class _SC, class _D>
concept bool _ClockTimeConversionViaSystemThenUtc =
    !_ClockTimeConversionDirect<_DC, _SC, _D>
    && !_ClockTimeConversionViaSystem<_DC, _SC, _D>
    && !_ClockTimeConversionViaUtc<_DC, _SC, _D>
    && requires(const time_point<_SC, _D>& __t)
    {
        clock_time_conversion<_DC, utc_clock>{}
            (clock_time_conversion<utc_clock, system_clock>{}
                (clock_time_conversion<system_clock, _SC>{}(__t)));
    };

template <class _DC, class _SC, class _D>
concept bool _ClockTimeConversionViaUtcThenSystem =
    !_ClockTimeConversionDirect<_DC, _SC, _D>
    && !_ClockTimeConversionViaSystem<_DC, _SC, _D>
    && !_ClockTimeConversionViaUtc<_DC, _SC, _D>
    && requires(const time_point<_SC, _D>& __t)
    {
        clock_time_conversion<_DC, system_clock>{}
            (clock_time_conversion<system_clock, utc_clock>{}
                (clock_time_conversion<utc_clock, _SC>{}(__t)));
    };

template <class _DestClock, class _SourceClock, class _Duration>
    requires _ClockTimeConversionDirect<_DestClock, _SourceClock, _Duration>
auto clock_cast(const time_point<_SourceClock, _Duration>& __t)
{
    return clock_time_conversion<_DestClock, _SourceClock>{}(__t);
}

template <class _DestClock, class _SourceClock, class _Duration>
    requires _ClockTimeConversionViaSystem<_DestClock, _SourceClock, _Duration>
auto clock_cast(const time_point<_SourceClock, _Duration>& __t)
{
    return clock_time_conversion<_DestClock, system_clock>{}
        (clock_time_conversion<system_clock, _SourceClock>{}(__t));
}

template <class _DestClock, class _SourceClock, class _Duration>
    requires _ClockTimeConversionViaUtc<_DestClock, _SourceClock, _Duration>
auto clock_cast(const time_point<_SourceClock, _Duration>& __t)
{
    return clock_time_conversion<_DestClock, utc_clock>{}
        (clock_time_conversion<utc_clock, _SourceClock>{}(__t));
}

template <class _DestClock, class _SourceClock, class _Duration>
    requires _ClockTimeConversionViaSystemThenUtc<_DestClock, _SourceClock, _Duration>
auto clock_cast(const time_point<_SourceClock, _Duration>& __t)
{
    return clock_time_conversion<_DestClock, utc_clock>{}
        (clock_time_conversion<utc_clock, system_clock>{}
            (clock_time_conversion<system_clock, _SourceClock>{}(__t)));
}

template <class _DestClock, class _SourceClock, class _Duration>
    requires _ClockTimeConversionViaUtcThenSystem<_DestClock, _SourceClock, _Duration>
auto clock_cast(const time_point<_SourceClock, _Duration>& __t)
{
    return clock_time_conversion<_DestClock, system_clock>{}
        (clock_time_conversion<system_clock, utc_clock>{}
            (clock_time_conversion<utc_clock, _SourceClock>{}(__t)));
}


struct last_spec
{
    explicit last_spec() = default;
};

inline constexpr last_spec last{};


class day
{
public:

    day() = default;

    constexpr explicit day(unsigned __d) noexcept
        : _M_d(static_cast<unsigned char>(__d))
    {
    }

    constexpr day& operator++() noexcept
    {
        ++_M_d;
        return *this;
    }

    constexpr day operator++(int) noexcept
    {
        return day{_M_d++};
    }

    constexpr day& operator--() noexcept
    {
        --_M_d;
        return *this;
    }

    constexpr day operator--(int) noexcept
    {
        return day{_M_d--};
    }

    constexpr day& operator+=(const days& __d) noexcept
    {
        _M_d += __d.count();
        return *this;
    }

    constexpr day& operator-=(const days& __d) noexcept
    {
        _M_d -= __d.count();
        return *this;
    }

    constexpr explicit operator unsigned() const noexcept
    {
        return _M_d;
    }

    constexpr bool ok() const noexcept
    {
        return 1 <= _M_d && _M_d <= 31;
    }

private:

    unsigned char _M_d;
};

constexpr bool operator==(const day& __x, const day& __y) noexcept
{
    return unsigned{__x} == unsigned{__y};
}

constexpr bool operator!=(const day& __x, const day& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const day& __x, const day& __y) noexcept
{
    return unsigned{__x} < unsigned{__y};
}

constexpr bool operator>(const day& __x, const day& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const day& __x, const day& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const day& __x, const day& __y) noexcept
{
    return !(__x < __y);
}

constexpr day operator+(const day& __x, const days& __y) noexcept
{
    return day(unsigned{__x} +__y.count());
}

constexpr day operator+(const days& __x, const day& __y) noexcept
{
    return __y + __x;
}

constexpr day operator-(const day& __x, const days& __y) noexcept
{
    return __x + -__y;
}

constexpr days operator-(const day& __x, const day& __y) noexcept
{
    return days{int(unsigned{__x}) - int(unsigned{__y})};
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const day& __d);

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const day& __d);

template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, days& __d, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


class month;
constexpr month operator+(const month&, const months&) noexcept;
constexpr month operator-(const month&, const months&) noexcept;

class month
{
public:

    month() = default;

    constexpr explicit month(unsigned __m) noexcept
        : _M_m(static_cast<unsigned char>(__m))
    {
    }

    constexpr month& operator++() noexcept
    {
        return *this += months{1};
    }

    constexpr month operator++(int) noexcept
    {
        auto __tmp = *this;
        *this += months{1};
        return __tmp;
    }

    constexpr month& operator--() noexcept
    {
        return *this -= months{1};
    }

    constexpr month operator--(int) noexcept
    {
        auto __tmp = *this;
        *this -= months{1};
        return __tmp;
    }

    constexpr month& operator+=(const months& __m) noexcept
    {
        return *this = *this + __m;
    }

    constexpr month& operator-=(const months& __m) noexcept
    {
        return *this = *this - __m;
    }

    constexpr explicit operator unsigned() const noexcept
    {
        return _M_m;
    }

    constexpr bool ok() const noexcept
    {
        return 1 <= _M_m && _M_m <= 12;
    }

private:

    unsigned char _M_m;
};

constexpr bool operator==(const month& __x, const month& __y) noexcept
{
    return unsigned{__x} == unsigned{__y};
}

constexpr bool operator!=(const month& __x, const month& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const month& __x, const month& __y) noexcept
{
    return unsigned{__x} < unsigned{__y};
}

constexpr bool operator>(const month& __x, const month& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const month& __x, const month& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const month& __x, const month& __y) noexcept
{
    return !(__x < __y);
}

constexpr month operator+(const month& __x, const months& __y) noexcept
{
    auto __mod12 = [](auto __val) -> unsigned
    {
        return (__val < 0) ? ((__val % 12) + 12) % 12 : __val % 12;
    };

    return month{__mod12(static_cast<long long>(unsigned{__x}) + (__y.count() - 1)) + 1};
}

constexpr month operator+(const months& __y, const month& __x) noexcept
{
    return __y + __x;
}

constexpr month operator-(const month& __x, const months& __y) noexcept
{
    return __x + -__y;
}

constexpr months operator-(const month& __x, const month& __y) noexcept
{
    int __diff = unsigned{__x} - unsigned{__y};
    return (__diff < 0) ? months{__diff + 12} : months{__diff};
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const month& __m);

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const month& __m);

template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, month& __m, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


inline constexpr month January{1};
inline constexpr month February{2};
inline constexpr month March{3};
inline constexpr month April{4};
inline constexpr month May{5};
inline constexpr month June{6};
inline constexpr month July{7};
inline constexpr month August{8};
inline constexpr month September{9};
inline constexpr month October{10};
inline constexpr month November{11};
inline constexpr month December{12};


class year
{
public:

    year() = default;

    constexpr explicit year(int __y) noexcept
        : _M_y(static_cast<short>(__y))
    {
    }

    constexpr year& operator++() noexcept
    {
        ++_M_y;
        return *this;
    }

    constexpr year operator++(int) noexcept
    {
        return year{_M_y++};
    }

    constexpr year& operator--() noexcept
    {
        --_M_y;
        return *this;
    }

    constexpr year operator--(int) noexcept
    {
        return year{_M_y--};
    }

    constexpr year& operator+=(const years& __y) noexcept
    {
        _M_y += __y.count();
        return *this;
    }

    constexpr year& operator-=(const years& __y) noexcept
    {
        _M_y -= __y.count();
        return *this;
    }

    constexpr year operator+() const noexcept
    {
        return *this;
    }

    constexpr year operator-() const noexcept
    {
        return year{-_M_y};
    }

    constexpr bool is_leap() const noexcept
    {
        return _M_y % 4 == 0 && (_M_y % 100 != 0 || _M_y % 400 == 0);
    }

    constexpr explicit operator int() const noexcept
    {
        return _M_y;
    }

    constexpr bool ok() const noexcept
    {
        return int{min()} <= _M_y && _M_y <= int{max()};
    }

    static constexpr year min() noexcept
    {
        return year{-32767};
    }

    static constexpr year max() noexcept
    {
        return year{32767};
    }

private:

    short _M_y;
};

constexpr bool operator==(const year& __x, const year& __y) noexcept
{
    return int{__x} == int{__y};
}

constexpr bool operator!=(const year& __x, const year& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const year& __x, const year& __y) noexcept
{
    return int{__x} < int{__y};
}

constexpr bool operator>(const year& __x, const year& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const year& __x, const year& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const year& __x, const year& __y) noexcept
{
    return !(__x < __y);
}

constexpr year operator+(const year& __x, const years& __y) noexcept
{
    return year{int{__x} + __y.count()};
}

constexpr year operator+(const years& __x, const year& __y) noexcept
{
    return __y + __x;
}

constexpr year operator-(const year& __x, const years& __y) noexcept
{
    return __x + -__y;
}

constexpr years operator-(const year& __x, const year& __y) noexcept
{
    return years{int{__x} - int{__y}};
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const year& __y);

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const year& __y);

template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, year& __y, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


class weekday;
class weekday_indexed;
class weekday_last;
constexpr weekday operator+(const weekday&, const days&) noexcept;
constexpr weekday operator-(const weekday&, const days&) noexcept;

class weekday
{
public:

    weekday() = default;

    constexpr explicit weekday(unsigned __wd) noexcept
        : _M_wd{static_cast<unsigned char>(__wd)}
    {
    }

    // Defined after year_month_day.
    constexpr weekday(const sys_days& __dp) noexcept;

    constexpr explicit weekday(const local_days& __dp) noexcept
        : weekday{sys_days{__dp.time_since_epoch()}}
    {
    }

    constexpr weekday& operator++() noexcept
    {
        return *this += days{1};
    }

    constexpr weekday operator++(int) noexcept
    {
        auto __tmp = *this;
        *this += days{1};
        return __tmp;
    }

    constexpr weekday& operator--() noexcept
    {
        return *this -= days{1};
    }

    constexpr weekday operator--(int) noexcept
    {
        auto __tmp = *this;
        *this -= days{1};
        return __tmp;
    }

    constexpr weekday& operator+=(const days& __d) noexcept
    {
        return *this = *this + __d;
    }

    constexpr weekday& operator-=(const days& __d) noexcept
    {
        return *this = *this - __d;
    }

    constexpr explicit operator unsigned() const noexcept
    {
        return _M_wd;
    }

    constexpr bool ok() const noexcept
    {
        return _M_wd <= 6;
    }

    // Defined after weekday_index and weekday_last are defined.
    constexpr weekday_indexed operator[](unsigned __index) const noexcept;
    constexpr weekday_last operator[](last_spec) const noexcept;

private:

    unsigned char _M_wd;
};

constexpr bool operator==(const weekday& __x, const weekday& __y) noexcept
{
    return unsigned{__x} == unsigned{__y};
}

constexpr bool operator!=(const weekday& __x, const weekday& __y) noexcept
{
    return !(__x == __y);
}

constexpr weekday operator+(const weekday& __x, const days& __y) noexcept
{
    auto __mod7 = [](auto __val) -> unsigned
    {
        return (__val < 0) ? ((__val % 7) + 7) % 7 : __val % 7;
    };

    return weekday{__mod7(static_cast<long long>(unsigned{__x}) + __y.count())};
}

constexpr weekday operator+(const days& __x, const weekday& __y) noexcept
{
    return __y + __x;
}

constexpr weekday operator-(const weekday& __x, const days& __y) noexcept
{
    return __x + -__y;
}

constexpr days operator-(const weekday& __x, const weekday& __y) noexcept
{
    int __diff = unsigned{__x} - unsigned{__y};
    return (__diff < 0) ? days{__diff + 7} : days{__diff};
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const weekday& __wd);

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const weekday& __wd);

template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, weekday& __wd, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


inline constexpr weekday Sunday{0};
inline constexpr weekday Monday{1};
inline constexpr weekday Tuesday{2};
inline constexpr weekday Wednesday{3};
inline constexpr weekday Thursday{4};
inline constexpr weekday Friday{5};
inline constexpr weekday Saturday{6};


class weekday_indexed
{
public:

    weekday_indexed() = default;

    constexpr weekday_indexed(const chrono::weekday& __wd, unsigned __index) noexcept
        : _M_wd(__wd), _M_index(__index)
    {
    }

    constexpr chrono::weekday weekday() const noexcept
    {
        return _M_wd;
    }

    constexpr unsigned index() const noexcept
    {
        return _M_index;
    }

    constexpr bool ok() const noexcept
    {
        return _M_wd.ok() && 1 <= _M_index && _M_index <= 5;
    }

private:

    chrono::weekday _M_wd;
    unsigned char   _M_index;
};

constexpr bool operator==(const weekday_indexed& __x, const weekday_indexed& __y) noexcept
{
    return __x.weekday() == __y.weekday() && __x.index() == __y.index();
}

constexpr bool operator!=(const weekday_indexed& __x, const weekday_indexed& __y) noexcept
{
    return !(__x == __y);
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const weekday_indexed& __wdi)
{
    bool __valid_idx = 1 <= __wdi.index() && __wdi.index() <= 5;
    __os << __wdi.weekday() << '[' << __wdi.index();
    if (__valid_idx)
        __os << ']';
    else
        __os << " is not a valid index]";
    return __os;
}


class weekday_last
{
public:

    constexpr explicit weekday_last(const chrono::weekday& __wd) noexcept
        : _M_wd{__wd}
    {
    }

    constexpr chrono::weekday weekday() const noexcept
    {
        return _M_wd;
    }

    constexpr bool ok() const noexcept
    {
        return _M_wd.ok();
    }

private:

    chrono::weekday _M_wd;
};

constexpr bool operator==(const weekday_last& __x, const weekday_last& __y) noexcept
{
    return __x.weekday() == __y.weekday();
}

constexpr bool operator!=(const weekday_last& __x, const weekday_last& __y) noexcept
{
    return !(__x == __y);
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const weekday_last& __wdl)
{
    return __os << __wdl.weekday() << "[last]";
}


constexpr weekday_indexed weekday::operator[](unsigned __index) const noexcept
{
    return {*this, __index};
}

constexpr weekday_last weekday::operator[](last_spec) const noexcept
{
    return weekday_last{*this};
}


class month_day
{
public:

    month_day() = default;

    constexpr month_day(const chrono::month& __m, const chrono::day& __d) noexcept
        : _M_m{__m}, _M_d{__d}
    {
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr chrono::day day() const noexcept
    {
        return _M_d;
    }

    constexpr bool ok() const noexcept
    {
        if (!_M_m.ok())
            return false;

        if (chrono::day{1} > _M_d)
            return false;

        if (_M_m == February)
            return (_M_d <= chrono::day{29});

        if (_M_m == April || _M_m == June || _M_m == September || _M_m == November)
            return (_M_d <= chrono::day{30});

        return (_M_d <= chrono::day{31});
    }

private:

    chrono::month _M_m;
    chrono::day   _M_d;
};

constexpr bool operator==(const month_day& __x, const month_day& __y) noexcept
{
    return __x.month() == __y.month() && __x.day() == __y.day();
}

constexpr bool operator!=(const month_day& __x, const month_day& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const month_day& __x, const month_day& __y) noexcept
{
    if (__x.month() < __y.month())
        return true;
    else if (__x.month() > __y.month())
        return false;
    else // (__x.month() == __y.month())
        return __x.day() < __y.day();
}

constexpr bool operator>(const month_day& __x, const month_day& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const month_day& __x, const month_day& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const month_day& __x, const month_day& __y) noexcept
{
    return !(__x < __y);
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const month_day& __md)
{
    return __os << __md.month() << '/' << __md.day();
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const month_day& __md);

template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, month_day& __md, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


class month_day_last
{
public:

    constexpr explicit month_day_last(const chrono::month& __m) noexcept
        : _M_m{__m}
    {
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr bool ok() const noexcept
    {
        return _M_m.ok();
    }

private:

    chrono::month _M_m;
};

constexpr bool operator==(const month_day_last& __x, const month_day_last& __y) noexcept
{
    return __x.month() == __y.month();
}

constexpr bool operator!=(const month_day_last& __x, const month_day_last& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const month_day_last& __x, const month_day_last& __y) noexcept
{
    return __x.month() < __y.month();
}

constexpr bool operator>(const month_day_last& __x, const month_day_last& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const month_day_last& __x, const month_day_last& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const month_day_last& __x, const month_day_last& __y) noexcept
{
    return !(__x < __y);
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const month_day_last& __mdl)
{
    return __os << __mdl.month() << "/last";
}


class month_weekday
{
public:

    constexpr month_weekday(const chrono::month& __m, const chrono::weekday_indexed& __wdi) noexcept
        : _M_m{__m}, _M_wdi{__wdi}
    {
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr chrono::weekday_indexed weekday_indexed() const noexcept
    {
        return _M_wdi;
    }

    constexpr bool ok() const noexcept
    {
        return _M_m.ok() && _M_wdi.ok();
    }

private:

    chrono::month           _M_m;
    chrono::weekday_indexed _M_wdi;
};

constexpr bool operator==(const month_weekday& __x, const month_weekday& __y) noexcept
{
    return __x.month() == __y.month() && __x.weekday_indexed() == __y.weekday_indexed();
}

constexpr bool operator!=(const month_weekday& __x, const month_weekday& __y) noexcept
{
    return !(__x == __y);
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const month_weekday& __mwd)
{
    return __os << __mwd.month() << '/' << __mwd.weekday_indexed();
}


class month_weekday_last
{
public:

    constexpr month_weekday_last(const chrono::month& __m, const chrono::weekday_last& __wdl) noexcept
        : _M_m{__m}, _M_wdl{__wdl}
    {
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr chrono::weekday_last weekday_last() const noexcept
    {
        return _M_wdl;
    }

    constexpr bool ok() const noexcept
    {
        return _M_m.ok() && _M_wdl.ok();
    }

private:

    chrono::month        _M_m;
    chrono::weekday_last _M_wdl;
};

constexpr bool operator==(const month_weekday_last& __x, const month_weekday_last& __y) noexcept
{
    return __x.month() == __y.month() && __x.weekday_last() == __y.weekday_last();
}

constexpr bool operator!=(const month_weekday_last& __x, const month_weekday_last& __y) noexcept
{
    return !(__x == __y);
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const month_weekday_last& __mwdl)
{
    return __os << __mwdl.month() << '/' << __mwdl.weekday_last();
}


class year_month;
constexpr year_month operator+(const year_month&, const months&) noexcept;
constexpr year_month operator-(const year_month&, const months&) noexcept;
constexpr year_month operator+(const year_month&, const years&) noexcept;
constexpr year_month operator-(const year_month&, const years&) noexcept;

class year_month
{
public:

    year_month() = default;

    constexpr year_month(const chrono::year& __y, const chrono::month& __m) noexcept
        : _M_y{__y}, _M_m{__m}
    {
    }

    constexpr chrono::year year() const noexcept
    {
        return _M_y;
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr year_month& operator+=(const months& __dm) noexcept
    {
        return *this = *this + __dm;
    }

    constexpr year_month& operator-=(const months& __dm) noexcept
    {
        return *this = *this - __dm;
    }

    constexpr year_month& operator+=(const years& __dy) noexcept
    {
        return *this = *this + __dy;
    }

    constexpr year_month& operator-=(const years& __dy) noexcept
    {
        return *this = *this - __dy;
    }

    constexpr bool ok() const noexcept
    {
        return _M_y.ok() && _M_m.ok();
    }

private:

    chrono::year  _M_y;
    chrono::month _M_m;
};

constexpr bool operator==(const year_month& __x, const year_month& __y) noexcept
{
    return __x.year() == __y.year() && __x.month() == __y.month();
}

constexpr bool operator!=(const year_month& __x, const year_month& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const year_month& __x, const year_month& __y) noexcept
{
    if (__x.year() < __y.year())
        return true;
    else if (__x.year() > __y.year())
        return false;
    else // (__x.year() == __y.year())
        return (__x.month() < __y.month());
}

constexpr bool operator>(const year_month& __x, const year_month& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const year_month& __x, const year_month& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const year_month& __x, const year_month& __y) noexcept
{
    return !(__x < __y);
}

constexpr year_month operator+(const year_month& __ym, const months& __dm) noexcept
{
    auto __s = __sign(__dm.count());
    auto __m = __XVI_STD_UTILITY_NS::abs(__dm.count());
    auto __years = years{__m / 12};
    auto __months = months{__m % 12};
    if (__s > 0)
    {
        auto __month = __ym.month() + __months;
        if (__month < __ym.month())
            ++__years;
        return year_month{__ym.year() + __years, __month};
    }
    else
    {
        auto __month = __ym.month() - __months;
        if (__month > __ym.month())
            ++__years;
        return year_month{__ym.year() - __years, __month};
    }
}

constexpr year_month operator+(const months& __dm, const year_month& __ym) noexcept
{
    return __ym + __dm;
}

constexpr year_month operator-(const year_month& __ym, const months& __dm) noexcept
{
    return __ym + -__dm;
}

constexpr months operator-(const year_month& __x, const year_month& __y) noexcept
{
    return __x.year() - __y.year() + months{static_cast<int>(unsigned{__x.month()})
                                            - static_cast<int>(unsigned{__y.month()})};
}

constexpr year_month operator+(const year_month& __ym, const years& __dy) noexcept
{
    return year_month{__ym.year() + __dy, __ym.month()};
}

constexpr year_month operator+(const years& __dy, const year_month& __ym) noexcept
{
    return __ym + __dy;
}

constexpr year_month operator-(const year_month& __ym, const years& __dy) noexcept
{
    return __ym + -__dy;
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const year_month& __ym)
{
    return __os << __ym.year() << '/' << __ym.month();
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const year_month& __ym);

template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, year_month& __ym, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


class year_month_day_last;
constexpr year_month_day_last operator+(const year_month_day_last&, const months&) noexcept;
constexpr year_month_day_last operator-(const year_month_day_last&, const months&) noexcept;
constexpr year_month_day_last operator+(const year_month_day_last&, const years&) noexcept;
constexpr year_month_day_last operator-(const year_month_day_last&, const years&) noexcept;

class year_month_day_last
{
public:

    constexpr year_month_day_last(const chrono::year& __y, const chrono::month_day_last& __mdl) noexcept
        : _M_y{__y}, _M_mdl{__mdl}
    {
    }

    constexpr year_month_day_last& operator+=(const months& __m) noexcept
    {
        return *this = *this + __m;
    }

    constexpr year_month_day_last& operator-=(const months& __m) noexcept
    {
        return *this = *this - __m;
    }

    constexpr year_month_day_last& operator+=(const years& __y) noexcept
    {
        return *this = *this + __y;
    }

    constexpr year_month_day_last& operator-=(const years& __y) noexcept
    {
        return *this = *this - __y;
    }

    constexpr chrono::year year() const noexcept
    {
        return _M_y;
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_mdl.month();
    }

    constexpr chrono::month_day_last month_day_last() const noexcept
    {
        return _M_mdl;
    }

    constexpr chrono::day day() const noexcept
    {
        if (month() == February)
            return (year().is_leap()) ? chrono::day{29} : chrono::day{28};

        if (month() == April || month() == June || month() == September || month() == November)
            return chrono::day{30};
        
        return chrono::day{31};
    }

    // Defined after year_month_day.
    constexpr operator sys_days() const noexcept;

    constexpr explicit operator local_days() const noexcept
    {
        return local_days{sys_days{*this}.time_since_epoch()};
    }

    constexpr bool ok() const noexcept
    {
        return _M_y.ok() && _M_mdl.ok();
    }

private:

    chrono::year           _M_y;
    chrono::month_day_last _M_mdl;
};

constexpr bool operator==(const year_month_day_last& __x, const year_month_day_last& __y) noexcept
{
    return __x.year() == __y.year() && __x.month_day_last() == __y.month_day_last();
}

constexpr bool operator!=(const year_month_day_last& __x, const year_month_day_last& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const year_month_day_last& __x, const year_month_day_last& __y) noexcept
{
    if (__x.year() < __y.year())
        return true;
    else if (__x.year() > __y.year())
        return false;
    else
        return (__x.month() < __y.month());
}

constexpr bool operator>(const year_month_day_last& __x, const year_month_day_last& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const year_month_day_last& __x, const year_month_day_last& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const year_month_day_last& __x, const year_month_day_last& __y) noexcept
{
    return !(__x < __y);
}

constexpr year_month_day_last operator+(const year_month_day_last& __ymdl, const months& __dm) noexcept
{
    auto __ym = year_month{__ymdl.year(), __ymdl.month()} + __dm;
    return {__ym.year(), month_day_last{__ym.month()}};
}

constexpr year_month_day_last operator+(const months& __dm, const year_month_day_last& __ymdl) noexcept
{
    return __ymdl + __dm;
}

constexpr year_month_day_last operator-(const year_month_day_last& __ymdl, const months& __dm) noexcept
{
    return __ymdl + -__dm;
}

constexpr year_month_day_last operator+(const year_month_day_last& __ymdl, const years& __dy) noexcept
{
    return {__ymdl.year() + __dy, __ymdl.month_day_last()};
}

constexpr year_month_day_last operator+(const years& __dy, const year_month_day_last& __ymdl) noexcept
{
    return __ymdl + __dy;
}

constexpr year_month_day_last operator-(const year_month_day_last& __ymdl, const years& __dy) noexcept
{
    return __ymdl + -__dy;
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const year_month_day_last& __ymdl) noexcept
{
    return __os << __ymdl.year() << '/' << __ymdl.month_day_last();
}


// Calculates number of leap years between the given year and year 0 (1 BCE).
constexpr auto __leap_count(chrono::year __y)
{
    int __n = __XVI_STD_UTILITY_NS::abs(int{__y});
    return __n/4 - __n/100 + __n/400;
}

// Calculates the number of years (counting from year 0) the given day count represents.
constexpr auto __year_count(chrono::days __d)
{
    // Number of 400-year periods.
    constexpr int __days_per_400_years = (365 * 400) + __leap_count(chrono::year{400});  
    auto __quadcenturies = __d.count() / __days_per_400_years;
    auto __remaining = __d.count() % __days_per_400_years;

    // Number of 100-year periods remaining.
    constexpr int __days_per_100_years = (365 * 100) + __leap_count(chrono::year{100});
    auto __centuries = __remaining / __days_per_100_years;
    __remaining %= __days_per_100_years;

    // Number of 4-year periods remaining.
    constexpr int __days_per_4_years = (365 * 4) + __leap_count(chrono::year{4});
    auto __quads = __remaining / __days_per_4_years;
    __remaining %= __days_per_4_years;

    // Number of 1-year periods remaining.
    auto __years = __remaining / 365;
    __remaining %= 365;

    // Total number of years.
    __years += (__quads * 4) + (__centuries * 100) + (__quadcenturies * 400);
    return pair{year{__years}, days{__remaining}};
};

// Returns the zero-based index of the month+day within the given year.
constexpr days __day_of_year(year __y, month __m, day __d)
{
    unsigned __n = unsigned{__d} - 1;

    switch (unsigned{__m})
    {
        case 12:
            __n += 30;
        case 11:
            __n += 31;
        case 10:
            __n += 30;
        case 9:
            __n += 31;
        case 8:
            __n += 31;
        case 7:
            __n += 30;
        case 6:
            __n += 31;
        case 5:
            __n += 30;
        case 4:
            __n += 31;
        case 3:
            __n += (__y.is_leap()) ? 29 : 28;
        case 2:
            __n += 31;
        case 1:
            break;
    }

    return days{__n};
}


constexpr month_day __day_of_year(year __y, days __d)
{
    auto __n = static_cast<unsigned>(__d.count());

    // January is the same in all years
    if (__n < 31)
        return {January, day{__n + 1}};
    
    // February depends on if is leap year.
    __n -= 31;
    if (__y.is_leap())
    {
        if (__n < 29)
            return {February, day{__n + 1}};
        __n -= 29;
    }
    else
    {
        if (__n < 28)
            return {February, day{__n + 1}};
        __n -= 28;
    }

    if (__n < 31)
        return {March, day{__n + 1}};
    __n -= 31;

    if (__n < 30)
        return {April, day{__n + 1}};
    __n -= 30;

    if (__n < 31)
        return {May, day{__n + 1}};
    __n -= 31;

    if (__n < 30)
        return {June, day{__n + 1}};
    __n -= 30;

    if (__n < 31)
        return {July, day{__n + 1}};
    __n -= 31;

    if (__n < 31)
        return {August, day{__n + 1}};
    __n -= 31;

    if (__n < 30)
        return {September, day{__n + 1}};
    __n -= 30;

    if (__n < 31)
        return {October, day{__n + 1}};
    __n -= 31;

    if (__n < 30)
        return {November, day{__n + 1}};
    __n -= 30;

    return {December, day{__n + 1}};
}

class year_month_day;
constexpr year_month_day operator+(const year_month_day&, const months&) noexcept;
constexpr year_month_day operator-(const year_month_day&, const months&) noexcept;
constexpr year_month_day operator+(const year_month_day&, const years&) noexcept;
constexpr year_month_day operator-(const year_month_day&, const years&) noexcept;

class year_month_day
{
public:

    year_month_day() = default;

    constexpr year_month_day(const chrono::year& __y, const chrono::month& __m, const chrono::day& __d) noexcept
        : _M_y{__y}, _M_m{__m}, _M_d{__d}
    {
    }

    constexpr year_month_day(const year_month_day_last& __ymdl) noexcept;

    constexpr year_month_day(const sys_days& __dp) noexcept
        : year_month_day(__from_sys_days(__dp))
    {
    }

    constexpr explicit year_month_day(const local_days& __dp) noexcept
        : year_month_day{sys_days{__dp.time_since_epoch()}}
    {
    }

    constexpr year_month_day& operator+=(const months& __m) noexcept
    {
        return *this = *this + __m;
    }

    constexpr year_month_day& operator-=(const months& __m) noexcept
    {
        return *this = *this - __m;
    }

    constexpr year_month_day& operator+=(const years& __y) noexcept
    {
        return *this = *this + __y;
    }

    constexpr year_month_day& operator-=(const years& __y) noexcept
    {
        return *this = *this - __y;
    }

    constexpr chrono::year year() const noexcept
    {
        return _M_y;
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr chrono::day day() const noexcept
    {
        return _M_d;
    }

    constexpr operator sys_days() const noexcept
    {
        return __to_sys_days(*this);
    }

    constexpr explicit operator local_days() const noexcept
    {
        return local_days{sys_days{*this}.time_since_epoch()};
    }

    constexpr bool ok() const noexcept
    {
        if (!_M_y.ok() || _M_m.ok())
            return false;

        if (chrono::day{1} > _M_d)
            return false;

        return (_M_d <= year_month_day_last{_M_y, month_day_last{_M_m}}.day());
    }

private:

    chrono::year  _M_y;
    chrono::month _M_m;
    chrono::day   _M_d;

    static constexpr year_month_day __from_sys_days(sys_days __dp)
    {
        // Epoch for sys_clock is 1970-01-01.
        constexpr year_month_day __epoch = {chrono::year{1970}, January, chrono::day{01}};

        // Number of days between sys_clock epoch and 0000-01-01.
        auto __epoch_days = days{int{__epoch.year()} * 365 + __leap_count(__epoch.year())};

        // Number of days beween 0000-01-01 and the target date.
        auto __days = __epoch_days + __dp.time_since_epoch();

        // Number of years between 0000-01-01 and the target date and left-over days.
        auto [__y, __n] = __year_count(__days);

        // Calculate the month and day.
        auto __md = __day_of_year(__y, __n);

        return {__y, __md.month(), __md.day()};
    }

    static constexpr sys_days __to_sys_days(year_month_day __ymd)
    {
        // Epoch for sys_clock is 1970-01-01.
        constexpr year_month_day __epoch = {chrono::year{1970}, January, chrono::day{01}};

        // Number of days between sys_clock epoch and 0000-01-01.
        auto __epoch_days = days{int{__epoch.year()} * 365 + __leap_count(__epoch.year())};

        // Number of days beween 0000-01-01 and the given date
        auto __days = days{int{__ymd.year()} * 365 + __leap_count(__ymd.year())};
        __days += days{__day_of_year(__ymd.year(), __ymd.month(), __ymd.day())};

        // Number of days between sys_clock epoch and the given date.
        auto __days_since_epoch = __days - __epoch_days;

        return sys_days{__days_since_epoch};
    }
};

constexpr bool operator==(const year_month_day& __x, const year_month_day& __y) noexcept
{
    return __x.year() == __y.year() && __x.month() == __y.month() && __x.day() == __y.day();
}

constexpr bool operator!=(const year_month_day& __x, const year_month_day& __y) noexcept
{
    return !(__x == __y);
}

constexpr bool operator<(const year_month_day& __x, const year_month_day& __y) noexcept
{
    if (__x.year() < __y.year())
        return true;
    else if (__x.year() > __y.year())
        return false;
    else if (__x.month() < __y.month())
        return true;
    else if (__x.month() > __y.month())
        return false;
    else
        return (__x.day() < __y.day());
}

constexpr bool operator>(const year_month_day& __x, const year_month_day& __y) noexcept
{
    return __y < __x;
}

constexpr bool operator<=(const year_month_day& __x, const year_month_day& __y) noexcept
{
    return !(__y < __x);
}

constexpr bool operator>=(const year_month_day& __x, const year_month_day& __y) noexcept
{
    return !(__x < __y);
}

constexpr year_month_day operator+(const year_month_day& __ymd, const months& __dm) noexcept
{
    auto __ym = (year_month{__ymd.year(), __ymd.month()} + __dm);
    return {__ym.year(), __ym.month(), __ymd.day()};
}

constexpr year_month_day operator+(const months& __dm, const year_month_day& __ymd) noexcept
{
    return __ymd + __dm;
}

constexpr year_month_day operator-(const year_month_day& __ymd, const months& __dm) noexcept
{
    return __ymd + -__dm;
}

constexpr year_month_day operator+(const year_month_day& __ymd, const years& __dy) noexcept
{
    return {__ymd.year() + __dy, __ymd.month(), __ymd.day()};
}

constexpr year_month_day operator+(const years& __dy, const year_month_day& __ymd) noexcept
{
    return __ymd + __dy;
}

constexpr year_month_day operator-(const year_month_day& __ymd, const years& __dy) noexcept
{
    return __ymd + -__dy;
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const year_month_day& __ymd);

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& to_stream(basic_ostream<_CharT, _Traits>& __os, const _CharT* __fmt, const year_month_day& __ymd);

template <class _CharT, class _Traits, class _Alloc = allocator<_CharT>>
basic_istream<_CharT, _Traits>& from_stream(basic_istream<_CharT, _Traits>& __is, const _CharT* __fmt, year_month_day& __ymd, basic_string<_CharT, _Traits, _Alloc>* __abbrev = nullptr, minutes* __offset = nullptr);


constexpr unsigned char __day_of_week(const year_month_day& __ymd) noexcept
{
    constexpr unsigned char __w[12] =
        { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };

    auto __y = int{__ymd.year()};
    auto __m = unsigned{__ymd.month()};
    auto __d = unsigned{__ymd.day()};

    if (__ymd.month() < March)
        --__y;

    auto __wd = (__y + __y / 4 - __y / 100 + __y / 400 + __w[__m-1] + __d) % 7;
    return __wd;
}

// Note: epoch date of 1970-01-01 was a Thursday.
constexpr weekday::weekday(const sys_days& __dp) noexcept
    //: _M_wd{__day_of_week(year_month_day{__dp})}
    : weekday(Thursday + __dp.time_since_epoch())
{
}

constexpr year_month_day_last::operator sys_days() const noexcept
{
    return year_month_day{year(), month(), day()};
}


class year_month_weekday;
constexpr year_month_weekday operator+(const year_month_weekday&, const months&) noexcept;
constexpr year_month_weekday operator-(const year_month_weekday&, const months&) noexcept;
constexpr year_month_weekday operator+(const year_month_weekday&, const years&) noexcept;
constexpr year_month_weekday operator-(const year_month_weekday&, const years&) noexcept;

class year_month_weekday
{
public:

    year_month_weekday() = default;

    constexpr year_month_weekday(const chrono::year& __y, const chrono::month& __m, const chrono::weekday_indexed& __wdi) noexcept
        : _M_y{__y}, _M_m{__m}, _M_wdi{__wdi}
    {
    }

    constexpr year_month_weekday(const sys_days& __dp) noexcept
        : year_month_weekday{__from_sys_days(__dp)}
    {
    }

    constexpr explicit year_month_weekday(const local_days& __dp) noexcept
        : year_month_weekday{sys_days{__dp.time_since_epoch()}}
    {
    }

    constexpr year_month_weekday& operator+=(const months& __m) noexcept
    {
        return *this = *this + __m;
    }

    constexpr year_month_weekday& operator-=(const months& __m) noexcept
    {
        return *this = *this - __m;
    }

    constexpr year_month_weekday& operator+=(const years& __y) noexcept
    {
        return *this = *this + __y;
    }

    constexpr year_month_weekday& operator-=(const years& __y) noexcept
    {
        return *this = *this - __y;
    }

    constexpr chrono::year year() const noexcept
    {
        return _M_y;
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr chrono::weekday weekday() const noexcept
    {
        return _M_wdi.weekday();
    }

    constexpr unsigned index() const noexcept
    {
        return _M_wdi.index();
    }

    constexpr chrono::weekday_indexed weekday_indexed() const noexcept
    {
        return _M_wdi;
    }

    constexpr operator sys_days() const noexcept
    {
        if (index() != 0)
        {
            return sys_days{year_month_day{year(), month(), __get_day()}};
        }
        else
        {
            auto __first = year_month_weekday{year(), month(), {weekday(), 1}};
            return sys_days{year_month_day{year(), month(), __first.__get_day()}} - days{7};
        }
    }

    constexpr explicit operator local_days() const noexcept
    {
        return local_days{sys_days{*this}.time_since_epoch()};
    }

    constexpr bool ok() const noexcept
    {
        if (!_M_y.ok() || !_M_m.ok() || !_M_wdi.ok())
            return false;

        return year_month_day{year(), month(), __get_day()}.ok();
    }

private:

    chrono::year            _M_y;
    chrono::month           _M_m;
    chrono::weekday_indexed _M_wdi;

    static constexpr year_month_weekday __from_sys_days(sys_days __dp) noexcept
    {
        // Convert to YMD then get the weekday from that.
        auto __ymd = year_month_day{__dp};
        auto __wd = chrono::weekday{__dp};
        auto __index = (unsigned{__ymd.day()} - 1) / 7;
        return {__ymd.year(), __ymd.month(), {__wd, __index}};
    }

    constexpr day __get_day() const noexcept
    {
        // First weekday of the year+month.
        auto __first_day = year_month_day{year(), month(), day{1}};
        auto __first_wd = chrono::weekday{sys_days{__first_day}};

        // Number of days required to get to the correct weekday.
        unsigned __days = (weekday() - __first_wd).count();
        __days += (index() - 1) * 7;

        return day{__days + 1};
    }
};

constexpr bool operator==(const year_month_weekday& __x, const year_month_weekday& __y) noexcept
{
    return __x.year() == __y.year() && __x.month() == __y.month() && __x.weekday_indexed() == __y.weekday_indexed();
}

constexpr bool operator!=(const year_month_weekday& __x, const year_month_weekday& __y) noexcept
{
    return !(__x == __y);
}

constexpr year_month_weekday operator+(const year_month_weekday& __ymwd, const months& __dm) noexcept
{
    auto __ym = year_month{__ymwd.year(), __ymwd.month()} + __dm;
    return {__ym.year(), __ym.month(), __ymwd.weekday_indexed()};
}

constexpr year_month_weekday operator+(const months& __dm, const year_month_weekday& __ymwd) noexcept
{
    return __ymwd + __dm;
}

constexpr year_month_weekday operator-(const year_month_weekday& __ymwd, const months& __dm) noexcept
{
    return __ymwd + -__dm;
}

constexpr year_month_weekday operator+(const year_month_weekday& __ymwd, const years& __dy) noexcept
{
    return {__ymwd.year() + __dy, __ymwd.month(), __ymwd.weekday_indexed()};
}

constexpr year_month_weekday operator+(const years& __dy, const year_month_weekday& __ymwd) noexcept
{
    return __ymwd + __dy;
}

constexpr year_month_weekday operator-(const year_month_weekday& __ymwd, const years& __dy) noexcept
{
    return __ymwd + -__dy;
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits&> operator<<(basic_ostream<_CharT, _Traits>& __os, const year_month_weekday& __ymwd)
{
    return __os << __ymwd.year() << '/' << __ymwd.month() << '/' << __ymwd.weekday_indexed();
}


class year_month_weekday_last;
constexpr year_month_weekday_last operator+(const year_month_weekday_last&, const months&) noexcept;
constexpr year_month_weekday_last operator-(const year_month_weekday_last&, const months&) noexcept;
constexpr year_month_weekday_last operator+(const year_month_weekday_last&, const years&) noexcept;
constexpr year_month_weekday_last operator-(const year_month_weekday_last&, const years&) noexcept;

class year_month_weekday_last
{
public:

    constexpr year_month_weekday_last(const chrono::year& __y, const chrono::month& __m, const chrono::weekday_last& __wdl) noexcept
        : _M_y{__y}, _M_m{__m}, _M_wdl{__wdl}
    {
    }

    constexpr year_month_weekday_last& operator+=(const months& __m) noexcept
    {
        return *this = *this + __m;
    }

    constexpr year_month_weekday_last& operator-=(const months& __m) noexcept
    {
        return *this = *this - __m;
    }

    constexpr year_month_weekday_last& operator+=(const years& __y) noexcept
    {
        return *this = *this + __y;
    }

    constexpr year_month_weekday_last& operator-=(const years& __y) noexcept
    {
        return *this = *this - __y;
    }

    constexpr chrono::year year() const noexcept
    {
        return _M_y;
    }

    constexpr chrono::month month() const noexcept
    {
        return _M_m;
    }

    constexpr chrono::weekday weekday() const noexcept
    {
        return _M_wdl.weekday();
    }

    constexpr chrono::weekday_last weekday_last() const noexcept
    {
        return _M_wdl;
    }

    constexpr operator sys_days() const noexcept
    {
        // There are either 4 or 5 of each day in a month.
        auto __ymwd = year_month_weekday{year(), month(), {weekday(), 5}};
        if (__ymwd.ok())
            return sys_days{__ymwd};
        else
            return sys_days{__ymwd} - days{7};
    }

    constexpr explicit operator local_days() const noexcept
    {
        return local_days{sys_days{*this}.time_since_epoch()};
    }

    constexpr bool ok() const noexcept
    {
        return _M_y.ok() && _M_m.ok() && _M_wdl.ok();
    }

private:

    chrono::year         _M_y;
    chrono::month        _M_m;
    chrono::weekday_last _M_wdl;
};

constexpr bool operator==(const year_month_weekday_last& __x, const year_month_weekday_last& __y) noexcept
{
    return __x.year() == __y.year() && __x.month() == __y.month() && __x.weekday_last() == __y.weekday_last();
}

constexpr bool operator!=(const year_month_weekday_last& __x, const year_month_weekday_last& __y) noexcept
{
    return !(__x == __y);
}

constexpr year_month_weekday_last operator+(const year_month_weekday_last& __ymwdl, const months& __dm) noexcept
{
    auto __ym = year_month{__ymwdl.year(), __ymwdl.month()} + __dm;
    return {__ym.year(), __ym.month(), __ymwdl.weekday_last()};
}

constexpr year_month_weekday_last operator+(const months& __dm, const year_month_weekday_last& __ymwdl) noexcept
{
    return __ymwdl + __dm;
}

constexpr year_month_weekday_last operator-(const year_month_weekday_last& __ymwdl, const months& __dm) noexcept
{
    return __ymwdl + -__dm;
}

constexpr year_month_weekday_last operator+(const year_month_weekday_last& __ymwdl, const years& __dy) noexcept
{
    return {__ymwdl.year() + __dy, __ymwdl.month(), __ymwdl.weekday_last()};
}

constexpr year_month_weekday_last operator+(const years& __dy, const year_month_weekday_last& __ymwdl) noexcept
{
    return __ymwdl + __dy;
}

constexpr year_month_weekday_last operator-(const year_month_weekday_last& __ymwdl, const years& __dy) noexcept
{
    return __ymwdl + -__dy;
}

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const year_month_weekday_last& __ymwdl)
{
    return __os << __ymwdl.year() << '/' << __ymwdl.month() << '/' << __ymwdl.weekday_last();
}


constexpr year_month operator/(const year& __y, const month& __m) noexcept
{
    return {__y, __m};
}

constexpr year_month operator/(const year& __y, int __m) noexcept
{
    return __y / month(__m);
}

constexpr month_day operator/(const month& __m, const day& __d) noexcept
{
    return {__m, __d};
}

constexpr month_day operator/(const month& __m, int __d) noexcept
{
    return __m / day(__d);
}

const month_day operator/(int __m, const day& __d) noexcept
{
    return month(__m) / __d;
}

constexpr month_day operator/(const day& __d, const month& __m) noexcept
{
    return __m / __d;
}

constexpr month_day operator/(const day& __d, int __m) noexcept
{
    return month(__m) / __d;
}

constexpr month_day_last operator/(const month& __m, last_spec) noexcept
{
    return month_day_last{__m};
}

constexpr month_day_last operator/(int __m, last_spec) noexcept
{
    return month(__m) / last;
}

constexpr month_day_last operator/(last_spec, const month& __m) noexcept
{
    return __m / last;
}

constexpr month_day_last operator/(last_spec, int __m) noexcept
{
    return month(__m) / last;
}

constexpr month_weekday operator/(const month& __m, const weekday_indexed& __wdi) noexcept
{
    return {__m, __wdi};
}

constexpr month_weekday operator/(const weekday_indexed& __wdi, const month& __m) noexcept
{
    return __m / __wdi;
}

constexpr month_weekday operator/(const weekday_indexed& __wdi, int __m) noexcept
{
    return month(__m) / __wdi;
}

constexpr month_weekday operator/(int __m, const weekday_indexed& __wdi) noexcept
{
    return month(__m) / __wdi;
}

constexpr month_weekday_last operator/(const month& __m, const weekday_last& __wdl) noexcept
{
    return {__m, __wdl};
}

constexpr month_weekday_last operator/(int __m, const weekday_last& __wdl) noexcept
{
    return month(__m) / __wdl;
}

constexpr month_weekday_last operator/(const weekday_last& __wdl, const month& __m) noexcept
{
    return __m / __wdl;
}

constexpr month_weekday_last operator/(const weekday_last& __wdl, int __m) noexcept
{
    return month(__m) / __wdl;
}

constexpr year_month_day operator/(const year_month& __ym, const day& __d) noexcept
{
    return {__ym.year(), __ym.month(), __d};
}

constexpr year_month_day operator/(const year_month& __ym, int __d) noexcept
{
    return __ym / day(__d);
}

constexpr year_month_day operator/(const year& __y, const month_day& __md) noexcept
{
    return __y / __md.month() / __md.day();
}

constexpr year_month_day operator/(int __y, const month_day& __md) noexcept
{
    return year(__y) / __md;
}

constexpr year_month_day operator/(const month_day& __md, const year& __y) noexcept
{
    return __y / __md;
}

constexpr year_month_day operator/(const month_day& __md, int __y) noexcept
{
    return year(__y) / __md;
}

constexpr year_month_day_last operator/(const year_month& __ym, last_spec) noexcept
{
    return {__ym.year(), month_day_last{__ym.month()}};
}

constexpr year_month_day_last operator/(const year& __y, const month_day_last& __mdl) noexcept
{
    return {__y, __mdl};
}

constexpr year_month_day_last operator/(int __y, const month_day_last& __mdl) noexcept
{
    return year(__y) / __mdl;
}

constexpr year_month_day_last operator/(const month_day_last& __mdl, const year& __y) noexcept
{
    return __y / __mdl;
}

constexpr year_month_day_last operator/(const month_day_last& __mdl, int __y) noexcept
{
    return year(__y) / __mdl;
}

constexpr year_month_weekday operator/(const year_month& __ym, const weekday_indexed& __wdi) noexcept
{
    return {__ym.year(), __ym.month(), __wdi};
}

constexpr year_month_weekday operator/(const year& __y, const month_weekday& __mwd) noexcept
{
    return {__y, __mwd.month(), __mwd.weekday_indexed()};
}

constexpr year_month_weekday operator/(int __y, const month_weekday& __mwd) noexcept
{
    return year(__y) / __mwd;
}

constexpr year_month_weekday operator/(const month_weekday& __mwd, const year& __y) noexcept
{
    return __y / __mwd;
}

constexpr year_month_weekday operator/(const month_weekday& __mwd, int __y) noexcept
{
    return year(__y) / __mwd;
}

constexpr year_month_weekday_last operator/(const year_month& __ym, const weekday_last& __wdl) noexcept
{
    return {__ym.year(), __ym.month(), __wdl};
}

constexpr year_month_weekday_last operator/(const year& __y, const month_weekday_last& __mwdl) noexcept
{
    return {__y, __mwdl.month(), __mwdl.weekday_last()};
}

constexpr year_month_weekday_last operator/(int __y, const month_weekday_last& __mwdl) noexcept
{
    return year(__y) / __mwdl;
}

constexpr year_month_weekday_last operator/(const month_weekday_last& __mwdl, const year& __y) noexcept
{
    return __y / __mwdl;
}

constexpr year_month_weekday_last operator/(const month_weekday_last& __mwdl, int __y) noexcept
{
    return year(__y) / __mwdl;
}


// Not defined.
template <class _Duration> class time_of_day;

template <> class
time_of_day<hours>
{
public:

    using precision = chrono::hours;

    time_of_day() = default;

    constexpr explicit time_of_day(chrono::hours __since_midnight) noexcept
        : _M_hours(__since_midnight.count())
    {
    }

    constexpr chrono::hours hours() const noexcept
    {
        return (_M_hours < 0) ? chrono::hours(-_M_hours % 12) : chrono::hours(_M_hours);
    }

    constexpr explicit operator precision() const noexcept
    {
        return chrono::hours{__XVI_STD_UTILITY_NS::abs(_M_hours)};
    }

    constexpr precision to_duration() const noexcept
    {
        return precision{*this};
    }

    constexpr void make24() noexcept
    {
        _M_hours = __XVI_STD_UTILITY_NS::abs(_M_hours);
    }

    constexpr void make12() noexcept
    {
        _M_hours = -__XVI_STD_UTILITY_NS::abs(_M_hours);
    }

private:

    signed char _M_hours;
};

template <> class
time_of_day<minutes>
{
public:

    using precision = chrono::minutes;

    time_of_day() = default;

    constexpr explicit time_of_day(chrono::minutes __since_midnight) noexcept
        : _M_hours(chrono::hours(__since_midnight.count() / 60)),
          _M_mins(__since_midnight.count() % 60)
    {
    }

    constexpr chrono::hours hours() const noexcept
    {
        return _M_hours.hours();
    }

    constexpr chrono::minutes minutes() const noexcept
    {
        return chrono::minutes(_M_mins);
    }

    constexpr explicit operator precision() const noexcept
    {
        return chrono::minutes(_M_hours.to_duration()) + minutes();
    }

    constexpr precision to_duration() const noexcept
    {
        return precision{*this};
    }

    constexpr void make24() noexcept
    {
        _M_hours.make24();
    }

    constexpr void make12() noexcept
    {
        _M_hours.make12();
    }

private:

    time_of_day<chrono::hours> _M_hours;
    unsigned char              _M_mins;
};

template <> class
time_of_day<seconds>
{
public:

    using precision = chrono::seconds;

    time_of_day() = default;

    constexpr explicit time_of_day(chrono::seconds __since_midnight) noexcept
        : _M_mins(chrono::minutes(__since_midnight.count() / 60)),
          _M_secs(__since_midnight.count() % 60)
    {
    }

    constexpr chrono::hours hours() const noexcept
    {
        return _M_mins.hours();
    }

    constexpr chrono::minutes minutes() const noexcept
    {
        return _M_mins.minutes();
    }

    constexpr chrono::seconds seconds() const noexcept
    {
        return chrono::seconds(_M_secs);
    }

    constexpr explicit operator precision() const noexcept
    {
        return chrono::seconds(_M_mins.to_duration()) + seconds();
    }

    constexpr precision to_duration() const noexcept
    {
        return precision{*this};
    }

    constexpr void make24() noexcept
    {
        _M_mins.make24();
    }

    constexpr void make12() noexcept
    {
        _M_mins.make12();
    }

private:

    time_of_day<chrono::minutes> _M_mins;
    unsigned char                _M_secs;
};

template <class _Rep, class _Period>
class time_of_day<duration<_Rep, _Period>>
{
public:

    using precision = duration<_Rep, _Period>;

    time_of_day() = default;

    constexpr explicit time_of_day(precision __since_midnight) noexcept
        : _M_secs(duration_cast<chrono::seconds>(__since_midnight)),
          _M_frac(__since_midnight - duration_cast<chrono::seconds>(__since_midnight))
    {
    }

    constexpr chrono::hours hours() const noexcept
    {
        return _M_secs.hours();
    }

    constexpr chrono::minutes minutes() const noexcept
    {
        return _M_secs.minutes();
    }

    constexpr chrono::seconds seconds() const noexcept
    {
        return _M_secs.seconds();
    }

    constexpr precision subseconds() const noexcept
    {
        return precision(_M_frac);
    }

    constexpr explicit operator precision() const noexcept
    {
        return precision(_M_secs.to_duration()) + subseconds();
    }

    constexpr precision to_duration() const noexcept
    {
        return precision{*this};
    }

    constexpr void make24() noexcept
    {
        _M_secs.make24();
    }

    constexpr void make12() noexcept
    {
        _M_secs.make12();
    }

private:

    using __rep_t = conditional_t<(ratio_less_equal_v<precision, ratio<1, numeric_limits<uint8_t>::max()>>),
                                  uint8_t,
                                  conditional_t<(ratio_less_equal_v<precision, ratio<1, numeric_limits<uint16_t>::max()>>),
                                                uint16_t,
                                                conditional_t<(ratio_less_equal_v<precision, ratio<1, numeric_limits<uint32_t>::max()>>),
                                                              uint32_t,
                                                              uint64_t>>>;

    time_of_day<chrono::seconds> _M_secs;
    __rep_t                      _M_frac;
};

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const time_of_day<hours>& __t);

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const time_of_day<minutes>& __t);

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const time_of_day<seconds>& __t);

template <class _CharT, class _Traits, class _Rep, class _Period>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const time_of_day<duration<_Rep, _Period>>& __t);

} // namespace chrono


inline namespace literals
{
inline namespace chrono_literals
{

// GCC doesn't always detect this as a system header properly and throws warnings about these suffixes.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"

constexpr chrono::hours operator""h(unsigned long long __n)
{
    return chrono::hours(__n);
}

constexpr chrono::minutes operator""min(unsigned long long __n)
{
    return chrono::minutes(__n);
}

constexpr chrono::seconds operator""s(unsigned long long __n)
{
    return chrono::seconds(__n);
}

constexpr chrono::milliseconds operator""ms(unsigned long long __n)
{
    return chrono::milliseconds(__n);
}

constexpr chrono::microseconds operator""us(unsigned long long __n)
{
    return chrono::microseconds(__n);
}

constexpr chrono::nanoseconds operator""ns(unsigned long long __n)
{
    return chrono::nanoseconds(__n);
}

constexpr chrono::day operator""d(unsigned long long __n) noexcept
{
    return chrono::day(__n);
}

constexpr chrono::year operator""y(unsigned long long __n) noexcept
{
    return chrono::year(__n);
}

#pragma GCC diagnostic pop

} // namespace chrono_literals
} // namespace literals


namespace chrono
{

using namespace literals::chrono_literals;

}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_CHRONO_H */
