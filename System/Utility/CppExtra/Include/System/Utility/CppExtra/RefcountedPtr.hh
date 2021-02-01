#ifndef __SYSTEM_UTILITY_CPPEXTRA_REFCOUNTEDPTR_H
#define __SYSTEM_UTILITY_CPPEXTRA_REFCOUNTEDPTR_H


#include <atomic>
#include <cstddef>
#include <type_traits>
#include <utility>


namespace System::Utility::CppExtra
{


class refcounted_mixin
{
public:

    constexpr refcounted_mixin() noexcept = default;

    refcounted_mixin(const refcounted_mixin&) = delete;
    refcounted_mixin(refcounted_mixin&&) = delete;

    refcounted_mixin& operator=(const refcounted_mixin&) = delete;
    refcounted_mixin& operator=(refcounted_mixin&&) = delete;

    constexpr ~refcounted_mixin() = default;

private:

    template <class> friend class refcounted_ptr_traits;

    mutable std::atomic<std::size_t> m_refcount = 1;
};


template <class T> struct refcounted_ptr_traits {};

template <class T> struct refcounted_ptr_traits<const T> : refcounted_ptr_traits<T> {};

template <class T> 
    requires (std::is_base_of_v<refcounted_mixin, T>)
struct refcounted_ptr_traits<T>
{
    static constexpr void addRef(const refcounted_mixin* p)
    {
        ++p->m_refcount;
    }

    static constexpr void releaseRef(const refcounted_mixin* p)
    {
        if (p->m_refcount.fetch_sub(1) == 1)
            delete static_cast<T*>(const_cast<refcounted_mixin*>(p));
    }
};

template <class T>
concept refcounted_object = requires(const T* p)
{
    { refcounted_ptr_traits<std::remove_cv_t<T>>::addRef(p) };
    { refcounted_ptr_traits<std::remove_cv_t<T>>::releaseRef(p) };
};


template <class T>
    requires std::is_class_v<T>
class refcounted_ptr
{
public:

    // Static assertion instead of requirement so aliases of refcounted_ptr<T> for incomplete T can be declared.
    static_assert(refcounted_object<T>);

    using element_type      = T;
    using traits            = refcounted_ptr_traits<T>;

    constexpr refcounted_ptr() noexcept = default;

    constexpr refcounted_ptr(std::nullptr_t) :
        refcounted_ptr()
    {
    }

    template <class Y>
        requires std::is_convertible_v<Y*, T*>
    refcounted_ptr(Y* ptr) :
        m_ptr(ptr)
    {
    }

    constexpr refcounted_ptr(const refcounted_ptr& p) noexcept :
        m_ptr(p.m_ptr)
    {
        if (m_ptr)
            traits::addRef(m_ptr);
    }

    constexpr refcounted_ptr(refcounted_ptr&& p) noexcept :
        m_ptr(p.m_ptr)
    {
        p.m_ptr = nullptr;
    }

    template <class Y>
        requires std::is_convertible_v<Y*, T*>
    constexpr refcounted_ptr(const refcounted_ptr<Y>& p) noexcept :
        m_ptr(p.m_ptr)
    {
        if (m_ptr)
            traits::addRef(m_ptr);
    }

    template <class Y>
        requires std::is_convertible_v<Y*, T*>
    constexpr refcounted_ptr(refcounted_ptr<Y>&& p) noexcept :
        m_ptr(p.m_ptr)
    {
        p.m_ptr = nullptr;
    }

    constexpr refcounted_ptr& operator=(const refcounted_ptr& p) noexcept
    {
        if (p.m_ptr)
            traits::addRef(p.m_ptr);
        
        if (m_ptr)
            traits::releaseRef(m_ptr);

        m_ptr = p.m_ptr;
        return *this;
    }

    constexpr refcounted_ptr& operator=(refcounted_ptr&& p) noexcept
    {
        auto tmp = p.m_ptr;
        p.m_ptr = nullptr;
        m_ptr = tmp;
        return *this;
    }

