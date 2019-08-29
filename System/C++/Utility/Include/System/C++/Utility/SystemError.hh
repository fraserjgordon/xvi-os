#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SYSTEMERROR_H
#define __SYSTEM_CXX_UTILITY_SYSTEMERROR_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/ErrorCodes.hh>
#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/StdExcept.hh>
#include <System/C++/Utility/String.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
class error_category;
class error_code;
class error_condition;


//! @TODO: provide these in a different library (C++/Runtime?)
const error_category& generic_category() noexcept;
const error_category& system_category() noexcept;


template <class _T> struct is_error_code_enum : false_type {};
template <class _T> struct is_error_condition_enum : false_type {};

template <class _T> inline constexpr bool is_error_code_enum_v = is_error_code_enum<_T>::value;
template <class _T> inline constexpr bool is_error_condition_enum_v = is_error_condition_enum<_T>::value;


class error_condition
{
public:

    error_condition() noexcept
        : error_condition(0, generic_category())
    {
    }

    error_condition(int __val, const error_category& __cat) noexcept
        : _M_val(__val),
          _M_category(&__cat)
    {
    }

    template <class _ErrorConditionEnum,
              class = enable_if_t<is_error_condition_enum_v<_ErrorConditionEnum>, void>>
    error_condition(_ErrorConditionEnum __e) noexcept
    {
        auto __cond = make_error_condition(__e);
        _M_val = __cond._M_val;
        _M_category = __cond._M_category;
    }

    void assign(int __val, const error_category& __cat) noexcept
    {
        _M_val = __val;
        _M_category = &__cat;
    }

    template <class _ErrorConditionEnum,
              class = enable_if_t<is_error_condition_enum_v<_ErrorConditionEnum>, void>>
    error_condition& operator=(_ErrorConditionEnum __e) noexcept
    {
        auto __cond = make_error_condition(__e);
        _M_val = __cond._M_val;
        _M_category = __cond._M_category;
        return *this;
    }

    void clear() noexcept
    {
        _M_val = 0;
        _M_category = &generic_category();
    }

    int value() const noexcept
    {
        return _M_val;
    }

    const error_category& category() const noexcept
    {
        return *_M_category;
    }

    //! @TODO: define elsewhere (C++/Runtime?)
    string message() const;

    explicit operator bool() const noexcept
    {
        return value() != 0;
    }

private:

    int _M_val;
    const error_category* _M_category;
};


class error_code
{
public:

    error_code() noexcept
        : error_code(0, system_category())
    {
    }

    error_code(int __val, const error_category& __cat) noexcept
        : _M_val(__val),
          _M_category(&__cat)
    {
    }

    template <class _ErrorCodeEnum,
              class = enable_if_t<is_error_code_enum_v<_ErrorCodeEnum>, void>>
    error_code(_ErrorCodeEnum __e) noexcept
    {
        auto __code = make_error_code(__e);
        _M_val = __code._M_val;
        _M_category = __code._M_category;
    }

    void assign(int __val, const error_category& __cat) noexcept
    {
        _M_val = __val;
        _M_category = &__cat;
    }

    template <class _ErrorCodeEnum,
              class = enable_if_t<is_error_code_enum_v<_ErrorCodeEnum>, void>>
    error_code& operator=(_ErrorCodeEnum __e) noexcept
    {
        auto __code = make_error_code(__e);
        _M_val = __code._M_val;
        _M_category = __code._M_category;
        return *this;
    }

    void clear() noexcept
    {
        _M_val = 0;
        _M_category = &system_category();
    }

    int value() const noexcept
    {
        return _M_val;
    }

    const error_category& category() const noexcept
    {
        return *_M_category;
    }

    // Defined later due to a mutual dependence between error_code and error_category.
    error_condition default_error_condition() const noexcept;

    //! @TODO: defined elsewhere (C++/Runtime?)
    string message() const;

    explicit operator bool() const noexcept
    {
        return value() != 0;
    }

private:

    int _M_val;
    const error_category* _M_category;
};


class error_category
{
public:

    constexpr error_category() noexcept = default;

    virtual ~error_category() = default;

