#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H

#ifdef __IAR_SYSTEMS_ICC__
  #include <intrinsics.h>
#endif

// Up-channel 0: Terminal
#ifndef SEGGER_RTT_MAX_NUM_UP_BUFFERS
  #define SEGGER_RTT_MAX_NUM_UP_BUFFERS   (1)
#endif

// Down-channel 0: Terminal
#ifndef SEGGER_RTT_MAX_NUM_DOWN_BUFFERS
  #define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS (1)
#endif

#ifndef BUFFER_SIZE_UP
  #define BUFFER_SIZE_UP   (1024)
#endif

#ifndef BUFFER_SIZE_DOWN
  #define BUFFER_SIZE_DOWN (16)
#endif

#ifndef SEGGER_RTT_PRINTF_BUFFER_SIZE
  #define SEGGER_RTT_PRINTF_BUFFER_SIZE (64u)
#endif

#ifndef SEGGER_RTT_MODE_DEFAULT
  #define SEGGER_RTT_MODE_DEFAULT SEGGER_RTT_MODE_NO_BLOCK_SKIP
#endif

#ifndef SEGGER_RTT_MEMCPY_USE_BYTELOOP
  #define SEGGER_RTT_MEMCPY_USE_BYTELOOP 0
#endif

/*********************************************************************
 *  RTT lock configuration for GCC / Cortex-M7
 *
 *  Uses BASEPRI to mask interrupts up to a priority threshold,
 *  keeping higher-priority interrupts unaffected.
 */
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__CC_ARM) && !defined(WIN32)
  #if defined(__ARM_ARCH_7EM__) // Cortex-M4 / M7
    #ifndef SEGGER_RTT_MAX_INTERRUPT_PRIORITY
      #define SEGGER_RTT_MAX_INTERRUPT_PRIORITY (0x20)
    #endif

    #define SEGGER_RTT_LOCK()                                       \
        {                                                           \
            unsigned int _SEGGER_RTT__LockState;                    \
            __asm volatile("mrs   %0, basepri  \n\t"                \
                           "mov   r1, %1       \n\t"                \
                           "msr   basepri, r1  \n\t"                \
                           : "=r"(_SEGGER_RTT__LockState)           \
                           : "i"(SEGGER_RTT_MAX_INTERRUPT_PRIORITY) \
                           : "r1", "cc");

    #define SEGGER_RTT_UNLOCK()                      \
            __asm volatile("msr   basepri, %0  \n\t" \
                           :                         \
                           : "r"(_SEGGER_RTT__LockState) \
                           :);                       \
        }
  #endif
#endif

// Fallback: no locking
#ifndef SEGGER_RTT_LOCK
  #define SEGGER_RTT_LOCK()
#endif
#ifndef SEGGER_RTT_UNLOCK
  #define SEGGER_RTT_UNLOCK()
#endif

#endif // SEGGER_RTT_CONF_H