    template <class Y>
        requires std::is_convertible_v<Y*, T*>
    constexpr refcounted_ptr& operator=(const refcounted_ptr<Y>& p) noexcept
    {
        return operator=(refcounted_ptr(p));
    }

    template <class Y>
        requires std::is_convertible_v<Y*, T*>
    constexpr refcounted_ptr& operator=(refcounted_ptr<Y>&& p) noexcept
    {
        return operator=(refcounted_ptr(std::move(p)));
    }

    constexpr ~refcounted_ptr()
    {
        reset();
    }

    constexpr void reset() noexcept
    {
        if (m_ptr)
            traits::releaseRef(m_ptr);
        m_ptr = nullptr;
    }

    template <class Y>
        requires std::is_convertible_v<Y*, T*>
    constexpr void reset(Y* p) noexcept
    {
        reset();
        m_ptr = p;
    }

    constexpr void swap(refcounted_ptr& p) noexcept
    {
        using std::swap;
        swap(m_ptr, p.m_ptr);
    }

    constexpr element_type* get() const noexcept
    {
        return m_ptr;
    }

    [[nodiscard]] constexpr element_type* take() noexcept
    {
        auto tmp = m_ptr;
        m_ptr = nullptr;
        return tmp;
    }

    T& operator*() const noexcept
    {
        return *m_ptr;
    }

    T* operator->() const noexcept
    {
        return m_ptr;
    }

    constexpr operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    constexpr bool operator==(const refcounted_ptr&) const noexcept = default;

    constexpr auto operator<=>(const refcounted_ptr&) const noexcept = default;

private:

    T*  m_ptr   = nullptr;
};


template <class T, class U>
refcounted_ptr<T> static_pointer_cast(const refcounted_ptr<U>& p) noexcept
{
    refcounted_ptr<T> pt;
    pt = static_cast<T*>(p.get());
    return pt;
}

template <class T, class U>
refcounted_ptr<T> static_pointer_cast(refcounted_ptr<U>&& p) noexcept
{
    return refcounted_ptr<T>(static_cast<T*>(p.take()));
}

template <class T, class U>
refcounted_ptr<T> dynamic_pointer_cast(const refcounted_ptr<U>& p) noexcept
{
    refcounted_ptr<T> pt;
    if (auto pp = dynamic_cast<T*>(p.get()); pp)
        pt = pp;
    return pt;
}

template <class T, class U>
refcounted_ptr<T> dynamic_pointer_cast(refcounted_ptr<U>&& p) noexcept
{
    if (auto pp = dynamic_cast<T*>(p.get()); pp)
    {
        refcounted_ptr<T> pt(pp);
        static_cast<void>(pp.take());
        return pt;
    }

    return {};
}

template <class T, class U>
refcounted_ptr<T> const_pointer_cast(const refcounted_ptr<U>& p) noexcept
{
    refcounted_ptr<T> pt;
    pt = const_cast<T*>(p.get());
    return pt;
}

template <class T, class U>
refcounted_ptr<T> const_pointer_cast(refcounted_ptr<U>&& p) noexcept
{
    return refcounted_ptr<T>(const_cast<T*>(p.take()));
}

template <class T, class U>
refcounted_ptr<T> reinterpret_pointer_cast(const refcounted_ptr<U>& p) noexcept
{
    refcounted_ptr<T> pt;
    pt = reinterpret_cast<T*>(p.get());
    return pt;
}

template <class T, class U>
refcounted_ptr<T> reinterpret_pointer_cast(refcounted_ptr<U>&& p) noexcept
{
    return refcounted_ptr<T>(reinterpret_cast<T*>(p.take()));
}

template <class T>
void swap(refcounted_ptr<T>& x, refcounted_ptr<T>& y) noexcept
{
    x.swap(y);
}


} // namespace System::Utility::CppExtra


#endif /* ifndef __SYSTEM_UTILITY_CPPEXTRA_REFCOUNTEDPTR_H */