    error_category(const error_category&) = delete;
    error_category& operator=(const error_category&) = delete;

    virtual const char* name() const noexcept = 0;

    virtual error_condition default_error_condition(int __ev) const noexcept
    {
        return error_condition(__ev, *this);
    }

    // Defined later due to a mutual dependency between error_category and operator== for error_condition.
    virtual bool equivalent(int __code, const error_condition& __condition) const noexcept;

    virtual bool equivalent(const error_code& __code, int __condition) const noexcept
    {
        return *this == __code.category() && __code.value() == __condition;
    }

    virtual string message(int) const = 0;

    bool operator==(const error_category& __rhs) const noexcept
    {
        return this == &__rhs;
    }

    bool operator!=(const error_category& __rhs) const noexcept
    {
        return !(*this == __rhs);
    }

    bool operator< (const error_category& __rhs) const noexcept
    {
        return less<const error_category*>()(this, &__rhs);
    }
};


inline error_condition error_code::default_error_condition() const noexcept
{
    return category().default_error_condition(value());
}


enum class errc
{
    address_family_not_supported        = EAFNOSUPPORT,
    address_in_use                      = EADDRINUSE,
    address_not_available               = EADDRNOTAVAIL,
    already_connected                   = EISCONN,
    argument_list_too_long              = E2BIG,
    argument_out_of_domain              = EDOM,
    bad_address                         = EFAULT,
    bad_file_descriptor                 = EBADF,
    bad_message                         = EBADMSG,
    broken_pipe                         = EPIPE,
    connection_aborted                  = ECONNABORTED,
    connection_already_in_progress      = EALREADY,
    connection_refused                  = ECONNREFUSED,
    connection_reset                    = ECONNRESET,
    cross_device_link                   = EXDEV,
    destination_address_required        = EDESTADDRREQ,
    device_or_resource_busy             = EBUSY,
    directory_not_empty                 = ENOTEMPTY,
    executable_format_error             = ENOEXEC,
    file_exists                         = EEXIST,
    file_too_large                      = EFBIG,
    filename_too_long                   = ENAMETOOLONG,
    function_not_supported              = ENOSYS,
    host_unreachable                    = EHOSTUNREACH,
    identifier_removed                  = EIDRM,
    illegal_byte_sequence               = EILSEQ,
    inappropriate_io_control_operation  = ENOTTY,
    interrupted                         = EINTR,
    invalid_argument                    = EINVAL,
    invalid_seek                        = ESPIPE,
    io_error                            = EIO,
    is_a_directory                      = EISDIR,
    message_size                        = EMSGSIZE,
    network_down                        = ENETDOWN,
    network_reset                       = ENETRESET,
    network_unreachable                 = ENETUNREACH,
    no_buffer_space                     = ENOBUFS,
    no_child_process                    = ECHILD,
    no_link                             = ENOLINK,
    no_lock_available                   = ENOLCK,
    no_message_available                = ENODATA,
    no_message                          = ENOMSG,
    no_protocol_option                  = ENOPROTOOPT,
    no_space_on_device                  = ENOSPC,
    no_stream_resources                 = ENOSR,
    no_such_device_or_address           = ENODEV,
    no_such_file_or_directory           = ENOENT,
    no_such_process                     = ESRCH,
    not_a_directory                     = ENOTDIR,
    not_a_socket                        = ENOTSOCK,
    not_a_stream                        = ENOSTR,
    not_connected                       = ENOTCONN,
    not_enough_memory                   = ENOMEM,
    not_supported                       = ENOTSUP,
    operation_canceled                  = ECANCELED,
    operation_in_progress               = EINPROGRESS,
    operation_not_permitted             = EPERM,
    operation_not_supported             = EOPNOTSUPP,
    operation_would_block               = EWOULDBLOCK,
    owner_dead                          = EOWNERDEAD,
    permission_denied                   = EACCES,
    protocol_error                      = EPROTO,
    protocol_not_supported              = EPROTONOSUPPORT,
    read_only_file_system               = EROFS,
    resource_deadlock_would_occur       = EDEADLK,
    resource_unavailable_try_again      = EAGAIN,
    result_out_of_range                 = ERANGE,
    state_not_recoverable               = ENOTRECOVERABLE,
    stream_timeout                      = ETIME,
    text_file_busy                      = ETXTBUSY,
    timed_out                           = ETIMEDOUT,
    too_many_files_open_in_system       = ENFILE,
    too_many_files_open                 = EMFILE,
    too_many_links                      = EMLINK,
    too_many_symbolic_link_levels       = ELOOP,
    value_too_large                     = EOVERFLOW,
    wrong_protocol_type                 = EPROTOTYPE,
};


