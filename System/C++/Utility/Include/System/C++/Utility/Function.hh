#pragma once
#ifndef __SYSTEM_CXX_UTILITY_FUNCTION_H
#define __SYSTEM_CXX_UTILITY_FUNCTION_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/TypeInfo.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>


#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/InPlace.hh>
#include <System/C++/Utility/ReferenceWrapper.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class> class function;
template <class...> class move_only_function;


class bad_function_call : public exception
{
public:

    constexpr bad_function_call() noexcept = default;
    const char* what() const noexcept override
        { return "bad function call"; }
};


namespace __detail
{

// Forward declarations.
template <class, class, class...> class __function_utils;

inline constexpr size_t __function_obj_size = 4*sizeof(void*);
inline constexpr size_t __function_inline_size = __function_obj_size - sizeof(void*);

// Dummy type used to represent the empty state.
template <class _R, class... _Args>
struct __empty_function
{
    _R operator()(_Args&&...)
    {
        __XVI_CXX_UTILITY_THROW(bad_function_call());
    }
};

struct __function_storage_t
{
    union
    {
        byte    __raw[__function_inline_size];
        void*   __external;
    };
};

template <class> class __function_utils_base;

template <class _R, class... _Args>
class __function_utils_base<_R(_Args...)>
{
public:

    virtual void __duplicate_utils_to(void*) const = 0;

    virtual void __copy_construct_to(__function_storage_t&, const __function_storage_t&) const = 0;
    virtual void __move_construct_to(__function_storage_t&, __function_storage_t&) const = 0;
    virtual void __destroy(__function_storage_t&) const = 0;
    virtual const type_info& __get_type() const = 0;
    virtual void* __ptr(__function_storage_t&) const = 0;
    virtual const void* __ptr(const __function_storage_t&) const = 0;

    ~__function_utils_base() = default;

    virtual _R __call(__function_storage_t&, _Args&&... __args) const = 0;

    template <class _T, class... _EArgs>
    void __emplace(__function_storage_t& __s, _EArgs&&... __args) const
    {
        // Cast allows avoidance of virtual method calls.
        auto __this = static_cast<const __function_utils<_T, _R, _Args...>*>(this);
        
        __this->__prepare(__s);
        new (__this->__ptr(__s)) _T(std::forward<_EArgs>(__args)...);
    }

protected:

    virtual void __prepare(__function_storage_t&) const = 0;
    virtual void __cleanup(__function_storage_t&) const = 0;
};

template <class _T,
          class _Signature,
          bool _StoreExternally = (sizeof(_T) > __function_inline_size)
                                  || (alignof(_T) > alignof(void*))
                                  || !std::is_nothrow_move_constructible_v<_T>>
class __function_utils_ptr : public __function_utils_base<_Signature>
{
public:

    void* __ptr(__function_storage_t& __s) const final
    {
        return __s.__raw;
    }

    const void* __ptr(const __function_storage_t& __s) const final
    {
        return __s.__raw;
    }

protected:

    void __prepare(__function_storage_t&) const final
    {
    }

    void __cleanup(__function_storage_t&) const final
    {
    }
};

template <class _T, class _Signature>
class __function_utils_ptr<_T, _Signature, true> : public __function_utils_base<_Signature>
{
public:

    void* __ptr(__function_storage_t& __s) const final
    {
        return __s.__external;
    }

    const void* __ptr(const __function_storage_t& __s) const final
    {
        return __s.__external;
    }

protected:

    void __prepare(__function_storage_t& __s) const final
    {
        __s.__external = operator new(sizeof(_T), align_val_t{alignof(_T)});
    }

    void __cleanup(__function_storage_t& __s) const final
    {
        operator delete(__s.__external, align_val_t{alignof(_T)});
    }
};

template <class _T, class _R, class... _Args>
class __function_utils final : public __function_utils_ptr<_T, _R(_Args...)>
{
public:

    using _Super = __function_utils_ptr<_T, _R(_Args...)>;
    using _Super::__cleanup;
    using _Super::__prepare;
    using _Super::__ptr;

