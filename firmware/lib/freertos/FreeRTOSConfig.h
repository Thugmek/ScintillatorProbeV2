#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32h7xx_hal.h"

/* --- Core configuration ------------------------------------------------- */
#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      (480000000UL)
#define configTICK_RATE_HZ                      (1000U)
#define configMAX_PRIORITIES                    8
#define configMINIMAL_STACK_SIZE                ((uint16_t)256)
#define configTOTAL_HEAP_SIZE                   ((size_t)(32 * 1024))
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TASK_NOTIFICATIONS            1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           0
#define configQUEUE_REGISTRY_SIZE               0
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIME_SLICING                  1
#define configSTACK_DEPTH_TYPE                  uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t
#define configENABLE_BACKWARD_COMPATIBILITY     0

/* --- Memory allocation -------------------------------------------------- */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1

/* --- Hook functions ----------------------------------------------------- */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1

/* --- Run-time stats ----------------------------------------------------- */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* --- Co-routine (unused) ------------------------------------------------ */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         2

/* --- Software timer ----------------------------------------------------- */
#define configUSE_TIMERS                        0
#define configTIMER_TASK_PRIORITY               2
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE

/* --- Cortex-M7 interrupt nesting ---------------------------------------- */
/* STM32H7 uses 4 priority bits (16 levels, 0 = highest). */
#ifdef __NVIC_PRIO_BITS
  #define configPRIO_BITS __NVIC_PRIO_BITS
#else
  #define configPRIO_BITS 4
#endif

/* Lowest priority for kernel-aware interrupts (shifted into upper bits).
   Priority 5..15 can use FreeRTOS API from ISR; 0..4 cannot. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* --- Assert ------------------------------------------------------------- */
#define configASSERT(x) if ((x) == 0) { taskDISABLE_INTERRUPTS(); for (;;); }

/* --- FreeRTOS API includes ---------------------------------------------- */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1

/* --- Map FreeRTOS port handlers to Cortex-M vector names ---------------- */
#define vPortSVCHandler         SVC_Handler
#define xPortPendSVHandler      PendSV_Handler
/* SysTick_Handler is written manually in main.cpp so it can call HAL_IncTick()
   and guard the FreeRTOS tick until the scheduler is actually running. */

#endif /* FREERTOS_CONFIG_H */
