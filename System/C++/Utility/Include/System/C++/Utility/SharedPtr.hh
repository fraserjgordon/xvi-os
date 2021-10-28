#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SHAREDPTR_H
#define __SYSTEM_CXX_UTILITY_SHAREDPTR_H


#include <System/C++/Atomic/Atomic.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/TypeInfo.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/AllocatorTraits.hh>
#include <System/C++/Utility/DefaultDelete.hh>
#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/MemoryAlgorithms.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class> class allocator;
template <class> class enable_shared_from_this;
template <class> class shared_ptr;
template <class> class weak_ptr;
template <class, class> class unique_ptr;


class bad_weak_ptr : public exception
{
public:

    constexpr bad_weak_ptr() noexcept = default;

    const char* what() const noexcept override
        { return "bad weak pointer"; }
};


namespace __detail
{

template <class> struct __is_enable_shared_from_this_specialization : false_type {};
template <class _T> struct __is_enable_shared_from_this_specialization<enable_shared_from_this<_T>> : true_type {};

template <class _T> inline constexpr bool __is_enable_shared_from_this_specialization_v
    = __is_enable_shared_from_this_specialization<remove_cv_t<_T>>::value;


class __shared_ptr_owner_base
{
public:

    constexpr __shared_ptr_owner_base(void* __ptr, size_t __elems)
        : _M_object(__ptr),
          _M_extent(__elems)
    {
    }

    virtual void __delete_object(void*) = 0;
    virtual void __delete_this_owner() = 0;

    virtual void* __get_deleter_by_type_info(const type_info& __ti) noexcept = 0;

    virtual ~__shared_ptr_owner_base() = default;

    void __add_weak_ref()
    {
        _M_weak_ref.fetch_add(1, memory_order::relaxed);
    }

    void __sub_weak_ref()
    {
        if (_M_weak_ref.fetch_sub(1, memory_order::release) == 1)
        {
            atomic_thread_fence(memory_order::acquire);
            delete this;
        }
    }

    void __add_strong_ref()
    {
        _M_strong_ref.fetch_add(1, memory_order::relaxed);
    }

    void __sub_strong_ref()
    {
        if (_M_strong_ref.fetch_sub(1, memory_order::release) == 1)
        {
            atomic_thread_fence(memory_order::acquire);
            __delete_object(_M_object);
            _M_object = nullptr;
            __sub_weak_ref();
        }
    }

    size_t __get_strong_ref_count() const
    {
        return _M_strong_ref.load(memory_order::relaxed);
    }

    bool __try_add_strong_ref()
    {
        size_t __refcount = _M_strong_ref.load(memory_order::relaxed);
        while (__refcount != 0)
        {
            if (_M_strong_ref.compare_exchange_weak(__refcount, __refcount + 1, memory_order::relaxed))
                return true;
        }

        return false;
    }

#ifdef __cpp_impl_destroying_delete
    void operator delete(__shared_ptr_owner_base* __ptr, destroying_delete_t)
    {
        __ptr->__delete_this_owner();
    }
#endif // ifdef __cpp_impl_destroying_delete

    size_t __get_elem_count() const
    {
        return _M_extent;
    }

    template <class _D>
    _D* __get_deleter() noexcept
    {
        return static_cast<_D*>(__get_deleter_by_type_info(typeid(remove_cv_t<_D>)));
    }

private:

    void*          _M_object = nullptr;
    size_t         _M_extent = 0;
    atomic<size_t> _M_strong_ref = 1;
    atomic<size_t> _M_weak_ref = 1;
};

template <bool, class, class, class> class __shared_ptr_owner;

template <class _T, class _D, class _A>
struct __make_shared_combined_t
{
    __shared_ptr_owner<true, _T, _D, _A>    _M_owner;
    remove_extent_t<_T>                     _M_data[1];
};

template <bool _OneAllocation, class _T, class _D = __default_delete_for<_T>, class _A = allocator<void>>
class __shared_ptr_owner final : public __shared_ptr_owner_base
{
public:

    using _U = conditional_t<is_array_v<_T>, remove_extent_t<_T>, remove_cv_t<_T>>;

    using __combined_t = __make_shared_combined_t<_T, _D, _A>;
    using __allocation_t = conditional_t<_OneAllocation, __combined_t, __shared_ptr_owner>;

    using __combined_alloc_t = typename allocator_traits<_A>::template rebind_alloc<__combined_t>;
    using __data_alloc_t = typename allocator_traits<_A>::template rebind_alloc<_T>;
    using __owner_alloc_t = typename allocator_traits<_A>::template rebind_alloc<__shared_ptr_owner>;
    using __allocation_alloc_t = conditional_t<_OneAllocation, __combined_alloc_t, __owner_alloc_t>;

    template <class _Alloc>
    static __allocation_t* __allocate(_Alloc& __a, size_t __elems = 1)
    {
        using __alloc_t = typename allocator_traits<_Alloc>::template rebind_alloc<__allocation_t>;
        __alloc_t __alloc(__a);
        size_t __size = sizeof(__allocation_t);
        if constexpr (_OneAllocation)
            __size += (__elems - 1) * sizeof(_U);
        return allocator_traits<__alloc_t>::allocate(__alloc, __size);
    }