    void __duplicate_utils_to(void* __to) const final
    {
        reinterpret_cast<__function_utils_base<_R(_Args...)>*>(__to)->~__function_utils_base();
        new (__to) __function_utils<_T, _R, _Args...>();
    }

    void __copy_construct_to(__function_storage_t& __to, const __function_storage_t& __from) const final
    {
        __prepare(__to);
        new (__ptr(__to)) _T(*reinterpret_cast<const _T*>(__ptr(__from)));
    }

    void __move_construct_to(__function_storage_t& __to, __function_storage_t& __from) const final
    {
        __prepare(__to);
        new (__ptr(__to)) _T(std::move(*reinterpret_cast<_T*>(__ptr(__from))));
    }

    void __destroy(__function_storage_t& __s) const final
    {
        reinterpret_cast<_T*>(__ptr(__s))->~_T();
        __cleanup(__s);
    }

    const type_info& __get_type() const final
    {
        if constexpr (is_same_v<_T, __empty_function<_R, _Args...>>)
            return typeid(void);
        else
            return typeid(_T);
    }

    _R __call(__function_storage_t& __s, _Args&&... __args) const final
    {
        return __XVI_STD_UTILITY_NS::invoke_r<_R>(*reinterpret_cast<_T*>(__ptr(__s)), std::forward<_Args>(__args)...);
    }
};


template <class> struct __mem_fn_to_plain_fn {};
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) &> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) const &> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) volatile &> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) const volatile &> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) & noexcept> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) const & noexcept> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) volatile & noexcept> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) const volatile & noexcept> { using type = _R(_Args...); };


} // namespace __detail


// Not defined in general. Only specialised for function types.
template <class> class function;


template <class _R, class... _ArgTypes>
class function<_R(_ArgTypes...)>
{
public:

    using result_type = _R;

    function() noexcept
        : _M_utils_storage {},
          _M_data_storage {}
    {
        __construct_utils<__detail::__empty_function<_R, _ArgTypes...>>();
    }

    function(nullptr_t) noexcept
        : function()
    {
    }

    function(const function& __rhs)
        : function()
    {
        __rhs.__get_utils()->__copy_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
    }

    function(function&& __rhs) noexcept
        : function()
    {
        __rhs.__get_utils()->__move_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
    }

    template <class _F>
        requires (!std::is_same_v<std::remove_cvref_t<_F>, function>)
            && requires { invoke_r<_R>(declval<std::decay_t<_F>&>(), declval<_ArgTypes>()...); }
    function(_F __f)
        : function()
    {
        auto __utils = __detail::__function_utils<remove_cvref_t<_F>, _R, _ArgTypes...>();
        __utils.template __emplace<remove_cvref_t<_F>>(_M_data_storage, std::move(__f));
        __construct_utils<remove_cvref_t<_F>>();
    }

    function& operator=(const function& __rhs)
    {
        if (&__rhs == this)
            return *this;

        __get_utils()->__destroy(_M_data_storage);
        __rhs.__get_utils()->__copy_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
        return *this;
    }

    function& operator=(function&& __rhs)
    {
        if (&__rhs == this) [[unlikely]]
            return *this;

        __get_utils()->__destroy(_M_data_storage);
        __rhs.__get_utils()->__move_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
        return *this;
    }

    function& operator=(nullptr_t) noexcept
    {
        return operator&=(function());
    }

    template <class _F>
        requires std::is_invocable_v<std::decay_t<_F>, _ArgTypes...>
    function& operator=(_F&& __f)
    {
        function(std::forward<_F>(__f).swap(*this));
        return *this;
    }

    template <class _F>
    function& operator=(reference_wrapper<_F> __f) noexcept
    {
        function(__f).swap(*this);
        return *this;
    }

    ~function()
    {
        __get_utils()->__destroy(_M_data_storage);
        __set_utils<__detail::__empty_function<_R, _ArgTypes...>>();
    }

    void swap(function& __rhs) noexcept
    {
        auto __temp = std::move(__rhs);
        __rhs = std::move(*this);
        *this = std::move(__temp);
    }