template <> struct is_error_condition_enum<errc> : true_type {};


template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const error_code& __ec)
{
    __os << __ec.category().name() << ':' << __ec.value();
    return __os;
}


error_condition make_error_condition(errc __c) noexcept
{
    return error_condition(static_cast<int>(__c), generic_category());
}

error_code make_error_code(errc __c) noexcept
{
    return error_code(static_cast<int>(__c), generic_category());
}


bool operator==(const error_code& __lhs, const error_code& __rhs) noexcept
{
    return __lhs.category() == __rhs.category() && __lhs.value() == __rhs.value();
}

bool operator==(const error_code& __lhs, const error_condition& __rhs) noexcept
{
    return __lhs.category().equivalent(__lhs.value(), __rhs) || __rhs.category().equivalent(__lhs, __rhs.value());
}

bool operator==(const error_condition& __lhs, const error_code& __rhs) noexcept
{
    return __rhs.category().equivalent(__rhs.value(), __lhs) || __lhs.category().equivalent(__rhs, __lhs.value());
}

bool operator==(const error_condition& __lhs, const error_condition& __rhs) noexcept
{
    return __lhs.category() == __rhs.category() && __lhs.value() == __rhs.value();
}

bool operator!=(const error_code& __lhs, const error_code& __rhs) noexcept
{
    return !(__lhs == __rhs);
}

bool operator!=(const error_code& __lhs, const error_condition& __rhs) noexcept
{
    return !(__lhs == __rhs);
}

bool operator!=(const error_condition& __lhs, const error_code& __rhs) noexcept
{
    return !(__lhs == __rhs);
}

bool operator!=(const error_condition& __lhs, const error_condition& __rhs) noexcept
{
    return !(__lhs == __rhs);
}

bool operator<(const error_code& __lhs, const error_code& __rhs) noexcept
{
    return __lhs.category() < __rhs.category() || (__lhs.category() == __rhs.category() && __lhs.value() < __rhs.value());
}

bool operator<(const error_condition& __lhs, const error_condition& __rhs) noexcept
{
    return __lhs.category() < __rhs.category() || (__lhs.category() == __rhs.category() && __lhs.value() < __rhs.value());
}


inline bool error_category::equivalent(int __code, const error_condition& __condition) const noexcept
{
    return default_error_condition(__code) == __condition;
}


//! @TODO: implement.
template <class> struct hash;
template <> struct hash<error_code>;
template <> struct hash<error_condition>;


class system_error : public runtime_error
{
public:

    system_error(error_code __ec, const string& __what)
        : runtime_error(__detail::__make_exception_string(__what, ": ", __ec.message())),
          _M_ec(__ec)
    {
    }

    system_error(error_code __ec, const char* __what)
        : runtime_error(__detail::__make_exception_string(__what, ": ", __ec.message())),
          _M_ec(__ec)
    {
    }

    system_error(error_code __ec)
        : system_error(__ec, "system_error: ")
    {
    }

    system_error(int __ev, const error_category& __cat, const string& __what)
        : system_error(error_code(__ev, __cat), __what)
    {
    }

    system_error(int __ev, const error_category& __cat, const char* __what)
        : system_error(error_code(__ev, __cat), __what)
    {
    }

    system_error(int __ev, const error_category& __cat)
        : system_error(error_code(__ev, __cat))
    {
    }

    const error_code& code() const noexcept
    {
        return _M_ec;
    }

    const char* what() const noexcept override
    {
        return runtime_error::what();
    }

private:

    error_code _M_ec;
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SYSTEMERROR_H */