    template <class _Alloc>
    static void __deallocate(_Alloc& __a, __allocation_t* __p, size_t __elems = 1)
    {
        using __alloc_t = typename allocator_traits<_Alloc>::template rebind_alloc<__allocation_t>;
        __alloc_t __alloc(__a);
        size_t __size = sizeof(__allocation_t);
        if constexpr (_OneAllocation)
            __size += (__elems - 1) * sizeof(_U);
        allocator_traits<__alloc_t>::deallocate(__alloc, __p, __size);
    }

    constexpr __shared_ptr_owner(_U* __p, size_t __elems, _D __d = {}, _A __a = {})
        : __shared_ptr_owner_base(__p, __elems),
          _M_delete(__d),
          _M_allocate(__a)
    {
    }

    void __delete_object(void* __ptr) final
    {
        if (__ptr == nullptr)
            return;
        
        if constexpr (!_OneAllocation)
        {
            //! @TODO: destroy arrays.
            _M_delete(static_cast<_U*>(__ptr));
            __data_alloc_t __a(_M_allocate);
            __deallocate(__a, __ptr, __get_elem_count());
        }
        else
        {
            // Objects created by allocate_shared need to be destroyed via their allocator.
            //! @TODO: destroy arrays.
            using __alloc_t = typename allocator_traits<_A>::template rebind_alloc<_U>;
            __alloc_t __alloc(_M_allocate);
            allocator_traits<__alloc_t>::destroy(__alloc, static_cast<_U*>(__ptr));
        }
    }

    void __delete_this_owner() final
    {
        __allocation_alloc_t __a(_M_allocate);
        auto __ptr = reinterpret_cast<__allocation_t*>(this);
        this->~__shared_ptr_owner();
        __deallocate(__a, __ptr, __get_elem_count());
    }

    void* __get_deleter_by_type_info(const type_info& __ti) noexcept final
    {
        if (typeid(remove_cv_t<_D>) == __ti)
            return addressof(_M_delete);
        else
            return nullptr;
    }

private:

    [[no_unique_address]] _D _M_delete;
    [[no_unique_address]] _A _M_allocate;
};


template <class _T> using __delete_detector = decltype(delete declval<_T*>());
template <class _T> using __delete_arr_detector = decltype(delete[] declval<_T*>());

template <class _T> inline constexpr bool __has_delete_v = is_detected_v<__delete_detector, _T>;
template <class _T> inline constexpr bool __has_array_delete_v = is_detected_v<__delete_arr_detector, _T>;


#ifdef __XVI_CXX_UTILITY_NO_EXCEPTIONS
[[noreturn]] inline void __bad_weak_pointer() noexcept
{
    terminate();
}
#else
[[noreturn]] inline void __bad_weak_pointer()
{
    throw bad_weak_ptr();
}
#endif

// Stand-in for when a void reference or other invalid construct would be needed.
struct __invalid_type {};

} // namespace __detail


template <class _T>
class shared_ptr
{
private:

    template <class _Y>
    void __enables_shared_from_this(_Y* __p)
    {
        if constexpr (__detail::__is_enable_shared_from_this_specialization_v<_Y>)
        {
            if (__p != nullptr && __p->weak_this.expired())
                __p->weak_this = shared_ptr<remove_cv_t<_Y>>(*this, const_cast<remove_cv_t<_Y>*>(__p));
        }
        else
            static_cast<void>(__p); // Silence warning about unused parameter.
    }

    template <class _Y>
    constexpr bool __valid_ptr_type()
    {
        if constexpr (is_array_v<_T>)
        {
            if constexpr (!__detail::__has_array_delete_v<_Y>)
                return false;
            
            constexpr size_t _N = extent_v<_T>;
            if constexpr (_N == 0)
                return is_convertible_v<_Y(*)[], _T*>;
            else
                return is_convertible_v<_Y(*)[_N], _T*>;
        }
        else
        {
            return __detail::__has_delete_v<_Y> && is_convertible_v<_Y*, _T*>;
        }
    }

    template <class _Y, class _D>
    static constexpr bool __valid_ptr_type()
    {
        if constexpr (is_array_v<_T>)
        {
            if constexpr (!is_invocable_v<_D, _Y*>)
                return false;

            constexpr size_t _N = extent_v<_T>;
            if constexpr (_N == 0)
                return is_convertible_v<_Y(*)[], _T*>;
            else
                return is_convertible_v<_Y(*)[_N], _T*>;
        }
        else
        {
            return is_invocable_v<_D, _Y*> && is_convertible_v<_Y*, _T*>;
        }
    }

public:

    using element_type = remove_extent_t<_T>;
    using weak_type = weak_ptr<_T>;

    constexpr shared_ptr() noexcept
        : _M_ptr(nullptr), _M_owner(nullptr) {}
    constexpr shared_ptr(nullptr_t) noexcept : shared_ptr() {};

    template <class _Y,
              class = enable_if_t<__valid_ptr_type<_Y>(), void>>
    explicit shared_ptr(_Y* __p) __XVI_CXX_UTILITY_FN_TRY
        : _M_ptr(__p),
          _M_owner(new __detail::__shared_ptr_owner<false, _Y>(__p))
    {
        __XVI_CXX_UTILITY_CHECK_NEW_RESULT(_M_owner);

        if constexpr (!is_array_v<_T>)
            __enables_shared_from_this(__p);
    }
    __XVI_CXX_UTILITY_FN_CATCH(...,
    {
        if constexpr (is_array_v<_T>)
            delete[] __p;
        else
            delete __p;

        throw;
    })