    explicit operator bool() const noexcept
    {
        return __get_utils()->__get_type() != typeid(__detail::__empty_function<_R, _ArgTypes...>);
    }

    _R operator()(_ArgTypes... __args) const
    {
        if (!bool(*this))
            __XVI_CXX_UTILITY_THROW(bad_function_call());

        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    const type_info& target_type() const noexcept
    {
        if (!bool(*this))
            return typeid(void);

        return __get_utils()->__get_type();
    }

    template <class _T> _T* target() noexcept
    {
        if (target_type() != typeid(_T))
            return nullptr;

        return reinterpret_cast<_T*>(__get_utils()->__ptr(_M_data_storage));
    }

    template <class _T> const _T* target() const noexcept
    {
        if (target_type() != typeid(_T))
            return nullptr;

        return reinterpret_cast<const _T*>(__get_utils()->__ptr(_M_data_storage));
    }

private:

    using __storage_t = __detail::__function_storage_t;
    using __utils_t = aligned_storage_t<sizeof(__detail::__function_utils<int, _R, _ArgTypes...>), alignof(__detail::__function_utils<int, _R, _ArgTypes...>)>;

    __utils_t   _M_utils_storage;
    __storage_t _M_data_storage;

    const __detail::__function_utils_base<_R(_ArgTypes...)>* __get_utils() const
    {
        return reinterpret_cast<const __detail::__function_utils_base<_R(_ArgTypes...)>*>(&_M_utils_storage);
    }

    template <class _T> void __set_utils()
    {
        auto __utils = reinterpret_cast<__detail::__function_utils_base<_R(_ArgTypes...)>*>(&_M_utils_storage);
        __utils->~__function_utils_base();
        new (__utils) __detail::__function_utils<_T, _R, _ArgTypes...>();
    }

    template <class _T> constexpr void __construct_utils()
    {
        static_assert(sizeof(__detail::__function_utils<_T, _R, _ArgTypes...>) == sizeof(void*));

        new(&_M_utils_storage) __detail::__function_utils<_T, _R, _ArgTypes...>();
    }
};

template <class _R, class... _ArgTypes>
function(_R(*)(_ArgTypes...)) -> function<_R(_ArgTypes...)>;

template <class _F>
    requires requires { &_F::operator(); }
        && std::is_member_function_pointer_v<decltype(&_F::operator())>
function(_F) -> function<typename __detail::__mem_fn_to_plain_fn<decltype(&_F::operator())>::type>;


template <class _R, class... _ArgTypes>
bool operator==(const function<_R(_ArgTypes...)>& __f, nullptr_t) noexcept
{
    return !__f;
}


template <class _R, class... _ArgTypes>
void swap(function<_R(_ArgTypes...)>& __l, function<_R(_ArgTypes...)>& __r) noexcept
{
    __l.swap(__r);
}


namespace __detail
{


template <class> struct __is_move_only_function_specialization : false_type {};
template <class... _T> struct __is_move_only_function_specialization<move_only_function<_T...>> : true_type {};

template <class _T> inline constexpr bool __is_move_only_function_specialization_v = __is_move_only_function_specialization<_T>::value;


template <class> class __move_only_function_utils_base;
template <class, class, class...> class __move_only_function_utils;

template <class _R, class... _Args>
class __move_only_function_utils_base<_R(_Args...)>
{
public:

    virtual void __move_utils_to(void*) const = 0;

    virtual void __move_construct_to(__function_storage_t&, __function_storage_t&) const = 0;
    virtual void __destroy(__function_storage_t&) const = 0;
    virtual void* __ptr(__function_storage_t&) const = 0;
    virtual const void* __ptr(const __function_storage_t&) const = 0;
    virtual bool __valid() const = 0;

    ~__move_only_function_utils_base() = default;

    virtual _R __call(__function_storage_t&, _Args&&... __args) const = 0;

    template <class _T, class... _EArgs>
    void __emplace(__function_storage_t& __s, _EArgs&&... __args) const
    {
        // Cast allows avoidance of virtual method calls.
        auto __this = static_cast<const __move_only_function_utils<_T, _R, _Args...>*>(this);
        
        __this->__prepare(__s);
        new (__this->__ptr(__s)) _T(std::forward<_EArgs>(__args)...);
    }

protected:

    virtual void __prepare(__function_storage_t&) const = 0;
    virtual void __cleanup(__function_storage_t&) const = 0;
};

template <class _T,
          class _Signature,
          bool _StoreExternally = (sizeof(_T) > __function_inline_size)
                                  || (alignof(_T) > alignof(void*))
                                  || !std::is_nothrow_move_constructible_v<_T>>
class __move_only_function_utils_ptr : public __move_only_function_utils_base<_Signature>
{
public:

    void* __ptr(__function_storage_t& __s) const final
    {
        return __s.__raw;
    }

    const void* __ptr(const __function_storage_t& __s) const final
    {
        return __s.__raw;
    }

protected:

    void __prepare(__function_storage_t&) const final
    {
    }

    void __cleanup(__function_storage_t&) const final
    {
    }
};

template <class _T, class _Signature>
class __move_only_function_utils_ptr<_T, _Signature, true> : public __move_only_function_utils_base<_Signature>
{
public:

    void* __ptr(__function_storage_t& __s) const final
    {
        return __s.__external;
    }

    const void* __ptr(const __function_storage_t& __s) const final
    {
        return __s.__external;
    }

protected:

    void __prepare(__function_storage_t& __s) const final
    {
        __s.__external = operator new(sizeof(_T), align_val_t{alignof(_T)});
    }

    void __cleanup(__function_storage_t& __s) const final
    {
        operator delete(__s.__external, align_val_t{alignof(_T)});
    }
};

template <class _T, class _R, class... _Args>
class __move_only_function_utils final : public __move_only_function_utils_ptr<_T, _R(_Args...)>
{
public:

    using _Super = __move_only_function_utils_ptr<_T, _R(_Args...)>;
    using _Super::__cleanup;
    using _Super::__prepare;
    using _Super::__ptr;

    void __move_utils_to(void* __to) const final
    {
        // Silence a warning about a non-virtual destructor on __function_utils_base.
        reinterpret_cast<__move_only_function_utils_base<_R(_Args...)>*>(__to)->~__move_only_function_utils_base();
        new (__to) __move_only_function_utils<_T, _R, _Args...>();
    }

    void __move_construct_to(__function_storage_t& __to, __function_storage_t& __from) const final
    {
        __prepare(__to);
        new (__ptr(__to)) _T(std::move(*reinterpret_cast<_T*>(__ptr(__from))));
    }

    void __destroy(__function_storage_t& __s) const final
    {
        reinterpret_cast<_T*>(__ptr(__s))->~_T();
        __cleanup(__s);
    }

    bool __valid() const final
    {
        return std::is_same_v<_T, __empty_function>;
    }

    _R __call(__function_storage_t& __s, _Args&&... __args) const final
    {
        return __XVI_STD_UTILITY_NS::invoke_r<_R>(*reinterpret_cast<_T*>(__ptr(__s)), std::forward<_Args>(__args)...);
    }
};


template <bool _Const, bool _Noexcept, class _RefType, class _R, class... _ArgTypes>
class __move_only_function_base
{

    template <class _T> using _ApplyCV = std::conditional_t<_Const, std::add_const_t<_T>, _T>;
    template <class _T> using _InvQuals = std::conditional_t<std::is_rvalue_reference_v<_RefType>, _ApplyCV<_T>&&, _ApplyCV<_T>&>;
    template <class _T> using _CVRef = std::conditional_t<!std::is_reference_v<_RefType>, _ApplyCV<_T>, _InvQuals<_T>>;

    template <class _VT>
    static constexpr bool __is_callable_from = _Noexcept
        ? std::is_nothrow_invocable_r_v<_R, _CVRef<_VT>, _ArgTypes...> && std::is_nothrow_invocable_r_v<_R, _InvQuals<_VT>, _ArgTypes...>
        : std::is_invocable_r_v<_R, _CVRef<_VT>, _ArgTypes...> && std::is_invocable_r_v<_R, _InvQuals<_VT>, _ArgTypes...>;

public:

    using result_type = _R;

