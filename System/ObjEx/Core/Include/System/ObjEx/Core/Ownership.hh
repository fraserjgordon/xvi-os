#ifndef __SYSTEM_OBJEX_CORE_OWNERSHIP_H
#define __SYSTEM_OBJEX_CORE_OWNERSHIP_H


//#include <type_traits>


namespace System::ObjEx::Core
{


enum class PointerValidation
{
    Off         = -1,
    Default     =  0,
    On          =  1,
};


class ManagedPointerBase
{
protected:

    struct opaque_obj;
    struct opaque_weak_obj;
    struct opaque_value;

    using object_ptr            = opaque_obj*;
    using object_weak_ptr       = const opaque_weak_obj*;
    using value_ptr             = const opaque_value*;
    using mutable_value_ptr     = opaque_value*;

    static value_ptr            valueCopy(value_ptr);
    static value_ptr            valueCopyAndRelease(value_ptr);
    static value_ptr            valueCopyAndAutorelease(value_ptr);
    static void                 valueRelease(value_ptr);
    static value_ptr            valueAutorelease(value_ptr);
    static mutable_value_ptr    valueMutableCopy(value_ptr);
    static mutable_value_ptr    valueMutableCopyAndRelease(value_ptr);

    static object_ptr objectRetain(object_ptr);
    static void       objectRelease(object_ptr);
    static void       objectAutorelease(object_ptr);

    // For weak references, the address of the weak reference is significant!
    static void       initWeak(object_weak_ptr*, object_ptr);
    static void       destroyWeak(object_weak_ptr*);
    static void       moveWeak(object_weak_ptr*, object_weak_ptr*);
    static void       copyWeak(object_weak_ptr*, const object_weak_ptr*);
    static object_ptr tryRetainWeak(const object_weak_ptr*);
};


template <class T>
class BorrowedValue
{
public:

    static constexpr bool Checked = true;


    constexpr BorrowedValue() = default;
    constexpr BorrowedValue(const BorrowedValue&) = default;
    constexpr BorrowedValue(BorrowedValue&&) = default;

    constexpr BorrowedValue& operator=(const BorrowedValue&) = default;
    constexpr BorrowedValue& operator=(BorrowedValue&&) = default;

    constexpr ~BorrowedValue() = default;


    const T* get() const noexcept
    {
        return m_ptr;
    }

    operator const T*() const noexcept
    {
        return get();
    }

    const T* operator->() const noexcept
    {
        
        return get();
    }

    const T& operator*() const
    {
        return *get();
    }


    friend constexpr auto operator<=>(const BorrowedValue& x, const BorrowedValue& y) noexcept
    {
        return x.m_ptr <=> y.m_ptr;
    }

private:

    const T* m_ptr = nullptr;

    [[noreturn]] void invalid() const;

    void validate() const
    {
        if constexpr (Checked)
        {
            if (!m_ptr)
                invalid();
        }
    }
};

template <class T>
class Value :
    protected ManagedPointerBase
{
public:



private:

    value_ptr   m_ptr = nullptr;


    const T* _get() const
    {
        return reinterpret_cast<const T*>(m_ptr);
    }

    void _set(const T* ptr)
    {
        m_ptr = reinterpret_cast<value_ptr>(ptr);
    }
};

template <class T>
class MutableValue :
    public Value<T>
{
};


} // namespace System::ObjEx::Core


#endif /* ifndef __SYSTEM_OBJEX_CORE_OWNERSHIP_H */
