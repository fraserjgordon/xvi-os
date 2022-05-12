#ifndef __SYSTEM_CONCURRENCY_SPINLOCKHINT_H
#define __SYSTEM_CONCURRENCY_SPINLOCKHINT_H


namespace System::Concurrency
{


#if defined(__aarch64__)

#elif defined(__arm__)

#elif defined(__mips64)

#elif defined(__mips__)

#elif defined(_ARCH_PPC64)

#elif defined(_ARCH_PPC)

#elif defined(__sparc_v9__)

#elif defined(__sparc__)

#elif defined(__x86_64__)
#  define SPINLOCK_YIELD_HINT   asm volatile ("pause")
#elif defined(__i386__)
#  define SPINLOCK_YIELD_HINT   asm volatile ("pause")
#endif


#ifndef SPINLOCK_YIELD_HINT
#  define SPINLOCK_YIELD_HINT
#endif


} // namespace System::Concurrency


#endif /* ifndef __SYSTEM_CONCURRENCY_SPINLOCKHINT_H */