    __move_only_function_base() noexcept :
        _M_utils_storage{},
        _M_data_storage{}
    {
        __construct_utils<__detail::__empty_function<_R, _ArgTypes...>>();
    }

    __move_only_function_base(std::nullptr_t) noexcept :
        __move_only_function_base()
    {
    }

    __move_only_function_base(__move_only_function_base&& __rhs) noexcept :
        __move_only_function_base()
    {
        __rhs.__get_utils()->__move_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__move_utils_to(&_M_utils_storage);
    }

    template <class _F>
        requires (!std::is_same_v<std::remove_cvref_t<_F>, __move_only_function_base>)
            && (!__detail::__is_in_place_type_specialization_v<std::remove_cvref_t<_F>>)
            && __is_callable_from<std::decay_t<_F>>
    __move_only_function_base(_F&& __f) :
        __move_only_function_base()
    {
        if constexpr (std::is_pointer_v<std::remove_cvref_t<_F>> && std::is_function_v<std::remove_pointer_t<std::remove_cvref_t<_F>>>)
        {
            // Null function pointer => empty.
            if (__f == nullptr)
                return;

            // Otherwise, construct normally.
        }
        else if constexpr (std::is_member_function_pointer_v<std::remove_cvref_t<_F>>)
        {
            // Null member function pointer => empty.
            if (__f == nullptr)
                return;

            // Otherwise, construct normally.
        }
        else if constexpr (__is_move_only_function_specialization_v<std::remove_cvref_t<_F>>)
        {
            if (!__f)
                return;

            // Otherwise, construct normally.
        }

        auto __utils = __detail::__move_only_function_utils<std::remove_cvref_t<_F>, _R, _ArgTypes...>();
        __utils.template __emplace<std::remove_cvref_t<_F>>(_M_data_storage, std::move(__f));
        __construct_utils<std::remove_cvref_t<_F>>();
    }

    template <class _T, class... _Args>
        requires std::is_constructible_v<std::decay_t<_T>, _Args...>
            && __is_callable_from<std::decay_t<_T>>
    explicit __move_only_function_base(in_place_type_t<_T>, _Args&&... __args)
    {
        using _VT = std::decay_t<_T>;
        static_assert(std::is_same_v<_T, _VT>);

        auto __utils = __detail::__move_only_function_utils<_VT, _R, _ArgTypes...>();
        __utils.template __emplace<_VT>(_M_data_storage, std::forward<_Args>(__args)...);
        __construct_utils<_VT>();
    }

    template <class _T, class _U, class... _Args>
        requires std::is_constructible_v<std::decay_t<_T>, std::initializer_list<_U>&, _Args...>
            && __is_callable_from<std::decay_t<_T>>
    explicit __move_only_function_base(in_place_type_t<_T>, std::initializer_list<_U> __il, _Args&&... __args)
    {
        using _VT = std::decay_t<_T>;
        static_assert(std::is_same_v<_T, _VT>);

        auto __utils = __detail::__move_only_function_utils<_VT, _R, _ArgTypes...>();
        __utils.template __emplace<_VT>(_M_data_storage, __il, std::forward<_Args>(__args)...);
        __construct_utils<_VT>();
    }

    __move_only_function_base& operator=(__move_only_function_base&& __rhs)
    {
        if (&__rhs == this) [[unlikely]]
            return *this;

        __get_utils()->__destroy(_M_data_storage);
        __rhs.__get_utils()->__move_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__move_utils_to(&_M_utils_storage);
        return *this;
    }

    __move_only_function_base& operator=(std::nullptr_t)
    {
        __get_utils()->__destroy(_M_data_storage);
        __set_utils<__detail::__empty_function<_R, _ArgTypes...>>();

        return *this;
    }

    template <class _F>
    __move_only_function_base& operator=(_F&& __f)
    {
        move_only_function(std::forward<_F>(__f)).swap(*this);
    }

    ~__move_only_function_base()
    {
        __get_utils()->__destroy(_M_data_storage);
        __set_utils<__detail::__empty_function<_R, _ArgTypes...>>();
    }

