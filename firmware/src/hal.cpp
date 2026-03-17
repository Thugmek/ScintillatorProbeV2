#include "hal.hpp"
#include "capture.hpp"
#include "stm32h7xx_hal_dma.h"

namespace hal {
    TIM_HandleTypeDef htim2;
    ADC_HandleTypeDef hadc1;
    ADC_HandleTypeDef hadc2;
    ADC_HandleTypeDef hadc3;
    DMA_HandleTypeDef hdma_adc1;
    SPI_HandleTypeDef hspi1;
    PCD_HandleTypeDef hpcd_USB_OTG_FS;

    void MPU_Config(void);
    void SystemClock_Config(void);
    void PeriphCommonClock_Config(void);
    void MX_TIM2_Init(void);
    void MX_ADC1_Init(void);
    void MX_ADC2_Init(void);
    void MX_ADC3_Init(void);
    void MX_USB_OTG_FS_PCD_Init(void);

    void panic(void){
        // TODO goto safe state here
        // Halt CPU in infinite loop
        while (true) {
            __asm volatile("nop");
        }
    }

    void init(void) {
        MPU_Config();
        HAL_Init();
        SystemClock_Config();
        PeriphCommonClock_Config();
        MX_TIM2_Init();
        MX_ADC1_Init();
        MX_ADC2_Init();
        MX_ADC3_Init();
        MX_USB_OTG_FS_PCD_Init();
    }

    void MPU_Config(void){
        MPU_Region_InitTypeDef MPU_InitStruct = {0};

        /* Disables the MPU */
        HAL_MPU_Disable();

        /** Initializes and configures the Region and the memory to be protected
        */
        MPU_InitStruct.Enable = MPU_REGION_ENABLE;
        MPU_InitStruct.Number = MPU_REGION_NUMBER0;
        MPU_InitStruct.BaseAddress = 0x0;
        MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
        MPU_InitStruct.SubRegionDisable = 0x87;
        MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
        MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
        MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
        MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
        MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
        MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

        HAL_MPU_ConfigRegion(&MPU_InitStruct);
        /* Enables the MPU */
        HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    }

    void SystemClock_Config(void){
        RCC_OscInitTypeDef RCC_OscInitStruct = {0};
        RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

        /** Supply configuration update enable
        */
        HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

        /** Configure the main internal regulator output voltage
        */
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

        while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

        /** Initializes the RCC Oscillators according to the specified parameters
        * in the RCC_OscInitTypeDef structure.
        */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
        RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
        RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
        RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
        RCC_OscInitStruct.PLL.PLLM = 4;
        RCC_OscInitStruct.PLL.PLLN = 60;
        RCC_OscInitStruct.PLL.PLLP = 2;
        RCC_OscInitStruct.PLL.PLLQ = 2;
        RCC_OscInitStruct.PLL.PLLR = 2;
        RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
        RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
        RCC_OscInitStruct.PLL.PLLFRACN = 0;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            panic();
        }

