#ifndef __SYSTEM_IO_FILEIO_SHAREDHANDLE_H
#define __SYSTEM_IO_FILEIO_SHAREDHANDLE_H


#include <memory>
#include <system_error>
#include <typeinfo>
#include <type_traits>

#include <System/IO/FileIO/UniqueHandle.hh>


namespace System::IO::FileIO
{


// Forward declarations.
class Handle;


class SharedHandleBase
{
protected:

    template <class T> using HandleData = UniqueHandleBase::HandleData<T>;
    template <class T> using HandleOps = UniqueHandleBase::HandleOps<T>;
};


template <std::derived_from<Handle> T>
class SharedHandle :
    protected SharedHandleBase
{
    template <std::derived_from<Handle> U> friend class SharedHandle;

public:

    constexpr SharedHandle() noexcept = default;
    SharedHandle(const SharedHandle&) = default;
    SharedHandle(SharedHandle&&) noexcept = default;

    SharedHandle(T handle) :
        m_handle(wrap(std::move(handle)))
    {
    }

    template <std::derived_from<T> U>
    SharedHandle(U handle) :
        m_handle(wrap(std::move(handle)))
    {
    }

    template <std::derived_from<T> U>
    SharedHandle(const SharedHandle<U>& handle) :
        m_handle(convert(handle.m_handle))
    {
    }

    template <std::derived_from<T> U>
    SharedHandle(SharedHandle<U>&& handle) :
        m_handle(convert(std::move(handle.m_handle)))
    {
    }

    SharedHandle(UniqueHandle<T> handle) :
        m_handle(std::move(handle.m_handle))
    {
    }

    template <std::derived_from<T> U>
    SharedHandle(UniqueHandle<U> handle) :
        m_handle(std::move(handle.m_handle))
    {
    }

    constexpr SharedHandle(std::nullptr_t) noexcept :
        SharedHandle()
    {
    }

    ~SharedHandle() = default;

    SharedHandle& operator=(const SharedHandle&) = default;
    SharedHandle& operator=(SharedHandle&&) noexcept = default;

    SharedHandle& operator=(std::nullptr_t) noexcept
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

    std::shared_ptr<HandleData<T>> m_handle = nullptr;


    template <std::derived_from<T> U>
    static std::shared_ptr<HandleData<T>> convert(const std::shared_ptr<HandleData<U>>& ptr)
    {
        return HandleData<U>::template convertTo<T>(ptr);
    }

    template <std::derived_from<T> U>
    static std::shared_ptr<HandleData<T>> convert(std::shared_ptr<HandleData<U>>&& ptr)
    {
        return HandleData<U>::template convertTo<T>(std::move(ptr));
    }

    static std::shared_ptr<HandleData<T>> wrap(T handle)
    {
        return HandleData<T>::makeShared(std::move(handle));
    }

    template <std::derived_from<T> U>
    static std::shared_ptr<HandleData<T>> wrap(U handle)
    {
        return convert(HandleData<U>::makeShared(std::move(handle)));
    }
};


} // namespace System::IO::FileHandle


#endif /* ifndef __SYSTEM_IO_FILEIO_SHAREDHANDLE_H */