    explicit operator bool() const noexcept
    {
        return __get_utils()->__valid();
    }

    void swap(__move_only_function_base& __rhs) noexcept
    {
        auto __temp = std::move(__rhs);
        __rhs = std::move(*this);
        *this = std::move(__temp);
    }

    _R operator()(_ArgTypes... __args)
        requires (!_Const && !std::is_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const
        requires (_Const && !std::is_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) &
        requires (!_Const && std::is_lvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const &
        requires (_Const && std::is_lvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) &&
        requires (!_Const && std::is_rvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const &&
        requires (_Const && std::is_rvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) noexcept
        requires (!_Const && !std::is_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const noexcept
        requires (_Const && !std::is_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) & noexcept
        requires (!_Const && std::is_lvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const & noexcept
        requires (_Const && std::is_lvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) && noexcept
        requires (!_Const && std::is_rvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const && noexcept
        requires (_Const && std::is_rvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), std::forward<_ArgTypes>(__args)...);
    }

private:
    
    using __storage_t = __detail::__function_storage_t;
    using __utils_t = aligned_storage_t<sizeof(__detail::__move_only_function_utils<int, _R, _ArgTypes...>), alignof(__detail::__move_only_function_utils<int, _R, _ArgTypes...>)>;

    __utils_t   _M_utils_storage;
    __storage_t _M_data_storage;

    const __detail::__move_only_function_utils_base<_R(_ArgTypes...)>* __get_utils() const
    {
        return reinterpret_cast<const __detail::__move_only_function_utils_base<_R(_ArgTypes...)>*>(&_M_utils_storage);
    }

    template <class _T> void __set_utils()
    {
        auto __utils = reinterpret_cast<__detail::__move_only_function_utils_base<_R(_ArgTypes...)>*>(&_M_utils_storage);
        __utils->~__move_only_function_utils_base();
        new (__utils) __detail::__move_only_function_utils<_T, _R, _ArgTypes...>();
    }

    template <class _T> constexpr void __construct_utils()
    {
        static_assert(sizeof(__detail::__move_only_function_utils<_T, _R, _ArgTypes...>) == sizeof(void*));

        new (&_M_utils_storage) __detail::__move_only_function_utils<_T, _R, _ArgTypes...>();
    }
};


} // namespace __detail


template <class...> class move_only_function;

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) noexcept(false)>
    : __detail::__move_only_function_base<false, false, int, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<false, false, int, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) const noexcept(false)>
    : __detail::__move_only_function_base<true, false, int, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, false, int, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) & noexcept(false)>
    : __detail::__move_only_function_base<false, false, int&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<false, false, int&, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) const & noexcept(false)>
    : __detail::__move_only_function_base<true, false, int&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, false, int&, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) && noexcept(false)>
    : __detail::__move_only_function_base<false, false, int&&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<false, false, int&&, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) const && noexcept(false)>
    : __detail::__move_only_function_base<true, false, int&&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, false, int&&, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) noexcept(true)>
    : __detail::__move_only_function_base<false, true, int, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, false, int, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) const noexcept(true)>
    : __detail::__move_only_function_base<true, true, int, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, true, int, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) & noexcept(true)>
    : __detail::__move_only_function_base<false, true, int&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<false, true, int&, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) const & noexcept(true)>
    : __detail::__move_only_function_base<true, true, int&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, true, int&, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) && noexcept(true)>
    : __detail::__move_only_function_base<false, true, int&&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, false, int&&, _R, _ArgTypes...>::__move_only_function_base; };

template <class _R, class... _ArgTypes>
class move_only_function<_R(_ArgTypes...) const && noexcept(true)>
    : __detail::__move_only_function_base<true, true, int&&, _R, _ArgTypes...>
        { using __detail::__move_only_function_base<true, true, int&&, _R, _ArgTypes...>::__move_only_function_base; };


template <class... _T>
void swap(move_only_function<_T...>& __x, move_only_function<_T...>& __y)
{
    return __x.swap(__y);
}


template <class... _T>
bool operator==(const move_only_function<_T...>& __x, std::nullptr_t)
{
    return bool(__x);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_FUNCTION_H */
