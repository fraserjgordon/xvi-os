#ifndef __SYSTEM_CXX_CHRONO_CHRONO_H
#define __SYSTEM_CXX_CHRONO_CHRONO_H


#include <System/C++/Utility/Chrono.hh>


namespace std::chrono
{


class system_clock
{
public:

    using rep           = int64_t;
    using period        = micro;
    using duration      = chrono::duration<rep, period>;
    using time_point    = chrono::time_point<system_clock>;

    static constexpr bool is_steady = false;

    static time_point now() noexcept;
};


class steady_clock
{
public:

    using rep           = uint64_t;
    using period        = nano;
    using duration      = chrono::duration<rep, period>;
    using time_point    = chrono::time_point<steady_clock>;

    static constexpr bool is_steady = true;

    static time_point now() noexcept;
};


class high_resolution_clock
{
public:

    using rep           = uint64_t;
    using period        = nano;
    using duration      = chrono::duration<rep, period>;
    using time_point    = chrono::time_point<steady_clock>;

    static constexpr bool is_steady = true;

    static time_point now() noexcept;
};


} // namespace std::chrono


#endif /* ifndef __SYSTEM_CXX_CHRONO_CHRONO_H */