    template <class _Y, class _D,
              class = enable_if_t<__valid_ptr_type<_Y, _D>()
                                  && is_move_constructible_v<_D>, void>>
    shared_ptr(_Y* __p, _D __d) __XVI_CXX_UTILITY_FN_TRY
        : _M_ptr(__p),
          _M_owner(new __detail::__shared_ptr_owner<false, _Y, _D>(__p, __XVI_STD_NS::move(__d)))
    {
        __XVI_CXX_UTILITY_CHECK_NEW_RESULT(_M_owner);

        if constexpr (!is_array_v<_T>)
            __enables_shared_from_this(__p);
    }
    __XVI_CXX_UTILITY_FN_CATCH(...,
    {
        __d(__p);
        throw;
    })

    template <class _Y, class _D, class _A,
              class = enable_if_t<__valid_ptr_type<_Y, _D>()
                                  && is_move_constructible_v<_D>, void>>
    shared_ptr(_Y* __p, _D __d, _A __a) __XVI_CXX_UTILITY_FN_TRY
        : _M_ptr(__p),
          _M_owner(nullptr)
    {
        _A __alloc = __a;
        using __owner_t = __detail::__shared_ptr_owner<false, _Y, _D, _A>;
        _M_owner = static_cast<__owner_t*>(__alloc.allocate(sizeof(__owner_t)));
        __XVI_CXX_UTILITY_CHECK_NEW_RESULT(_M_owner);
        new (_M_owner) __owner_t(__p, __XVI_STD_NS::move(__d), __XVI_STD_NS::move(__alloc));

        if constexpr (!is_array_v<_T>)
            __enables_shared_from_this(__p);
    }
    __XVI_CXX_UTILITY_FN_CATCH(...,
    {
        __d(__p);
        throw;
    })

    template <class _D,
              class = enable_if_t<__valid_ptr_type<element_type, _D>()
                                  && is_move_constructible_v<_D>, void>>
    shared_ptr(nullptr_t, _D __d) __XVI_CXX_UTILITY_FN_TRY
        : _M_ptr(nullptr),
          _M_owner(new __detail::__shared_ptr_owner<false, _T, _D>(nullptr, __XVI_STD_NS::move(__d)))
    {
        __XVI_CXX_UTILITY_CHECK_NEW_RESULT(_M_owner);
    }
    __XVI_CXX_UTILITY_FN_CATCH(...,
    {
        __d(nullptr);
        throw;
    })

    template <class _D, class _A,
              class = enable_if_t<__valid_ptr_type<_T, _D>()
                                  && is_move_constructible_v<_D>, void>>
    shared_ptr(nullptr_t, _D __d, _A __a) __XVI_CXX_UTILITY_FN_TRY
        : _M_ptr(nullptr),
          _M_owner(nullptr)
    {
        _A __alloc = __a;
        using __owner_t = __detail::__shared_ptr_owner<false, _T, _D, _A>;
        _M_owner = __owner_t::__allocate(__a, 0);
        __XVI_CXX_UTILITY_CHECK_NEW_RESULT(_M_owner);
        new (_M_owner) __owner_t(nullptr, 0, __XVI_STD_NS::move(__d), __XVI_STD_NS::move(__alloc));
    }
    __XVI_CXX_UTILITY_FN_CATCH(...,
    {
        __d(nullptr);
        throw;
    })

    template <class _Y>
    shared_ptr(const shared_ptr<_Y>& __r, element_type* __p) noexcept
        : _M_ptr(__p),
          _M_owner(__r._M_owner)
    {
        if (_M_owner)
            _M_owner->__add_strong_ref();
    }

    template <class _Y> shared_ptr(shared_ptr<_T>&& __r, element_type* __p) noexcept
        : _M_ptr(__p),
          _M_owner(__r._M_owner)
    {
        __r._M_ptr = nullptr;
        __r._M_owner = nullptr;
    }