        /** Initializes the CPU, AHB and APB buses clocks
        */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                    |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
        RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
        {
            panic();
        }
    }

    void PeriphCommonClock_Config(void){
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
        PeriphClkInitStruct.PLL2.PLL2M = 32;
        PeriphClkInitStruct.PLL2.PLL2N = 100;
        PeriphClkInitStruct.PLL2.PLL2P = 4;
        PeriphClkInitStruct.PLL2.PLL2Q = 2;
        PeriphClkInitStruct.PLL2.PLL2R = 2;
        PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_1;
        PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
        PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
        PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL2;
        PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
        PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            panic();
        }
    }

    void MX_TIM2_Init(void){
        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIM_OC_InitTypeDef sConfigOC = {0};

        __HAL_RCC_TIM2_CLK_ENABLE();

        //uint16_t period = 65535;
        uint16_t period = 100;

        htim2.Instance = TIM2;
        htim2.Init.Prescaler = 30;
        htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim2.Init.Period = period;
        htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
        {
            panic();
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
        {
            panic();
        }
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = 0;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
        {
            panic();
        }

        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        /**TIM2 GPIO Configuration
        PB10     ------> TIM2_CH3
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }

    void set_ht_power(uint16_t interval){
        TIM_OC_InitTypeDef sConfigOC = {0};
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = interval;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
        {
            panic();
        }
    }

    void MX_ADC1_Init(void){
        ADC_MultiModeTypeDef multimode = {0};
        ADC_ChannelConfTypeDef sConfig = {0};

        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /** Common config
        */
        hadc1.Instance = ADC1;
        hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
        hadc1.Init.Resolution = ADC_RESOLUTION_10B;
        hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
        hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hadc1.Init.LowPowerAutoWait = DISABLE;
        hadc1.Init.ContinuousConvMode = ENABLE;
        hadc1.Init.NbrOfConversion = 1;
        hadc1.Init.DiscontinuousConvMode = DISABLE;
        hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
        hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
        hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
        hadc1.Init.OversamplingMode = DISABLE;
        hadc1.Init.Oversampling.Ratio = 1;
        if (HAL_ADC_Init(&hadc1) != HAL_OK)
        {
            panic();
        }

        /** Configure the ADC multi-mode
        */
        multimode.Mode = ADC_DUALMODE_INTERL;
        multimode.DualModeData = ADC_DUALMODEDATAFORMAT_32_10_BITS;
        multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_4CYCLES;
        if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
        {
            panic();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_9;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        sConfig.OffsetSignedSaturation = DISABLE;
        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
        {
            panic();
        }

        /** Configure Analog Watchdog 1
         *  Triggers an interrupt when a conversion on ADC_CHANNEL_9 exceeds the threshold.
         *  Note: In interleaved mode, AWD on ADC1 only checks ADC1 conversions (every other
         *  sample). This is sufficient as long as the pulse spans multiple sample periods.
         */
        ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
        AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
        AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
        AnalogWDGConfig.Channel = ADC_CHANNEL_9;
        AnalogWDGConfig.ITMode = ENABLE;
        AnalogWDGConfig.HighThreshold = capture::TRIGGER_THRESHOLD;
        AnalogWDGConfig.LowThreshold = 0;
        if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
        {
            panic();
        }

        // Enable ADC interrupt for analog watchdog
        // Priority must be >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY (5)
        // so the ISR can use FreeRTOS API (xTaskNotifyFromISR).
        HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);

        __HAL_RCC_DMA1_CLK_ENABLE();

        hdma_adc1.Instance = DMA1_Stream0;
        hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_adc1.Init.Mode = DMA_CIRCULAR;
        hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
        {
            panic();
        }
        __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

        // Enable DMA interrupt
        // Priority must be >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY (5)
        // so the ISR can use FreeRTOS API (xTaskNotifyFromISR).
        HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

    }

    void MX_ADC2_Init(void){
        ADC_ChannelConfTypeDef sConfig = {0};

        /** Common config
        */
        hadc2.Instance = ADC2;
        hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
        hadc2.Init.Resolution = ADC_RESOLUTION_10B;
        hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
        hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hadc2.Init.LowPowerAutoWait = DISABLE;
        hadc2.Init.ContinuousConvMode = ENABLE;
        hadc2.Init.NbrOfConversion = 1;
        hadc2.Init.DiscontinuousConvMode = DISABLE;
        hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
        hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
        hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
        hadc2.Init.OversamplingMode = DISABLE;
        hadc2.Init.Oversampling.Ratio = 1;
        if (HAL_ADC_Init(&hadc2) != HAL_OK)
        {
            panic();
        }

        /** Configure Regular Channel
        */
        sConfig.Channel = ADC_CHANNEL_9;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        sConfig.OffsetSignedSaturation = DISABLE;
        if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
        {
            panic();
        }
    }

    void MX_ADC3_Init(void){
        ADC_ChannelConfTypeDef sConfig = {0};

        __HAL_RCC_ADC3_CLK_ENABLE();

        // PF4 -> ADC3_INP9 (analog mode)
        __HAL_RCC_GPIOF_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        hadc3.Instance = ADC3;
        hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
        hadc3.Init.Resolution = ADC_RESOLUTION_16B;
        hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
        hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hadc3.Init.LowPowerAutoWait = DISABLE;
        hadc3.Init.ContinuousConvMode = DISABLE;
        hadc3.Init.NbrOfConversion = 1;
        hadc3.Init.DiscontinuousConvMode = DISABLE;
        hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
        hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
        hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
        hadc3.Init.OversamplingMode = DISABLE;
        hadc3.Init.Oversampling.Ratio = 1;
        if (HAL_ADC_Init(&hadc3) != HAL_OK)
        {
            panic();
        }

        sConfig.Channel = ADC_CHANNEL_9;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        sConfig.OffsetSignedSaturation = DISABLE;
        if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
        {
            panic();
        }

        HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    }

    void MX_SPI1_Init(void){
        /* SPI1 parameter configuration*/
        hspi1.Instance = SPI1;
        hspi1.Init.Mode = SPI_MODE_MASTER;
        hspi1.Init.Direction = SPI_DIRECTION_2LINES;
        hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
        hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
        hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
        hspi1.Init.NSS = SPI_NSS_SOFT;
        hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
        hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
        hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
        hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        hspi1.Init.CRCPolynomial = 0x0;
        hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
        hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
        hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
        hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
        hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
        hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
        hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
        hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
        hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
        hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
        if (HAL_SPI_Init(&hspi1) != HAL_OK)
        {
            panic();
        }

        /**SPI1 GPIO Configuration
        PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
        PB4 (NJTRST)     ------> SPI1_MISO
        PB5     ------> SPI1_MOSI

        PA4     ------> LCD_CS
        */
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
        /*Configure GPIO pin : LCD_CS_Pin */
        GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = LCD_CS_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);
    }

    void MX_USB_OTG_FS_PCD_Init(void){
        hpcd_USB_OTG_FS.Instance = USB2_OTG_FS;
        hpcd_USB_OTG_FS.Init.dev_endpoints = 9;
        hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
        hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
        hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.battery_charging_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;

        HAL_PWREx_EnableUSBVoltageDetector();

        __HAL_RCC_USB2_OTG_FS_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_OTG2_FS;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

        if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
        {
            panic();
        }

        HAL_PCDEx_SetRxFiFo(&hpcd_USB_OTG_FS, 128);
        HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 0, 32);
        HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 1, 128);
    }

}

