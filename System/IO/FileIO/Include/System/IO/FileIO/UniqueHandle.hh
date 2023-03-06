#ifndef __SYSTEM_IO_FILEIO_UNIQUEHANDLE_H
#define __SYSTEM_IO_FILEIO_UNIQUEHANDLE_H


#include <memory>
#include <typeinfo>
#include <type_traits>


namespace System::IO::FileIO
{


// Forward declarations.
class Handle;


class UniqueHandleBase
{
    friend class SharedHandleBase;

protected:

    template <class T> struct HandleData;

    template <class T>
    struct HandleOps
    {
        using CloneSharedFn = std::shared_ptr<HandleData<T>>(const HandleData<T>*);
        using CloneUniqueFn = std::unique_ptr<HandleData<T>>(const HandleData<T>*);
        using TypeinfoFn = const std::type_info&();

        static std::shared_ptr<HandleData<T>> cloneSharedImpl(const HandleData<T>*);
        static std::unique_ptr<HandleData<T>> cloneUniqueImpl(const HandleData<T>*);
        static const std::type_info& typeInfoImpl() noexcept;

        CloneSharedFn* cloneShared = &cloneSharedImpl;
        CloneUniqueFn* cloneUnique = &cloneUniqueImpl;
        TypeinfoFn* typeInfo = &typeInfoImpl;
    };

    template <class T>
    struct HandleData
    {
        static inline constexpr HandleOps<T> ops = {};

        const HandleOps<T>* vtbl = &ops;
        T                   handle = {};

        static std::shared_ptr<HandleData> makeShared(T handle)
        {
            return std::make_shared<HandleData>(&ops, std::move(handle));
        }

        static std::unique_ptr<HandleData> makeUnique(T handle)
        {
            return std::make_unique<HandleData>(&ops, std::move(handle));
        }

        template <class U>
            requires std::same_as<T, U> || std::derived_from<T, U>
        static std::shared_ptr<HandleData<U>> convertTo(std::shared_ptr<HandleData> ptr)
        {
            // Nasty type punning here...
            return std::reinterpret_pointer_cast<HandleData<U>>(std::move(ptr));
        }

        template <class U>
            requires std::same_as<T, U> || std::derived_from<T, U>
        static std::unique_ptr<HandleData<U>> convertTo(std::unique_ptr<HandleData> ptr) noexcept
        {
            // Nasty type punning here...
            return std::reinterpret_pointer_cast<HandleData<U>>(std::move(ptr));
        }

        template <class U>
            requires std::same_as<T, U> || std::derived_from<T, U>
        std::shared_ptr<HandleData<U>> cloneShared() const
        {
            return convertTo<U>(vtbl->cloneShared(this));
        }

        template <class U>
            requires std::same_as<T, U> || std::derived_from<T, U>
        std::unique_ptr<HandleData<U>> cloneUnique() const
        {
            return convertTo<U>(vtbl->cloneUnique(this));
        }
        
        const std::type_info& typeInfo() const noexcept
        {
            return vtbl->typeInfo();
        }
    };
};


template <class T>
std::shared_ptr<UniqueHandleBase::HandleData<T>> UniqueHandleBase::HandleOps<T>::cloneSharedImpl(const HandleData<T>* data)
{
    auto clone = data->handle.clone();
    if (!clone)
        throw std::system_error(clone.error());

    return std::make_shared<HandleData<T>>(data->vtbl, std::move(*clone));
}


template <class T>
std::unique_ptr<UniqueHandleBase::HandleData<T>> UniqueHandleBase::HandleOps<T>::cloneUniqueImpl(const HandleData<T>* data)
{
    auto clone = data->handle.clone();
    if (!clone)
        throw std::system_error(clone.error());

    return std::make_unique<HandleData<T>>(data->vtbl, std::move(*clone));
}


template <class T>
const std::type_info& UniqueHandleBase::HandleOps<T>::typeInfoImpl() noexcept
{
    return typeid(T);
}



template <std::derived_from<Handle> T>
class UniqueHandle :
    protected UniqueHandleBase
{
    template <std::derived_from<Handle> U> friend class SharedHandle;
    template <std::derived_from<Handle> U> friend class UniqueHandle;

public:

    constexpr UniqueHandle() noexcept = default;
    UniqueHandle(const UniqueHandle&) = delete;
    constexpr UniqueHandle(UniqueHandle&&) noexcept = default;

    constexpr UniqueHandle(T handle) :
        m_handle(wrap(std::move(handle)))
    {
    }

    template <std::derived_from<T> U>
    constexpr UniqueHandle(U handle) :
        m_handle(wrap(std::move(handle)))
    {
    }

    template <std::derived_from<T> U>
    constexpr UniqueHandle(UniqueHandle<U> handle) :
        m_handle(std::move(handle.m_handle))
    {
    }

    constexpr UniqueHandle(std::nullptr_t) noexcept :
        UniqueHandle()
    {
    }

    constexpr ~UniqueHandle() = default;

    void operator=(const UniqueHandle&) = delete;
    constexpr UniqueHandle& operator=(UniqueHandle&&) noexcept = default;

    constexpr UniqueHandle& operator=(std::nullptr_t) noexcept
    {
        m_handle.reset();
        return *this;
    }

    constexpr operator T& () const noexcept
    {
        return *operator->();
    }

    constexpr T* operator->() const noexcept
    {
        return &m_handle->handle;
    }

    template <class U>
    constexpr bool is() const noexcept
    {
        return m_handle && m_handle->typesafeType() == typeid(U); 
    }

private:

    std::unique_ptr<HandleData<T>> m_handle = nullptr;


    template <std::derived_from<T> U>
    static std::unique_ptr<HandleData<T>> convert(const std::unique_ptr<HandleData<U>>& ptr)
    {
        return HandleData<U>::template convertTo<T>(ptr);
    }

    template <std::derived_from<T> U>
    static std::unique_ptr<HandleData<T>> convert(std::unique_ptr<HandleData<U>>&& ptr)
    {
        return HandleData<U>::template convertTo<T>(std::move(ptr));
    }

    static std::unique_ptr<HandleData<T>> wrap(T handle)
    {
        return HandleData<T>::makeUnique(std::move(handle));
    }

    template <std::derived_from<T> U>
    static std::unique_ptr<HandleData<T>> wrap(U handle)
    {
        return convert(HandleData<U>::makeUnique(std::move(handle)));
    }
};


} // namespace System::IO::FileHandle


#endif /* ifndef __SYSTEM_IO_FILEIO_UNIQUEHANDLE_H */
