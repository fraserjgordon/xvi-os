#ifndef __SYSTEM_CONCURRENCY_SEQLOCK_H
#define __SYSTEM_CONCURRENCY_SEQLOCK_H


#include <System/Concurrency/Private/Config.hh>

#include <concepts>
#include <cstdint>
#include <functional>


namespace System::Concurrency
{


struct seqlock_t { std::uintptr_t opaque = 0; };

struct seqlock_cookie_t { std::uintptr_t opaque = 0; };

__SYSTEM_CONCURRENCY_EXPORT seqlock_cookie_t SeqlockReadAcquire(seqlock_t*) __SYSTEM_CONCURRENCY_SYMBOL(SeqlockReadAcquire);
__SYSTEM_CONCURRENCY_EXPORT bool SeqlockReadValid(seqlock_t*, seqlock_cookie_t) __SYSTEM_CONCURRENCY_SYMBOL(SeqlockReadValid);

__SYSTEM_CONCURRENCY_EXPORT void SeqlockWriteAcquire(seqlock_t*) __SYSTEM_CONCURRENCY_SYMBOL(SeqlockWriteAcquire);
__SYSTEM_CONCURRENCY_EXPORT void SeqlockWriteRelease(seqlock_t*) __SYSTEM_CONCURRENCY_SYMBOL(SeqlockWriteRelease);


class Seqlock
{
public:

    constexpr Seqlock() = default;
    constexpr ~Seqlock() = default;

    Seqlock(const Seqlock&) = delete;
    Seqlock(Seqlock&&) = delete;

    Seqlock& operator=(const Seqlock&) = delete;
    Seqlock& operator=(Seqlock&&) = delete;


    template <class Callable>
        requires std::invocable<Callable>
    bool tryReadOperation(Callable&& callable)
    {
        auto cookie = SeqlockReadAcquire(&m_seqlock);
        std::invoke(std::forward<Callable>(callable));
        return SeqlockReadValid(&m_seqlock, cookie);
    }

    template <class Callable>
        requires std::invocable<Callable>
    auto readOperation(Callable callable)
    {
        while (true)
        {
            auto cookie = SeqlockReadAcquire(&m_seqlock);
            auto result = std::invoke(callable);
            if (SeqlockReadValid(&m_seqlock, cookie)) [[likely]]
                return result;
        }
    }

    template <class Callable>
        requires std::invocable<Callable>
    auto withWriteLock(Callable&& callable)
    {
        SeqlockWriteAcquire(&m_seqlock);
        auto result = std::invoke(std::forward<Callable>(callable));
        SeqlockWriteRelease(&m_seqlock);
        return result;
    }

private:

    seqlock_t m_seqlock = {};
};


template <class T>
class Seqlocked
{
public:

    Seqlocked() = default;
    ~Seqlocked() = default;

    Seqlocked(const Seqlocked&) = delete;
    Seqlocked(Seqlocked&&) = delete;

    Seqlocked& operator=(const Seqlocked&) = delete;
    Seqlocked& operator=(Seqlocked&&) = delete;


    template <class Callable>
        requires std::invocable<Callable, const T&>
    bool tryReadOperation(Callable&& callable) const
    {
        return m_lock.tryReadOperation([this, &callable]()
        {
            std::invoke(std::forward<Callable>(callable), m_data);
        });
    }

    template <class Callable>
        requires std::invocable<Callable, const T&>
    auto readOperation(Callable&& callable) const
    {
        return m_lock.readOperation([this, &callable]()
        {
            return std::invoke(std::forward<Callable>(callable), m_data);
        });
    }

    template <class Callable>
        requires std::invocable<Callable, T&>
    auto withWriteLock(Callable&& callable)
    {
        return m_lock.withWriteLock([this, &callable] mutable
        {
            return std::invoke(std::forward<Callable>(callable), m_data);
        });
    }

    T read() const
    {
        return readOperation([](const T& data)
        {
            return data;
        });
    }

    void write(const T& newdata)
    {
        withWriteLock([&newdata](T& data)
        {
            data = newdata;
        });
    }

    void write(T&& newdata)
    {
        withWriteLock([&newdata](T& data)
        {
            data = std::move(newdata);
        });
    }

private:

    mutable Seqlock     m_lock = {};
    T                   m_data = {};
};


} // namespace System::Concurrency


#endif /* ifndef __SYSTEM_CONCURRENCY_SEQLOCK_H */