    shared_ptr(const shared_ptr& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        if (_M_owner != nullptr)
            _M_owner->__add_strong_ref();
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    shared_ptr(const shared_ptr<_Y>& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        if (_M_owner != nullptr)
            _M_owner->__add_strong_ref();
    }

    shared_ptr(shared_ptr&& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        __r._M_ptr = nullptr;
        __r._M_owner = nullptr;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    shared_ptr(shared_ptr<_Y>&& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        __r._M_ptr = nullptr;
        __r._M_owner = nullptr;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    explicit shared_ptr(const weak_ptr<_Y>& __r)
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        if (_M_owner != nullptr && !_M_owner->__try_add_strong_ref())
            __detail::__bad_weak_pointer();
    }

    template <class _Y, class _D,
              class = enable_if_t<is_convertible_v<_Y*, _T*>
                                  && is_convertible_v<typename unique_ptr<_Y, _D>::pointer, element_type*>, void>>
    shared_ptr(unique_ptr<_Y, _D>&& __r)
        : shared_ptr()
    {
        if (__r.get() == nullptr)
            return;

        if constexpr (!is_reference_v<_D>)
            new (this) shared_ptr(__r.release(), __r.get_deleter());
        else
            new (this) shared_ptr(__r.release(), ref(__r.get_deleter()));
    }

    ~shared_ptr()
    {
        if (_M_owner)
            _M_owner->__sub_strong_ref();
    }

    shared_ptr& operator=(const shared_ptr& __r) noexcept
    {
        _M_ptr = __r._M_ptr;
        if (_M_owner != nullptr && _M_owner != __r._M_owner)
        {
            _M_owner->__sub_strong_ref();
            _M_owner = __r._M_owner;
            _M_owner->__add_strong_ref();
        }

        return *this;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    shared_ptr& operator=(const shared_ptr<_Y>& __r) noexcept
    {
        _M_ptr = __r._M_ptr;
        if (_M_owner != nullptr && _M_owner != __r._M_owner)
        {
            _M_owner->__sub_strong_ref();
            _M_owner = __r._M_owner;
            _M_owner->__add_strong_ref();
        }

        return *this;
    }

    shared_ptr& operator=(shared_ptr&& __r) noexcept
    {
        _M_ptr = __r._M_ptr;
        __r._M_ptr = nullptr;

        if (_M_owner != nullptr)
            _M_owner->__sub_strong_ref();

        _M_owner = __r._M_owner;
        __r._M_owner = nullptr;

        return *this;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    shared_ptr& operator=(shared_ptr<_Y>&& __r) noexcept
    {
        _M_ptr = __r._M_ptr;
        __r._M_ptr = nullptr;

        if (_M_owner != nullptr)
            _M_owner->__sub_strong_ref();

        _M_owner = __r._M_owner;
        __r._M_owner = nullptr;

        return *this;
    }

    template <class _Y, class _D,
              class = enable_if_t<is_convertible_v<_Y*, _T*>
                                  && is_convertible_v<typename unique_ptr<_Y, _D>::pointer, element_type*>, void>>
    shared_ptr& operator=(unique_ptr<_Y, _D>&& __r)
    {
        shared_ptr(__XVI_STD_NS::move(__r)).swap(*this);
        return *this;
    }

    void swap(shared_ptr& __r) noexcept
    {
        auto __ptr_temp = __r._M_ptr;
        auto __owner_temp = __r._M_owner;
        __r._M_ptr = _M_ptr;
        __r._M_owner = _M_owner;
        _M_ptr = __ptr_temp;
        _M_owner = __owner_temp;
    }

    void reset() noexcept
    {
        _M_ptr = nullptr;
        if (_M_owner != nullptr)
            _M_owner->__sub_strong_ref();
        _M_owner = nullptr;
    }

    template <class _Y>
    void reset(_Y* __p)
    {
        shared_ptr(__p).swap(*this);
    }

    template <class _Y, class _D>
    void reset(_Y* __p, _D __d)
    {
        shared_ptr(__p, __d).swap(*this);
    }

    template <class _Y, class _D, class _A>
    void reset(_Y* __p, _D __d, _A __a)
    {
        shared_ptr(__p, __d, __a).swap(*this);
    }

    element_type* get() const noexcept
    {
        return _M_ptr;
    }

    conditional_t<is_void_v<_T>, __detail::__invalid_type,_T&>
    operator*() const noexcept
    {
        return *get();
    }

    conditional_t<is_array_v<_T>, __detail::__invalid_type, _T*>
    operator->() const noexcept
    {
        return get();
    }

    conditional_t<is_array_v<_T>, element_type&, __detail::__invalid_type>
    operator[](ptrdiff_t __i) const
    {
        return get()[__i];
    }

    long use_count() const noexcept
    {
        return _M_owner ? _M_owner->__get_strong_ref_count() : 0;
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    template <class _U>
    bool owner_before(const shared_ptr<_U>& __b) const noexcept
    {
        return _M_owner < __b._M_owner;
    }

    template <class _U>
    bool owner_before(const weak_ptr<_U>& __b) const noexcept
    {
        return _M_owner < __b._M_owner;
    }

    template <class _D>
    _D* __get_deleter() const noexcept
    {
        return _M_owner->template __get_deleter<remove_cv_t<_D>>();
    }

    static shared_ptr __make(element_type* __p, __detail::__shared_ptr_owner_base* __o)
    {
        shared_ptr __ret;
        __ret._M_ptr = __p;
        __ret._M_owner = __o;
        if constexpr (!is_array_v<_T>)
            __ret.__enables_shared_from_this(__p);
        return __ret;
    }

private:

    template <class> friend class shared_ptr;
    template <class> friend class weak_ptr;
    friend struct atomic<shared_ptr>;

    element_type*                      _M_ptr;
    __detail::__shared_ptr_owner_base* _M_owner;
};

template <class _T>
shared_ptr(weak_ptr<_T>) -> shared_ptr<_T>;

template <class _T, class _D>
shared_ptr(unique_ptr<_T, _D>) -> shared_ptr<_T>;


template <class _T, class... _Args,
          class = enable_if_t<!is_array_v<_T>, void>>
shared_ptr<_T> make_shared(_Args&&... __args)
{
    using __owner_t = __detail::__shared_ptr_owner<true, _T>;
    allocator<_T> __alloc;
    auto* __mem = __owner_t::__allocate(__alloc);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        new (&__mem->_M_data[0]) _T(__XVI_STD_NS::forward<_Args>(__args)...);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__alloc, __mem);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], 1);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T, class _A, class... _Args,
          class = enable_if_t<!is_array_v<_T>, void>>
shared_ptr<_T> allocate_shared(const _A& __a, _Args&&... __args)
{
    using _U = remove_cv_t<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T, __detail::__default_delete_for<_T>, _A>;
    auto* __mem = __owner_t::__allocate(__a);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        using __alloc_t = typename allocator_traits<_A>::template rebind<_U>;
        __alloc_t __alloc(__a);
        allocator_traits<__alloc_t>::construct(__a, &__mem->_M_data[0], __XVI_STD_NS::forward<_Args>(__args)...);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__a, __mem);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], 1, {}, __a);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
shared_ptr<_T> make_shared(size_t __n)
{
    using _U = remove_extent_t<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T>;
    allocator<_T> __alloc;
    auto* __mem = __owner_t::__allocate(__alloc, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        _U* __ptr = &__mem->_M_data[0];
        uninitialized_value_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__alloc, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T, class _A,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
shared_ptr<_T> allocate_shared(const _A& __a, size_t __n)
{
    using _U = remove_extent_t<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T, __detail::__default_delete_for<_T>, _A>;
    auto* __mem = __owner_t::__allocate(__a, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        using __alloc_t = typename allocator_traits<_A>::template rebind<_U>;
        __alloc_t __alloc(__a);

        _U* __ptr = &__mem->_M_data[0];
        uninitialized_value_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__a, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n, {}, __a);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
shared_ptr<_T> make_shared()
{
    using _U = remove_extent_t<_T>;
    constexpr size_t __n = extent_v<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T>;
    allocator<_T> __alloc;
    auto* __mem = __owner_t::__allocate(__alloc, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        _U* __ptr = &__mem->_M_data[0];
        uninitialized_value_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__alloc, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T, class _A,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
shared_ptr<_T> allocate_shared(const _A& __a)
{
    using _U = remove_extent_t<_T>;
    constexpr size_t __n = extent_v<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T, __detail::__default_delete_for<_T>, _A>;
    auto* __mem = __owner_t::__allocate(__a, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        using __alloc_t = typename allocator_traits<_A>::template rebind<_U>;
        __alloc_t __alloc(__a);

        _U* __ptr = &__mem->_M_data[0];
        uninitialized_value_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__a, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n, {}, __a);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
shared_ptr<_T> make_shared(size_t __n, const remove_extent_t<_T>& __u)
{
    using _U = remove_extent_t<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T>;
    allocator<_T> __alloc;
    auto* __mem = __owner_t::__allocate(__alloc, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        _U* __ptr = &__mem->_M_data[0];
        uninitialized_fill_n(__ptr, __n, __u);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__alloc, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T, class _A,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
shared_ptr<_T> allocate_shared(const _A& __a, size_t __n, const remove_extent_t<_T>& __u)
{
    using _U = remove_extent_t<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T, __detail::__default_delete_for<_T>, _A>;
    auto* __mem = __owner_t::__allocate(__a, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        using __alloc_t = typename allocator_traits<_A>::template rebind<_U>;
        __alloc_t __alloc(__a);

        _U* __ptr = &__mem->_M_data[0];
        uninitialized_fill_n(__ptr, __n, __u);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__a, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n, {}, __a);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
shared_ptr<_T> make_shared(const remove_extent_t<_T>& __u)
{
    using _U = remove_extent_t<_T>;
    constexpr size_t __n = extent_v<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T>;
    allocator<_T> __alloc;
    auto* __mem = __owner_t::__allocate(__alloc, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        _U* __ptr = &__mem->_M_data[0];
        uninitialized_fill_n(__ptr, __n, __u);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__alloc, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T, class _A,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
shared_ptr<_T> allocate_shared(const _A& __a, const remove_extent_t<_T>& __u)
{
    using _U = remove_extent_t<_T>;
    constexpr size_t __n = extent_v<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T, __detail::__default_delete_for<_T>, _A>;
    auto* __mem = __owner_t::__allocate(__a, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        using __alloc_t = typename allocator_traits<_A>::template rebind<_U>;
        __alloc_t __alloc(__a);

        _U* __ptr = &__mem->_M_data[0];
        uninitialized_fill_n(__ptr, __n, __u);
        allocator_traits<__alloc_t>::construct(__a, &__mem->_M_data[0], __u);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__a, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n, {}, __a);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
shared_ptr<_T> make_shared_default_init()
{
    using _U = remove_extent_t<_T>;
    constexpr size_t __n = extent_v<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T>;
    allocator<_T> __alloc;
    auto* __mem = __owner_t::__allocate(__alloc, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        _U* __ptr = &__mem->_M_data[0];
        uninitialized_default_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__alloc, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T, class _A,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
shared_ptr<_T> allocate_shared_default_init(const _A& __a)
{
    using _U = remove_extent_t<_T>;
    constexpr size_t __n = extent_v<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T, __detail::__default_delete_for<_T>, _A>;
    auto* __mem = __owner_t::__allocate(__a, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        using __alloc_t = typename allocator_traits<_A>::template rebind<_U>;
        __alloc_t __alloc(__a);

        _U* __ptr = &__mem->_M_data[0];
        uninitialized_default_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__a, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n, {}, __a);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
shared_ptr<_T> make_shared_default_init(size_t __n)
{
    using _U = remove_extent_t<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T>;
    allocator<_T> __alloc;
    auto* __mem = __owner_t::__allocate(__alloc, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        _U* __ptr = &__mem->_M_data[0];
        uninitialized_default_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__alloc, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}

template <class _T, class _A,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
shared_ptr<_T> allocate_shared_default_init(const _A& __a, size_t __n)
{
    using _U = remove_extent_t<_T>;
    using __owner_t = __detail::__shared_ptr_owner<true, _T, __detail::__default_delete_for<_T>, _A>;
    auto* __mem = __owner_t::__allocate(__a, __n);
    __XVI_CXX_UTILITY_CHECK_NEW_RESULT(__mem);

    __XVI_CXX_UTILITY_TRY
    {
        using __alloc_t = typename allocator_traits<_A>::template rebind<_U>;
        __alloc_t __alloc(__a);

        _U* __ptr = &__mem->_M_data[0];
        uninitialized_default_construct_n(__ptr, __n);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        __owner_t::__deallocate(__a, __mem, __n);
        __XVI_CXX_UTILITY_RETHROW;
    }

    new (&__mem->_M_owner) __owner_t(&__mem->_M_data[0], __n, {}, __a);

    return shared_ptr<_T>::__make(&__mem->_M_data[0], &__mem->_M_owner);
}


template <class _T, class _U>
bool operator==(const shared_ptr<_T>& __t, const shared_ptr<_U>& __u) noexcept
{
    return __t.get() == __u.get();
}

template <class _T, class _U>
bool operator< (const shared_ptr<_T>& __t, const shared_ptr<_U>& __u) noexcept
{
    return less<>()(__t.get(), __u.get());
}

template <class _T>
bool operator==(const shared_ptr<_T>& __p, nullptr_t) noexcept
{
    return !__p;
}

template <class _T>
bool operator==(nullptr_t, const shared_ptr<_T>& __p) noexcept
{
    return !__p;
}

template <class _T>
bool operator!=(const shared_ptr<_T>& __p, nullptr_t) noexcept
{
    return static_cast<bool>(__p);
}

template <class _T>
bool operator!=(nullptr_t, const shared_ptr<_T>& __p) noexcept
{
    return static_cast<bool>(__p);
}

template <class _T>
bool operator< (const shared_ptr<_T>& __p, nullptr_t) noexcept
{
    return less<typename shared_ptr<_T>::element_type*>()(__p.get(), nullptr);
}

template <class _T>
bool operator< (nullptr_t, const shared_ptr<_T>& __p) noexcept
{
    return less<typename shared_ptr<_T>::element_type*>()(nullptr, __p.get());
}

template <class _T>
bool operator> (const shared_ptr<_T>& __p, nullptr_t) noexcept
{
    return nullptr < __p;
}

template <class _T>
bool operator> (nullptr_t, const shared_ptr<_T>& __p) noexcept
{
    return __p < nullptr;
}

template <class _T>
bool operator<=(const shared_ptr<_T>& __p, nullptr_t) noexcept
{
    return !(nullptr < __p);
}

template <class _T>
bool operator<=(nullptr_t, const shared_ptr<_T>& __p) noexcept
{
    return !(__p < nullptr);
}

template <class _T>
bool operator>=(const shared_ptr<_T>& __p, nullptr_t) noexcept
{
    return !(__p < nullptr);
}

template <class _T>
bool operator>=(nullptr_t, const shared_ptr<_T>& __p) noexcept
{
    return !(nullptr < __p);
}


template <class _T>
void swap(shared_ptr<_T>& __a, shared_ptr<_T>& __b) noexcept
{
    __a.swap(__b);
}


template <class _T, class _U>
shared_ptr<_T> static_pointer_cast(const shared_ptr<_U>& __r) noexcept
{
    return shared_ptr<_T>(__r, static_cast<typename shared_ptr<_T>::element_type*>(__r.get()));
}

template <class _T, class _U>
shared_ptr<_T> static_pointer_cast(shared_ptr<_U>&& __r) noexcept
{
    return shared_ptr<_T>(__XVI_STD_NS::move(__r), static_cast<typename shared_ptr<_T>::element_type*>(__r.get()));
}

template <class _T, class _U>
shared_ptr<_T> dynamic_pointer_cast(const shared_ptr<_U>& __r) noexcept
{
    auto __p = dynamic_cast<typename shared_ptr<_T>::element_type*>(__r.get());
    if (__p)
        return shared_ptr<_T>(__r, __p);
    else
        return shared_ptr<_T>();
}

template <class _T, class _U>
shared_ptr<_T> dynamic_pointer_cast(shared_ptr<_U>&& __r) noexcept
{
    auto __p = dynamic_cast<typename shared_ptr<_T>::element_type*>(__r.get());
    if (__p)
        return shared_ptr<_T>(__XVI_STD_NS::move(__r), __p);
    else
        return shared_ptr<_T>();
}

template <class _T, class _U>
shared_ptr<_T> const_pointer_cast(const shared_ptr<_U>& __r) noexcept
{
    return shared_ptr<_T>(__r, const_cast<typename shared_ptr<_T>::element_type*>(__r.get()));
}

template <class _T, class _U>
shared_ptr<_T> const_pointer_cast(shared_ptr<_U>&& __r) noexcept
{
    return shared_ptr<_T>(__XVI_STD_NS::move(__r), const_cast<typename shared_ptr<_T>::element_type*>(__r.get()));
}

template <class _T, class _U>
shared_ptr<_T> reinterpret_pointer_cast(const shared_ptr<_U>& __r) noexcept
{
    return shared_ptr<_T>(__r, reinterpret_cast<typename shared_ptr<_T>::element_type*>(__r.get()));
}

template <class _T, class _U>
shared_ptr<_T> reinterpret_pointer_cast(shared_ptr<_U>&& __r) noexcept
{
    return shared_ptr<_T>(__XVI_STD_NS::move(__r), reinterpret_cast<typename shared_ptr<_T>::element_type*>(__r.get()));
}


template <class _D, class _T>
_D* get_deleter(const shared_ptr<_T>& __p) noexcept
{
    return __p.template __get_deleter<_D>();
}


template <class _E, class _T, class _Y>
basic_ostream<_E, _T>& operator<<(basic_ostream<_E, _T>& __os, const shared_ptr<_Y>& __p)
{
    __os << __p.get();
    return __os;
}


template <class _T>
class weak_ptr
{
public:

    using element_type = remove_extent_t<_T>;

    constexpr weak_ptr() noexcept
        : _M_ptr(nullptr),
          _M_owner(nullptr)
    {
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    weak_ptr(const shared_ptr<_Y>& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        if (_M_owner)
            _M_owner->__add_weak_ref();
    }

    weak_ptr(const weak_ptr& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        if (_M_owner)
            _M_owner->__add_weak_ref();
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    weak_ptr(const weak_ptr<_Y>& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        if (_M_owner)
            _M_owner->__add_weak_ref();
    }

    weak_ptr(weak_ptr&& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        __r._M_ptr = nullptr;
        __r._M_owner = nullptr;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _Y*>, void>>
    weak_ptr(weak_ptr<_Y>&& __r) noexcept
        : _M_ptr(__r._M_ptr),
          _M_owner(__r._M_owner)
    {
        __r._M_ptr = nullptr;
        __r._M_owner = nullptr;
    }

    ~weak_ptr()
    {
        reset();
    }

    weak_ptr& operator=(const weak_ptr& __r) noexcept
    {
        if (__r._M_owner != _M_owner)
        {
            if (_M_owner)
                _M_owner->__sub_weak_ref();

            _M_owner = __r._M_owner;

            if (_M_owner)
                _M_owner->__add_weak_ref();
        }

        _M_ptr = __r._M_ptr;

        return *this;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    weak_ptr& operator=(const weak_ptr<_Y>& __r) noexcept
    {
        if (__r._M_owner != _M_owner)
        {
            if (_M_owner)
                _M_owner->__sub_weak_ref();

            _M_owner = __r._M_owner;

            if (_M_owner)
                _M_owner->__add_weak_ref();
        }

        _M_ptr = __r._M_ptr;

        return *this;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    weak_ptr& operator=(const shared_ptr<_Y>& __r) noexcept
    {
        if (__r._M_owner != _M_owner)
        {
            if (_M_owner)
                _M_owner->__sub_weak_ref();

            _M_owner = __r._M_owner;

            if (_M_owner)
                _M_owner->__add_weak_ref();
        }

        _M_ptr = __r._M_ptr;

        return *this;
    }

    weak_ptr& operator=(weak_ptr&& __r) noexcept
    {
        if (&__r == this)
            return *this;

        if (_M_owner)
            _M_owner->__sub_weak_ref();

        _M_owner = __r._M_owner;
        _M_ptr = __r._M_ptr;
        __r._M_ptr = nullptr;
        __r._M_owner = nullptr;

        return *this;
    }

    template <class _Y,
              class = enable_if_t<is_convertible_v<_Y*, _T*>, void>>
    weak_ptr& operator=(weak_ptr<_Y>&& __r) noexcept
    {
        if (&__r == this)
            return *this;

        if (_M_owner)
            _M_owner->__sub_weak_ref();

        _M_owner = __r._M_owner;
        _M_ptr = __r._M_ptr;
        __r._M_ptr = nullptr;
        __r._M_owner = nullptr;

        return *this;
    }

    void swap(weak_ptr& __r) noexcept
    {
        auto __owner_temp = _M_owner;
        auto __ptr_temp = _M_ptr;
        _M_owner = __r._M_owner;
        _M_ptr = __r._M_ptr;
        __r._M_owner = __owner_temp;
        __r._M_ptr = __ptr_temp;
    }

    void reset() noexcept
    {
        if (_M_owner)
            _M_owner->__sub_weak_ref();

        _M_owner = nullptr;
        _M_ptr = nullptr;
    }

    long use_count() const noexcept
    {
        return _M_owner ? _M_owner->__get_strong_ref_count() : 0;
    }

    bool expired() const noexcept
    {
        return use_count() == 0;
    }

    shared_ptr<_T> lock() const noexcept
    {
        shared_ptr<_T> __ret;
        if (_M_owner && _M_owner->__try_add_strong_ref())
        {
            __ret._M_owner = _M_owner;
            __ret._M_ptr = _M_ptr;
        }

        return __ret;
    }

    template <class _U>
    bool owner_before(const shared_ptr<_U>& __r) const noexcept
    {
        return _M_owner < __r._M_owner;
    }

    template <class _U>
    bool owner_before(const weak_ptr<_U>& __r) const noexcept
    {
        return _M_owner < __r._M_owner;
    }

private:

    template <class> friend class shared_ptr;
    template <class> friend class weak_ptr;
    friend struct atomic<weak_ptr>;

    _T*                                _M_ptr;
    __detail::__shared_ptr_owner_base* _M_owner;
};

template <class _T> weak_ptr(shared_ptr<_T>) -> weak_ptr<_T>;


template <class _T>
void swap(weak_ptr<_T>& __a, weak_ptr<_T>& __b) noexcept
{
    __a.swap(__b);
}


template <class _T = void>
struct owner_less;

template <class _T>
struct owner_less<shared_ptr<_T>>
{
    bool operator()(const shared_ptr<_T>& __a, const shared_ptr<_T>& __b) const noexcept
    {
        return __a.owner_less(__b);
    }

    bool operator()(const shared_ptr<_T>& __a, const weak_ptr<_T>& __b) const noexcept
    {
        return __a.owner_less(__b);
    }

    bool operator()(const weak_ptr<_T>& __a, const shared_ptr<_T>& __b) const noexcept
    {
        return __a.owner_less(__b);
    }
};

template <class _T>
struct owner_less<weak_ptr<_T>>
{
    bool operator()(const weak_ptr<_T>& __a, const weak_ptr<_T>& __b) const noexcept
    {
        return __a.owner_less(__b);
    }

    bool operator()(const weak_ptr<_T>& __a, const shared_ptr<_T>& __b) const noexcept
    {
        return __a.owner_less(__b);
    }

    bool operator()(const shared_ptr<_T>& __a, const weak_ptr<_T>& __b) const noexcept
    {
        return __a.owner_less(__b);
    }
};

template <>
struct owner_less<void>
{
    template <class _T, class _U>
    bool operator()(const shared_ptr<_T>& __a, const shared_ptr<_U>& __b) const noexcept
    {
        return __a.owner_before(__b);
    }

    template <class _T, class _U>
    bool operator()(const shared_ptr<_T>& __a, const weak_ptr<_U>& __b) const noexcept
    {
        return __a.owner_before(__b);
    }

    template <class _T, class _U>
    bool operator()(const weak_ptr<_T>& __a, const shared_ptr<_U>& __b) const noexcept
    {
        return __a.owner_before(__b);
    }

    template <class _T, class _U>
    bool operator()(const weak_ptr<_T>& __a, const weak_ptr<_U>& __b) const noexcept
    {
        return __a._owner_before(__b);
    }

    using is_transparent = void;
};


template <class _T>
class enable_shared_from_this
{
protected:

    constexpr enable_shared_from_this() noexcept
        : _M_weak_this{}
    {
    }

    enable_shared_from_this(const enable_shared_from_this&) noexcept
        : _M_weak_this{}
    {
    }

    enable_shared_from_this& operator=(const enable_shared_from_this&) noexcept
    {
        return *this;
    }

    ~enable_shared_from_this()
    {
    }

public:

    shared_ptr<_T> shared_from_this()
    {
        return shared_ptr<_T>(_M_weak_this);
    }

    shared_ptr<_T const> shared_from_this() const
    {
        return shared_ptr<_T const>(_M_weak_this);
    }

    weak_ptr<_T> weak_from_this() noexcept
    {
        return _M_weak_this;
    }

    weak_ptr<_T const> weak_from_this() const noexcept
    {
        return _M_weak_this;
    }

private:

    mutable weak_ptr<_T> _M_weak_this;
};


//! @TODO: hash support.
template <class> struct hash;
template <class _T> struct hash<shared_ptr<_T>>;


template <class _T>
struct atomic<shared_ptr<_T>>;
/*{
private:

    //! @TODO: implement (will need locking :-( )

    struct __raw_t { uintptr_t __raw[2]; };
    atomic<__raw_t> _M_atomic;

    static __raw_t __to_raw(shared_ptr<_T>&& __p) noexcept
    {
        __raw_t __ret = {uintptr_t(__p._M_ptr), uintptr_t(__p._M_owner)};
        __p._M_ptr = nullptr;
        __p._M_owner = nullptr;
        return __ret;
    }

    static shared_ptr<_T> __from_raw(__raw_t&& __r) noexcept
    {
        shared_ptr<_T> __p;
        __p._M_ptr = reinterpret_cast<_T*>(__r.__raw[0]);
        __p._M_owner = reinterpret_cast<__detail::__shared_ptr_owner_base*>(__r.__raw[1]);
        return __p;
    }

public:

    using value_type = shared_ptr<_T>;
};*/

template <class _T>
struct atomic<weak_ptr<_T>>;
/*{

};*/


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SHAREDPTR_H */
