/**
 * @file   startup_stm32h753xx.s
 * @brief  STM32H753xx vector table and startup code for GCC toolchain.
 *
 *         After reset the Cortex-M7 processor is in Thread mode,
 *         priority is Privileged, and the Stack is set to Main.
 */

  .syntax unified
  .cpu cortex-m7
  .fpu fpv5-d16
  .thumb

.global g_pfnVectors
.global Default_Handler

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr  sp, =_estack

/* Copy .data from flash to RAM */
  movs r1, #0
  b    LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit

/* Zero fill .bss */
  ldr  r2, =_sbss
  b    LoopFillZerobss

FillZerobss:
  movs r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, =_ebss
  cmp  r2, r3
  bcc  FillZerobss

/* Call main */
  bl   main
  bx   lr
  .size Reset_Handler, .-Reset_Handler

/**
 * @brief Default handler for unimplemented interrupts.
 */
  .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

/******************************************************************************
 * Vector table for STM32H753xx
 ******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

  /* External Interrupts */
  .word WWDG_IRQHandler                    /*  0: Window WatchDog */
  .word PVD_AVD_IRQHandler                 /*  1: PVD/AVD through EXTI */
  .word TAMP_STAMP_IRQHandler              /*  2: Tamper and TimeStamps */
  .word RTC_WKUP_IRQHandler                /*  3: RTC Wakeup */
  .word FLASH_IRQHandler                   /*  4: FLASH */
  .word RCC_IRQHandler                     /*  5: RCC */
  .word EXTI0_IRQHandler                   /*  6: EXTI Line0 */
  .word EXTI1_IRQHandler                   /*  7: EXTI Line1 */
  .word EXTI2_IRQHandler                   /*  8: EXTI Line2 */
  .word EXTI3_IRQHandler                   /*  9: EXTI Line3 */
  .word EXTI4_IRQHandler                   /* 10: EXTI Line4 */
  .word DMA1_Stream0_IRQHandler            /* 11: DMA1 Stream 0 */
  .word DMA1_Stream1_IRQHandler            /* 12: DMA1 Stream 1 */
  .word DMA1_Stream2_IRQHandler            /* 13: DMA1 Stream 2 */
  .word DMA1_Stream3_IRQHandler            /* 14: DMA1 Stream 3 */
  .word DMA1_Stream4_IRQHandler            /* 15: DMA1 Stream 4 */
  .word DMA1_Stream5_IRQHandler            /* 16: DMA1 Stream 5 */
  .word DMA1_Stream6_IRQHandler            /* 17: DMA1 Stream 6 */
  .word ADC_IRQHandler                     /* 18: ADC1 & ADC2 */
  .word FDCAN1_IT0_IRQHandler              /* 19: FDCAN1 interrupt 0 */
  .word FDCAN2_IT0_IRQHandler              /* 20: FDCAN2 interrupt 0 */
  .word FDCAN1_IT1_IRQHandler              /* 21: FDCAN1 interrupt 1 */
  .word FDCAN2_IT1_IRQHandler              /* 22: FDCAN2 interrupt 1 */
  .word EXTI9_5_IRQHandler                 /* 23: EXTI Lines [9:5] */
  .word TIM1_BRK_IRQHandler                /* 24: TIM1 Break */
  .word TIM1_UP_IRQHandler                 /* 25: TIM1 Update */
  .word TIM1_TRG_COM_IRQHandler            /* 26: TIM1 Trigger and Commutation */
  .word TIM1_CC_IRQHandler                 /* 27: TIM1 Capture Compare */
  .word TIM2_IRQHandler                    /* 28: TIM2 */
  .word TIM3_IRQHandler                    /* 29: TIM3 */
  .word TIM4_IRQHandler                    /* 30: TIM4 */
  .word I2C1_EV_IRQHandler                 /* 31: I2C1 Event */
  .word I2C1_ER_IRQHandler                 /* 32: I2C1 Error */
  .word I2C2_EV_IRQHandler                 /* 33: I2C2 Event */
  .word I2C2_ER_IRQHandler                 /* 34: I2C2 Error */
  .word SPI1_IRQHandler                    /* 35: SPI1 */
  .word SPI2_IRQHandler                    /* 36: SPI2 */
  .word USART1_IRQHandler                  /* 37: USART1 */
  .word USART2_IRQHandler                  /* 38: USART2 */
  .word USART3_IRQHandler                  /* 39: USART3 */
  .word EXTI15_10_IRQHandler               /* 40: EXTI Lines [15:10] */
  .word RTC_Alarm_IRQHandler               /* 41: RTC Alarm (A and B) */
  .word 0                                  /* 42: Reserved */
  .word TIM8_BRK_TIM12_IRQHandler          /* 43: TIM8 Break and TIM12 */
  .word TIM8_UP_TIM13_IRQHandler           /* 44: TIM8 Update and TIM13 */
  .word TIM8_TRG_COM_TIM14_IRQHandler      /* 45: TIM8 Trigger/Commutation and TIM14 */
  .word TIM8_CC_IRQHandler                 /* 46: TIM8 Capture Compare */
  .word DMA1_Stream7_IRQHandler            /* 47: DMA1 Stream7 */
  .word FMC_IRQHandler                     /* 48: FMC */
  .word SDMMC1_IRQHandler                  /* 49: SDMMC1 */
  .word TIM5_IRQHandler                    /* 50: TIM5 */
  .word SPI3_IRQHandler                    /* 51: SPI3 */
  .word UART4_IRQHandler                   /* 52: UART4 */
  .word UART5_IRQHandler                   /* 53: UART5 */
  .word TIM6_DAC_IRQHandler                /* 54: TIM6 and DAC1&2 underrun */
  .word TIM7_IRQHandler                    /* 55: TIM7 */
  .word DMA2_Stream0_IRQHandler            /* 56: DMA2 Stream 0 */
  .word DMA2_Stream1_IRQHandler            /* 57: DMA2 Stream 1 */
  .word DMA2_Stream2_IRQHandler            /* 58: DMA2 Stream 2 */
  .word DMA2_Stream3_IRQHandler            /* 59: DMA2 Stream 3 */
  .word DMA2_Stream4_IRQHandler            /* 60: DMA2 Stream 4 */
  .word ETH_IRQHandler                     /* 61: Ethernet */
  .word ETH_WKUP_IRQHandler                /* 62: Ethernet Wakeup */
  .word FDCAN_CAL_IRQHandler               /* 63: FDCAN calibration */
  .word 0                                  /* 64: Reserved */
  .word 0                                  /* 65: Reserved */
  .word 0                                  /* 66: Reserved */
  .word 0                                  /* 67: Reserved */
  .word DMA2_Stream5_IRQHandler            /* 68: DMA2 Stream 5 */
  .word DMA2_Stream6_IRQHandler            /* 69: DMA2 Stream 6 */
  .word DMA2_Stream7_IRQHandler            /* 70: DMA2 Stream 7 */
  .word USART6_IRQHandler                  /* 71: USART6 */
  .word I2C3_EV_IRQHandler                 /* 72: I2C3 Event */
  .word I2C3_ER_IRQHandler                 /* 73: I2C3 Error */
  .word OTG_HS_EP1_OUT_IRQHandler          /* 74: USB OTG HS End Point 1 Out */
  .word OTG_HS_EP1_IN_IRQHandler           /* 75: USB OTG HS End Point 1 In */
  .word OTG_HS_WKUP_IRQHandler             /* 76: USB OTG HS Wakeup */
  .word OTG_HS_IRQHandler                  /* 77: USB OTG HS */
  .word DCMI_IRQHandler                    /* 78: DCMI */
  .word CRYP_IRQHandler                    /* 79: CRYP */
  .word HASH_RNG_IRQHandler                /* 80: Hash and RNG */
  .word FPU_IRQHandler                     /* 81: FPU */
  .word UART7_IRQHandler                   /* 82: UART7 */
  .word UART8_IRQHandler                   /* 83: UART8 */
  .word SPI4_IRQHandler                    /* 84: SPI4 */
  .word SPI5_IRQHandler                    /* 85: SPI5 */
  .word SPI6_IRQHandler                    /* 86: SPI6 */
  .word SAI1_IRQHandler                    /* 87: SAI1 */
  .word LTDC_IRQHandler                    /* 88: LTDC */
  .word LTDC_ER_IRQHandler                 /* 89: LTDC Error */
  .word DMA2D_IRQHandler                   /* 90: DMA2D */
  .word SAI2_IRQHandler                    /* 91: SAI2 */
  .word QUADSPI_IRQHandler                 /* 92: QUADSPI */
  .word LPTIM1_IRQHandler                  /* 93: LPTIM1 */
  .word CEC_IRQHandler                     /* 94: HDMI-CEC */
  .word I2C4_EV_IRQHandler                 /* 95: I2C4 Event */
  .word I2C4_ER_IRQHandler                 /* 96: I2C4 Error */
  .word SPDIF_RX_IRQHandler                /* 97: SPDIF-RX */
  .word OTG_FS_EP1_OUT_IRQHandler          /* 98: USB OTG FS End Point 1 Out */
  .word OTG_FS_EP1_IN_IRQHandler           /* 99: USB OTG FS End Point 1 In */
  .word OTG_FS_WKUP_IRQHandler             /* 100: USB OTG FS Wakeup */
  .word OTG_FS_IRQHandler                  /* 101: USB OTG FS */
  .word DMAMUX1_OVR_IRQHandler             /* 102: DMAMUX1 Overrun */
  .word HRTIM1_Master_IRQHandler           /* 103: HRTIM Master Timer */
  .word HRTIM1_TIMA_IRQHandler             /* 104: HRTIM Timer A */
  .word HRTIM1_TIMB_IRQHandler             /* 105: HRTIM Timer B */
  .word HRTIM1_TIMC_IRQHandler             /* 106: HRTIM Timer C */
  .word HRTIM1_TIMD_IRQHandler             /* 107: HRTIM Timer D */
  .word HRTIM1_TIME_IRQHandler             /* 108: HRTIM Timer E */
  .word HRTIM1_FLT_IRQHandler              /* 109: HRTIM Fault */
  .word DFSDM1_FLT0_IRQHandler             /* 110: DFSDM Filter0 */
  .word DFSDM1_FLT1_IRQHandler             /* 111: DFSDM Filter1 */
  .word DFSDM1_FLT2_IRQHandler             /* 112: DFSDM Filter2 */
  .word DFSDM1_FLT3_IRQHandler             /* 113: DFSDM Filter3 */
  .word SAI3_IRQHandler                    /* 114: SAI3 */
  .word SWPMI1_IRQHandler                  /* 115: SWPMI1 */
  .word TIM15_IRQHandler                   /* 116: TIM15 */
  .word TIM16_IRQHandler                   /* 117: TIM16 */
  .word TIM17_IRQHandler                   /* 118: TIM17 */
  .word MDIOS_WKUP_IRQHandler              /* 119: MDIOS Wakeup */
  .word MDIOS_IRQHandler                   /* 120: MDIOS */
  .word JPEG_IRQHandler                    /* 121: JPEG */
  .word MDMA_IRQHandler                    /* 122: MDMA */
  .word 0                                  /* 123: Reserved */
  .word SDMMC2_IRQHandler                  /* 124: SDMMC2 */
  .word HSEM1_IRQHandler                   /* 125: HSEM1 */
  .word 0                                  /* 126: Reserved */
  .word ADC3_IRQHandler                    /* 127: ADC3 */
  .word DMAMUX2_OVR_IRQHandler             /* 128: DMAMUX2 Overrun */
  .word BDMA_Channel0_IRQHandler           /* 129: BDMA Channel 0 */
  .word BDMA_Channel1_IRQHandler           /* 130: BDMA Channel 1 */
  .word BDMA_Channel2_IRQHandler           /* 131: BDMA Channel 2 */
  .word BDMA_Channel3_IRQHandler           /* 132: BDMA Channel 3 */
  .word BDMA_Channel4_IRQHandler           /* 133: BDMA Channel 4 */
  .word BDMA_Channel5_IRQHandler           /* 134: BDMA Channel 5 */
  .word BDMA_Channel6_IRQHandler           /* 135: BDMA Channel 6 */
  .word BDMA_Channel7_IRQHandler           /* 136: BDMA Channel 7 */
  .word COMP1_IRQHandler                   /* 137: COMP1 */
  .word LPTIM2_IRQHandler                  /* 138: LPTIM2 */
  .word LPTIM3_IRQHandler                  /* 139: LPTIM3 */
  .word LPTIM4_IRQHandler                  /* 140: LPTIM4 */
  .word LPTIM5_IRQHandler                  /* 141: LPTIM5 */
  .word LPUART1_IRQHandler                 /* 142: LPUART1 */
  .word 0                                  /* 143: Reserved */
  .word CRS_IRQHandler                     /* 144: CRS */
  .word ECC_IRQHandler                     /* 145: ECC */
  .word SAI4_IRQHandler                    /* 146: SAI4 */
  .word 0                                  /* 147: Reserved */
  .word 0                                  /* 148: Reserved */
  .word WAKEUP_PIN_IRQHandler              /* 149: Wakeup Pin */

