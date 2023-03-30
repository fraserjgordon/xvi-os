#ifndef __SYSTEM_CXX_CORE_MOVEONLYFUNCTION_H
#define __SYSTEM_CXX_CORE_MOVEONLYFUNCTION_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/CtorTags.hh>
#include <System/C++/Core/Function.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class...> class move_only_function;


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
        __s.__external = operator new(sizeof(_T), std::align_val_t{alignof(_T)});
    }

    void __cleanup(__function_storage_t& __s) const final
    {
        operator delete(__s.__external, std::align_val_t{alignof(_T)});
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
        new (__ptr(__to)) _T(__XVI_STD_NS::move(*reinterpret_cast<_T*>(__ptr(__from))));
    }

    void __destroy(__function_storage_t& __s) const final
    {
        reinterpret_cast<_T*>(__ptr(__s))->~_T();
        __cleanup(__s);
    }

    bool __valid() const final
    {
        return is_same_v<_T, __empty_function>;
    }

    _R __call(__function_storage_t& __s, _Args&&... __args) const final
    {
        return __XVI_STD_NS::invoke_r<_R>(*reinterpret_cast<_T*>(__ptr(__s)), __XVI_STD_NS::forward<_Args>(__args)...);
    }
};


template <bool _Const, bool _Noexcept, class _RefType, class _R, class... _ArgTypes>
class __move_only_function_base
{

    template <class _T> using _ApplyCV = conditional_t<_Const, add_const_t<_T>, _T>;
    template <class _T> using _InvQuals = conditional_t<is_rvalue_reference_v<_RefType>, _ApplyCV<_T>&&, _ApplyCV<_T>&>;
    template <class _T> using _CVRef = conditional_t<!is_reference_v<_RefType>, _ApplyCV<_T>, _InvQuals<_T>>;

    template <class _VT>
    static constexpr bool __is_callable_from = _Noexcept
        ? is_nothrow_invocable_r_v<_R, _CVRef<_VT>, _ArgTypes...> && is_nothrow_invocable_r_v<_R, _InvQuals<_VT>, _ArgTypes...>
        : is_invocable_r_v<_R, _CVRef<_VT>, _ArgTypes...> && is_invocable_r_v<_R, _InvQuals<_VT>, _ArgTypes...>;

public:

    using result_type = _R;

    __move_only_function_base() noexcept :
        _M_utils_storage{},
        _M_data_storage{}
    {
        __construct_utils<__detail::__empty_function<_R, _ArgTypes...>>();
    }

    __move_only_function_base(nullptr_t) noexcept :
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
        requires (!is_same_v<remove_cvref_t<_F>, __move_only_function_base>)
            && (!__detail::__is_in_place_type_specialization_v<remove_cvref_t<_F>>)
            && __is_callable_from<decay_t<_F>>
    __move_only_function_base(_F&& __f) :
        __move_only_function_base()
    {
        if constexpr (is_pointer_v<remove_cvref_t<_F>> && is_function_v<remove_pointer_t<remove_cvref_t<_F>>>)
        {
            // Null function pointer => empty.
            if (__f == nullptr)
                return;

            // Otherwise, construct normally.
        }
        else if constexpr (is_member_function_pointer_v<remove_cvref_t<_F>>)
        {
            // Null member function pointer => empty.
            if (__f == nullptr)
                return;

            // Otherwise, construct normally.
        }
        else if constexpr (__is_move_only_function_specialization_v<remove_cvref_t<_F>>)
        {
            if (!__f)
                return;

            // Otherwise, construct normally.
        }

        auto __utils = __detail::__move_only_function_utils<remove_cvref_t<_F>, _R, _ArgTypes...>();
        __utils.template __emplace<remove_cvref_t<_F>>(_M_data_storage, __XVI_STD_NS::move(__f));
        __construct_utils<remove_cvref_t<_F>>();
    }

    template <class _T, class... _Args>
        requires is_constructible_v<decay_t<_T>, _Args...>
            && __is_callable_from<decay_t<_T>>
    explicit __move_only_function_base(in_place_type_t<_T>, _Args&&... __args)
    {
        using _VT = decay_t<_T>;
        static_assert(is_same_v<_T, _VT>);

        auto __utils = __detail::__move_only_function_utils<_VT, _R, _ArgTypes...>();
        __utils.template __emplace<_VT>(_M_data_storage, __XVI_STD_NS::forward<_Args>(__args)...);
        __construct_utils<_VT>();
    }

    template <class _T, class _U, class... _Args>
        requires is_constructible_v<decay_t<_T>, std::initializer_list<_U>&, _Args...>
            && __is_callable_from<decay_t<_T>>
    explicit __move_only_function_base(in_place_type_t<_T>, std::initializer_list<_U> __il, _Args&&... __args)
    {
        using _VT = decay_t<_T>;
        static_assert(is_same_v<_T, _VT>);

        auto __utils = __detail::__move_only_function_utils<_VT, _R, _ArgTypes...>();
        __utils.template __emplace<_VT>(_M_data_storage, __il, __XVI_STD_NS::forward<_Args>(__args)...);
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

    __move_only_function_base& operator=(nullptr_t)
    {
        __get_utils()->__destroy(_M_data_storage);
        __set_utils<__detail::__empty_function<_R, _ArgTypes...>>();

        return *this;
    }

    template <class _F>
    __move_only_function_base& operator=(_F&& __f)
    {
        move_only_function(__XVI_STD_NS::forward<_F>(__f)).swap(*this);
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
        auto __temp = __XVI_STD_NS::move(__rhs);
        __rhs = __XVI_STD_NS::move(*this);
        *this = __XVI_STD_NS::move(__temp);
    }

    _R operator()(_ArgTypes... __args)
        requires (!_Const && !is_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const
        requires (_Const && !is_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) &
        requires (!_Const && is_lvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const &
        requires (_Const && is_lvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) &&
        requires (!_Const && is_rvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const &&
        requires (_Const && is_rvalue_reference_v<_RefType> && !_Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) noexcept
        requires (!_Const && !is_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const noexcept
        requires (_Const && !is_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) & noexcept
        requires (!_Const && is_lvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const & noexcept
        requires (_Const && is_lvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) && noexcept
        requires (!_Const && is_rvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
    }

    _R operator()(_ArgTypes... __args) const && noexcept
        requires (_Const && is_rvalue_reference_v<_RefType> && _Noexcept)
    {
        return __get_utils()->__call(const_cast<__storage_t&>(_M_data_storage), __XVI_STD_NS::forward<_ArgTypes>(__args)...);
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
bool operator==(const move_only_function<_T...>& __x, nullptr_t)
{
    return bool(__x);
}


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_MOVEONLYFUNCTION_H */
