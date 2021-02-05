#include <System/ABI/C++/Guard.hh>

//#include <System/IPC/Sync/WaitOnAddress.hh>


namespace __cxxabiv1
{


struct guard_variable
{
    std::uint8_t    status      = 0;
    std::uint8_t    padding[3]  = {0, 0, 0};
    std::uint32_t   mutex       = 0;
};

static inline guard_variable* getGuard(std::int64_t* var)
{
    return reinterpret_cast<guard_variable*>(var);
}


/*int __cxa_guard_acquire(std::int64_t* ptr)
{
    auto* guard = getGuard(ptr);

}*/


} // namespace __cxxabiv1