/******************************************************************************
 * Weak aliases - any function with the same name overrides these
 ******************************************************************************/
  .weak      NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak      HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak      MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak      BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak      UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak      SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak      DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

  .weak      PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak      SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak      WWDG_IRQHandler
  .thumb_set WWDG_IRQHandler,Default_Handler

  .weak      PVD_AVD_IRQHandler
  .thumb_set PVD_AVD_IRQHandler,Default_Handler

  .weak      TAMP_STAMP_IRQHandler
  .thumb_set TAMP_STAMP_IRQHandler,Default_Handler

  .weak      RTC_WKUP_IRQHandler
  .thumb_set RTC_WKUP_IRQHandler,Default_Handler

  .weak      FLASH_IRQHandler
  .thumb_set FLASH_IRQHandler,Default_Handler

  .weak      RCC_IRQHandler
  .thumb_set RCC_IRQHandler,Default_Handler

  .weak      EXTI0_IRQHandler
  .thumb_set EXTI0_IRQHandler,Default_Handler

  .weak      EXTI1_IRQHandler
  .thumb_set EXTI1_IRQHandler,Default_Handler

  .weak      EXTI2_IRQHandler
  .thumb_set EXTI2_IRQHandler,Default_Handler

  .weak      EXTI3_IRQHandler
  .thumb_set EXTI3_IRQHandler,Default_Handler

  .weak      EXTI4_IRQHandler
  .thumb_set EXTI4_IRQHandler,Default_Handler

  .weak      DMA1_Stream0_IRQHandler
  .thumb_set DMA1_Stream0_IRQHandler,Default_Handler

  .weak      DMA1_Stream1_IRQHandler
  .thumb_set DMA1_Stream1_IRQHandler,Default_Handler

  .weak      DMA1_Stream2_IRQHandler
  .thumb_set DMA1_Stream2_IRQHandler,Default_Handler

  .weak      DMA1_Stream3_IRQHandler
  .thumb_set DMA1_Stream3_IRQHandler,Default_Handler

  .weak      DMA1_Stream4_IRQHandler
  .thumb_set DMA1_Stream4_IRQHandler,Default_Handler

  .weak      DMA1_Stream5_IRQHandler
  .thumb_set DMA1_Stream5_IRQHandler,Default_Handler

  .weak      DMA1_Stream6_IRQHandler
  .thumb_set DMA1_Stream6_IRQHandler,Default_Handler

  .weak      ADC_IRQHandler
  .thumb_set ADC_IRQHandler,Default_Handler

  .weak      FDCAN1_IT0_IRQHandler
  .thumb_set FDCAN1_IT0_IRQHandler,Default_Handler

  .weak      FDCAN2_IT0_IRQHandler
  .thumb_set FDCAN2_IT0_IRQHandler,Default_Handler

  .weak      FDCAN1_IT1_IRQHandler
  .thumb_set FDCAN1_IT1_IRQHandler,Default_Handler

  .weak      FDCAN2_IT1_IRQHandler
  .thumb_set FDCAN2_IT1_IRQHandler,Default_Handler

  .weak      EXTI9_5_IRQHandler
  .thumb_set EXTI9_5_IRQHandler,Default_Handler

  .weak      TIM1_BRK_IRQHandler
  .thumb_set TIM1_BRK_IRQHandler,Default_Handler

  .weak      TIM1_UP_IRQHandler
  .thumb_set TIM1_UP_IRQHandler,Default_Handler

  .weak      TIM1_TRG_COM_IRQHandler
  .thumb_set TIM1_TRG_COM_IRQHandler,Default_Handler

  .weak      TIM1_CC_IRQHandler
  .thumb_set TIM1_CC_IRQHandler,Default_Handler

  .weak      TIM2_IRQHandler
  .thumb_set TIM2_IRQHandler,Default_Handler

  .weak      TIM3_IRQHandler
  .thumb_set TIM3_IRQHandler,Default_Handler

  .weak      TIM4_IRQHandler
  .thumb_set TIM4_IRQHandler,Default_Handler

  .weak      I2C1_EV_IRQHandler
  .thumb_set I2C1_EV_IRQHandler,Default_Handler

  .weak      I2C1_ER_IRQHandler
  .thumb_set I2C1_ER_IRQHandler,Default_Handler

  .weak      I2C2_EV_IRQHandler
  .thumb_set I2C2_EV_IRQHandler,Default_Handler

  .weak      I2C2_ER_IRQHandler
  .thumb_set I2C2_ER_IRQHandler,Default_Handler

  .weak      SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler

  .weak      SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler

  .weak      USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler

  .weak      USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler

  .weak      USART3_IRQHandler
  .thumb_set USART3_IRQHandler,Default_Handler

  .weak      EXTI15_10_IRQHandler
  .thumb_set EXTI15_10_IRQHandler,Default_Handler

  .weak      RTC_Alarm_IRQHandler
  .thumb_set RTC_Alarm_IRQHandler,Default_Handler

  .weak      TIM8_BRK_TIM12_IRQHandler
  .thumb_set TIM8_BRK_TIM12_IRQHandler,Default_Handler

  .weak      TIM8_UP_TIM13_IRQHandler
  .thumb_set TIM8_UP_TIM13_IRQHandler,Default_Handler

  .weak      TIM8_TRG_COM_TIM14_IRQHandler
  .thumb_set TIM8_TRG_COM_TIM14_IRQHandler,Default_Handler

  .weak      TIM8_CC_IRQHandler
  .thumb_set TIM8_CC_IRQHandler,Default_Handler

  .weak      DMA1_Stream7_IRQHandler
  .thumb_set DMA1_Stream7_IRQHandler,Default_Handler

  .weak      FMC_IRQHandler
  .thumb_set FMC_IRQHandler,Default_Handler

  .weak      SDMMC1_IRQHandler
  .thumb_set SDMMC1_IRQHandler,Default_Handler

  .weak      TIM5_IRQHandler
  .thumb_set TIM5_IRQHandler,Default_Handler

  .weak      SPI3_IRQHandler
  .thumb_set SPI3_IRQHandler,Default_Handler

  .weak      UART4_IRQHandler
  .thumb_set UART4_IRQHandler,Default_Handler

  .weak      UART5_IRQHandler
  .thumb_set UART5_IRQHandler,Default_Handler

  .weak      TIM6_DAC_IRQHandler
  .thumb_set TIM6_DAC_IRQHandler,Default_Handler

  .weak      TIM7_IRQHandler
  .thumb_set TIM7_IRQHandler,Default_Handler

  .weak      DMA2_Stream0_IRQHandler
  .thumb_set DMA2_Stream0_IRQHandler,Default_Handler

  .weak      DMA2_Stream1_IRQHandler
  .thumb_set DMA2_Stream1_IRQHandler,Default_Handler

  .weak      DMA2_Stream2_IRQHandler
  .thumb_set DMA2_Stream2_IRQHandler,Default_Handler

  .weak      DMA2_Stream3_IRQHandler
  .thumb_set DMA2_Stream3_IRQHandler,Default_Handler

  .weak      DMA2_Stream4_IRQHandler
  .thumb_set DMA2_Stream4_IRQHandler,Default_Handler

  .weak      ETH_IRQHandler
  .thumb_set ETH_IRQHandler,Default_Handler

  .weak      ETH_WKUP_IRQHandler
  .thumb_set ETH_WKUP_IRQHandler,Default_Handler

  .weak      FDCAN_CAL_IRQHandler
  .thumb_set FDCAN_CAL_IRQHandler,Default_Handler

  .weak      DMA2_Stream5_IRQHandler
  .thumb_set DMA2_Stream5_IRQHandler,Default_Handler

  .weak      DMA2_Stream6_IRQHandler
  .thumb_set DMA2_Stream6_IRQHandler,Default_Handler

  .weak      DMA2_Stream7_IRQHandler
  .thumb_set DMA2_Stream7_IRQHandler,Default_Handler

  .weak      USART6_IRQHandler
  .thumb_set USART6_IRQHandler,Default_Handler

  .weak      I2C3_EV_IRQHandler
  .thumb_set I2C3_EV_IRQHandler,Default_Handler

  .weak      I2C3_ER_IRQHandler
  .thumb_set I2C3_ER_IRQHandler,Default_Handler

  .weak      OTG_HS_EP1_OUT_IRQHandler
  .thumb_set OTG_HS_EP1_OUT_IRQHandler,Default_Handler

  .weak      OTG_HS_EP1_IN_IRQHandler
  .thumb_set OTG_HS_EP1_IN_IRQHandler,Default_Handler

  .weak      OTG_HS_WKUP_IRQHandler
  .thumb_set OTG_HS_WKUP_IRQHandler,Default_Handler

  .weak      OTG_HS_IRQHandler
  .thumb_set OTG_HS_IRQHandler,Default_Handler

  .weak      DCMI_IRQHandler
  .thumb_set DCMI_IRQHandler,Default_Handler

  .weak      CRYP_IRQHandler
  .thumb_set CRYP_IRQHandler,Default_Handler

  .weak      HASH_RNG_IRQHandler
  .thumb_set HASH_RNG_IRQHandler,Default_Handler

  .weak      FPU_IRQHandler
  .thumb_set FPU_IRQHandler,Default_Handler

  .weak      UART7_IRQHandler
  .thumb_set UART7_IRQHandler,Default_Handler

  .weak      UART8_IRQHandler
  .thumb_set UART8_IRQHandler,Default_Handler

  .weak      SPI4_IRQHandler
  .thumb_set SPI4_IRQHandler,Default_Handler

  .weak      SPI5_IRQHandler
  .thumb_set SPI5_IRQHandler,Default_Handler

  .weak      SPI6_IRQHandler
  .thumb_set SPI6_IRQHandler,Default_Handler

  .weak      SAI1_IRQHandler
  .thumb_set SAI1_IRQHandler,Default_Handler

  .weak      LTDC_IRQHandler
  .thumb_set LTDC_IRQHandler,Default_Handler

  .weak      LTDC_ER_IRQHandler
  .thumb_set LTDC_ER_IRQHandler,Default_Handler

  .weak      DMA2D_IRQHandler
  .thumb_set DMA2D_IRQHandler,Default_Handler

  .weak      SAI2_IRQHandler
  .thumb_set SAI2_IRQHandler,Default_Handler

  .weak      QUADSPI_IRQHandler
  .thumb_set QUADSPI_IRQHandler,Default_Handler

  .weak      LPTIM1_IRQHandler
  .thumb_set LPTIM1_IRQHandler,Default_Handler

  .weak      CEC_IRQHandler
  .thumb_set CEC_IRQHandler,Default_Handler

  .weak      I2C4_EV_IRQHandler
  .thumb_set I2C4_EV_IRQHandler,Default_Handler

  .weak      I2C4_ER_IRQHandler
  .thumb_set I2C4_ER_IRQHandler,Default_Handler

  .weak      SPDIF_RX_IRQHandler
  .thumb_set SPDIF_RX_IRQHandler,Default_Handler

  .weak      OTG_FS_EP1_OUT_IRQHandler
  .thumb_set OTG_FS_EP1_OUT_IRQHandler,Default_Handler

  .weak      OTG_FS_EP1_IN_IRQHandler
  .thumb_set OTG_FS_EP1_IN_IRQHandler,Default_Handler

  .weak      OTG_FS_WKUP_IRQHandler
  .thumb_set OTG_FS_WKUP_IRQHandler,Default_Handler

  .weak      OTG_FS_IRQHandler
  .thumb_set OTG_FS_IRQHandler,Default_Handler

  .weak      DMAMUX1_OVR_IRQHandler
  .thumb_set DMAMUX1_OVR_IRQHandler,Default_Handler

  .weak      HRTIM1_Master_IRQHandler
  .thumb_set HRTIM1_Master_IRQHandler,Default_Handler

  .weak      HRTIM1_TIMA_IRQHandler
  .thumb_set HRTIM1_TIMA_IRQHandler,Default_Handler

  .weak      HRTIM1_TIMB_IRQHandler
  .thumb_set HRTIM1_TIMB_IRQHandler,Default_Handler

  .weak      HRTIM1_TIMC_IRQHandler
  .thumb_set HRTIM1_TIMC_IRQHandler,Default_Handler

  .weak      HRTIM1_TIMD_IRQHandler
  .thumb_set HRTIM1_TIMD_IRQHandler,Default_Handler

  .weak      HRTIM1_TIME_IRQHandler
  .thumb_set HRTIM1_TIME_IRQHandler,Default_Handler

  .weak      HRTIM1_FLT_IRQHandler
  .thumb_set HRTIM1_FLT_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT0_IRQHandler
  .thumb_set DFSDM1_FLT0_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT1_IRQHandler
  .thumb_set DFSDM1_FLT1_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT2_IRQHandler
  .thumb_set DFSDM1_FLT2_IRQHandler,Default_Handler

  .weak      DFSDM1_FLT3_IRQHandler
  .thumb_set DFSDM1_FLT3_IRQHandler,Default_Handler

  .weak      SAI3_IRQHandler
  .thumb_set SAI3_IRQHandler,Default_Handler

  .weak      SWPMI1_IRQHandler
  .thumb_set SWPMI1_IRQHandler,Default_Handler

  .weak      TIM15_IRQHandler
  .thumb_set TIM15_IRQHandler,Default_Handler

  .weak      TIM16_IRQHandler
  .thumb_set TIM16_IRQHandler,Default_Handler

  .weak      TIM17_IRQHandler
  .thumb_set TIM17_IRQHandler,Default_Handler

  .weak      MDIOS_WKUP_IRQHandler
  .thumb_set MDIOS_WKUP_IRQHandler,Default_Handler

  .weak      MDIOS_IRQHandler
  .thumb_set MDIOS_IRQHandler,Default_Handler

  .weak      JPEG_IRQHandler
  .thumb_set JPEG_IRQHandler,Default_Handler

  .weak      MDMA_IRQHandler
  .thumb_set MDMA_IRQHandler,Default_Handler

  .weak      SDMMC2_IRQHandler
  .thumb_set SDMMC2_IRQHandler,Default_Handler

  .weak      HSEM1_IRQHandler
  .thumb_set HSEM1_IRQHandler,Default_Handler

  .weak      ADC3_IRQHandler
  .thumb_set ADC3_IRQHandler,Default_Handler

  .weak      DMAMUX2_OVR_IRQHandler
  .thumb_set DMAMUX2_OVR_IRQHandler,Default_Handler

  .weak      BDMA_Channel0_IRQHandler
  .thumb_set BDMA_Channel0_IRQHandler,Default_Handler

  .weak      BDMA_Channel1_IRQHandler
  .thumb_set BDMA_Channel1_IRQHandler,Default_Handler

  .weak      BDMA_Channel2_IRQHandler
  .thumb_set BDMA_Channel2_IRQHandler,Default_Handler

  .weak      BDMA_Channel3_IRQHandler
  .thumb_set BDMA_Channel3_IRQHandler,Default_Handler

  .weak      BDMA_Channel4_IRQHandler
  .thumb_set BDMA_Channel4_IRQHandler,Default_Handler

  .weak      BDMA_Channel5_IRQHandler
  .thumb_set BDMA_Channel5_IRQHandler,Default_Handler

  .weak      BDMA_Channel6_IRQHandler
  .thumb_set BDMA_Channel6_IRQHandler,Default_Handler

  .weak      BDMA_Channel7_IRQHandler
  .thumb_set BDMA_Channel7_IRQHandler,Default_Handler

  .weak      COMP1_IRQHandler
  .thumb_set COMP1_IRQHandler,Default_Handler

  .weak      LPTIM2_IRQHandler
  .thumb_set LPTIM2_IRQHandler,Default_Handler

  .weak      LPTIM3_IRQHandler
  .thumb_set LPTIM3_IRQHandler,Default_Handler

  .weak      LPTIM4_IRQHandler
  .thumb_set LPTIM4_IRQHandler,Default_Handler

  .weak      LPTIM5_IRQHandler
  .thumb_set LPTIM5_IRQHandler,Default_Handler

  .weak      LPUART1_IRQHandler
  .thumb_set LPUART1_IRQHandler,Default_Handler

  .weak      CRS_IRQHandler
  .thumb_set CRS_IRQHandler,Default_Handler

  .weak      ECC_IRQHandler
  .thumb_set ECC_IRQHandler,Default_Handler

  .weak      SAI4_IRQHandler
  .thumb_set SAI4_IRQHandler,Default_Handler

  .weak      WAKEUP_PIN_IRQHandler
  .thumb_set WAKEUP_PIN_IRQHandler,Default_Handler
