#include <System/ABI/Atomic/Atomic.hh>

#include <System/C++/TypeTraits/TypeTraits.hh>
//#include <System/HW/CPU/CPUID/Arch/x86/CPUID.hh>


namespace System::ABI::Atomic
{


static constexpr uint32_t MWAIT_EXT_WAKE_ON_DISABLED_INTERRUPT      = 0x00000001;

static constexpr uint32_t MWAITX_EXT_WAKE_ON_DISABLED_INTERRUPT     = MWAIT_EXT_WAKE_ON_DISABLED_INTERRUPT;
static constexpr uint32_t MWAITX_EXT_TIMED_WAIT                     = 0x00000002;


static inline void monitor(const volatile void* address, uint32_t extensions = 0, uint32_t hints = 0)
{
    asm volatile
    (
        "monitor"
        :
        : "a" (address),
          "c" (extensions),
          "d" (hints)
        : "memory"
    );
}

static inline void mwait(uint32_t extensions = 0, uint32_t hints = 0)
{
    // The mwait opcode will block execution until the monitor is triggered. It may also wake up spuriously due to
    // events such as interrupts.
    asm volatile
    (
        "mwait"
        :
        : "a" (hints),
          "c" (extensions)
        : "memory"
    );
}


static inline void monitorx(const volatile void* address, uint32_t extensions = 0, uint32_t hints = 0)
{
    asm volatile
    (
        "monitorx"
        :
        : "a" (address),
          "c" (extensions),
          "d" (hints)
        : "memory"
    );
}

// Note: do not call with the MWAITX_EXT_TIMED_WAIT flag; use mwaitx_for instead.
static inline void mwaitx(uint32_t extensions = 0, uint32_t hints = 0)
{
    // The mwaitx opcode will block execution until the monitor is triggered. It may also wake up spuriously due to
    // events such as interrupts.
    asm volatile
    (
        "mwaitx"
        :
        : "a" (hints),
          "c" (extensions)
        : "memory"
    );
}

static inline void mwaitx_for(uint32_t cycles, uint32_t extensions = 0, uint32_t hints = 0)
{
    // The mwaitx opcode will block execution until the monitor is triggered. It may also wake up spuriously due to
    // events such as interrupts. This variant will also eventually time out.
    extensions |= MWAITX_EXT_TIMED_WAIT;
    asm volatile
    (
        "mwaitx"
        :
        : "a" (hints),
          "c" (extensions),
          "b" (cycles)
        : "memory"
    );
}


template <class T, class Condition, class Monitor, class Mwait>
static inline void generic_monitor_mwait_once(T* ptr, Condition&& cond, Monitor&& monitor_fn, Mwait&& mwait_fn)
{
    // Set up the monitor then re-check the condition as it may have become false while the monitor was being set up.
    monitor_fn(ptr);
    if (!cond(ptr))
        return;

    // This wait is subject to spurious wakeups but this function explicitly allows that.
    mwait_fn();
}

template <class T, class Condition, class Monitor, class Mwait>
static inline void generic_monitor_mwait_while(T* ptr, Condition&& cond, Monitor&& monitor_fn, Mwait&& mwait_fn)
{
    while (cond(ptr))
    {
        // Set up the monitor then re-check the condition as the condition may have become true while the monitor was
        // being initialised.
        monitor_fn(ptr);
        if (cond(ptr))
            mwait_fn();
        else
            break;
    }
}

template <class T, class Monitor, class Mwait>
static inline void generic_monitor_mwait_while_unchanged(T* ptr, Monitor&& monitor_fn, Mwait&& mwait_fn)
{
    std::remove_cv_t<T> old_value;
    __atomic_load(ptr, &old_value, __memory_order_relaxed);
    generic_monitor_mwait_while(ptr, [old_value](T* p)
    {
        std::remove_cv_t<T> current_value;
        __atomic_load(p, &current_value, __memory_order_relaxed);
        return (current_value == old_value);
    }, monitor_fn, mwait_fn);
}


template <class T>
static inline void monitor_mwait_once(T* ptr)
{
    std::remove_cv_t<T> old_value;
    __atomic_load(ptr, &old_value, __memory_order_relaxed);

    auto monitor_fn = [](const volatile void*p) { monitor(p); };
    auto mwait_fn = []() { mwait(); };
    auto condition = [old_value](T* p)
    { 
        std::remove_cv_t<T> current_value;
        __atomic_load(p, &current_value, __memory_order_relaxed);
        return (current_value == old_value);
    };
    generic_monitor_mwait_once(ptr, condition, monitor_fn, mwait_fn);
}

template <class T>
static inline void monitor_mwait_while_unchanged(T* ptr)
{
    auto monitor_fn = [](const volatile void* p) { monitor(p); };
    auto mwait_fn = []() { mwait(); };
    generic_monitor_mwait_while_unchanged(ptr, monitor_fn, mwait_fn);
}

template <class T>
static inline void monitorx_mwaitx_while_unchanged(T* ptr)
{
    auto monitorx_fn = [](const volatile void* p) { monitorx(p); };
    auto mwaitx_fn = []() { mwaitx(); };
    generic_monitor_mwait_while_unchanged(ptr, monitorx_fn, mwaitx_fn);
}

template <class T>
static inline void monitorx_mwaitx_while_unchanged_for(T* ptr, uint32_t cycles)
{
    auto monitorx_fn = [](const volatile void* p) { monitorx(p); };
    auto mwaitx_fn = [cycles]() { mwaitx_for(cycles); };
    generic_monitor_mwait_while_unchanged(ptr, monitorx_fn, mwaitx_fn);
}


} // namespace System::ABI::Atomic
