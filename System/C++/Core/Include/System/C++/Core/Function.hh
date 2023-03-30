#ifndef __SYSTEM_CXX_CORE_FUNCTION_H
#define __SYSTEM_CXX_CORE_FUNCTION_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/TypeInfo.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/Invoke.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class> class function;


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
        throw bad_function_call();
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
        new (__this->__ptr(__s)) _T(__XVI_STD_NS::forward<_EArgs>(__args)...);
    }

protected:

    virtual void __prepare(__function_storage_t&) const = 0;
    virtual void __cleanup(__function_storage_t&) const = 0;
};

template <class _T,
          class _Signature,
          bool _StoreExternally = (sizeof(_T) > __function_inline_size)
                                  || (alignof(_T) > alignof(void*))
                                  || !is_nothrow_move_constructible_v<_T>>
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
        __s.__external = operator new(sizeof(_T), std::align_val_t{alignof(_T)});
    }

    void __cleanup(__function_storage_t& __s) const final
    {
        operator delete(__s.__external, std::align_val_t{alignof(_T)});
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
        new (__ptr(__to)) _T(__XVI_STD_NS::move(*reinterpret_cast<_T*>(__ptr(__from))));
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
        return __XVI_STD_NS::invoke_r<_R>(*reinterpret_cast<_T*>(__ptr(__s)), __XVI_STD_NS::forward<_Args>(__args)...);
    }
};


template <class> struct __mem_fn_to_plain_fn {};
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...)> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) const> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) volatile> { using type = _R(_Args...); };
template <class _R, class _C, class... _Args> struct __mem_fn_to_plain_fn<_R(_C::*)(_Args...) const volatile> { using type = _R(_Args...); };
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
        requires (!is_same_v<remove_cvref_t<_F>, function>)
            && requires { __XVI_STD_NS::invoke_r<_R>(declval<decay_t<_F>&>(), declval<_ArgTypes>()...); }
    function(_F __f)
        : function()
    {
        auto __utils = __detail::__function_utils<remove_cvref_t<_F>, _R, _ArgTypes...>();
        __utils.template __emplace<remove_cvref_t<_F>>(_M_data_storage, __XVI_STD_NS::move(__f));
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
        requires is_invocable_v<decay_t<_F>, _ArgTypes...>
    function& operator=(_F&& __f)
    {
        function(__XVI_STD_NS::forward<_F>(__f).swap(*this));
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
        auto __temp = __XVI_STD_NS::move(__rhs);
        __rhs = __XVI_STD_NS::move(*this);
        *this = __XVI_STD_NS::move(__temp);
    }

    explicit operator bool() const noexcept
    {
        return __get_utils()->__get_type() != typeid(void);
    }

    _R operator()(_ArgTypes... __args) const
    {
        if (!bool(*this))
            throw bad_function_call();

        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
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
        && (!is_member_function_pointer_v<decltype(&_F::operator())>)
function(_F) -> function<remove_pointer_t<decltype(&_F::operator())>>;

template <class _F>
    requires requires { &_F::operator(); }
        && is_member_function_pointer_v<decltype(&_F::operator())>
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


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_FUNCTION_H */
