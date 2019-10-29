/*
 * board.h
 *
 *  Created on: 12 сент. 2015 г.
 *      Author: Kreyl
 */

#pragma once

// ==== General ====
#define BOARD_NAME          "BigSphere01"
#define APP_NAME            "BigSphere"

// MCU type as defined in the ST header.
#define STM32F205xx

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ 12000000

// OS timer settings
#define STM32_ST_IRQ_PRIORITY   2
#define STM32_ST_USE_TIMER      5
#define SYS_TIM_CLK             (Clk.GetTimInputFreq(TIM5))

//  Periphery
#define I2C1_ENABLED            FALSE
#define I2C2_ENABLED            FALSE
#define I2C3_ENABLED            FALSE
#define SIMPLESENSORS_ENABLED   FALSE
#define BUTTONS_ENABLED         FALSE

#define ADC_REQUIRED            FALSE

#if 1 // ========================== GPIO =======================================
// EXTI
#define INDIVIDUAL_EXTI_IRQ_REQUIRED    FALSE

// Debug pin
#define DBG_PIN         GPIOB, 5
#define DBG_HI()        PinSetHi(DBG_PIN)
#define DBG_LO()        PinSetLo(DBG_PIN)
#define DBG_TOGGLE()    PinToggle(DBG_PIN)

// UART
#define UART_GPIO       GPIOB
#define UART_TX_PIN     6
#define UART_RX_PIN     7

// Indication
#define BLINK_LED       GPIOB, 8, omPushPull
#define LED_PWR1        GPIOA, 8, omPushPull
#define LED_PWR2        GPIOA, 10, omPushPull
#define LED_PWR3        GPIOA, 15, omPushPull
#define LED_PWR4        GPIOB, 9, omPushPull
#define LED_PWR5        GPIOB, 10, omPushPull
#define LED_PWR6        GPIOB, 11, omPushPull
#define LED_PWR7        GPIOD, 2, omPushPull
#define LED_PWR8        GPIOC, 14, omPushPull
#define LED_PWR9        GPIOC, 15, omPushPull

// LED registers
#define LED_MOSI        GPIOB, 15, omPushPull, pudNone, AF5
#define LED_CLK         GPIOB, 13, omPushPull, pudNone, AF5
#define LED_LATCH       GPIOB, 12
#define LED_OE          GPIOB, 14

// LEDs
#define LEDS_CNT        63
#define LEDS_BYTE_CNT   8 // 63 bits => 8 bytes

// Power control
#define PWR_CNT         9
#define PWR_CTRL1       GPIOC, 4, omPushPull
#define PWR_CTRL2       GPIOC, 5, omPushPull
#define PWR_CTRL3       GPIOC, 6, omPushPull
#define PWR_CTRL4       GPIOC, 7, omPushPull
#define PWR_CTRL5       GPIOC, 9, omPushPull
#define PWR_CTRL6       GPIOC, 10, omPushPull
#define PWR_CTRL7       GPIOC, 11, omPushPull
#define PWR_CTRL8       GPIOC, 12, omPushPull
#define PWR_CTRL9       GPIOC, 13, omPushPull

#define VENT_CTRL       GPIOC, 8

// Buttons
#define BTN_PIN         GPIOB, 2, pudPullUp

// USB
#define USB_DETECT_PIN  GPIOA, 9
#define USB_DM          GPIOA, 11
#define USB_DP          GPIOA, 12
#define USB_AF          AF10

// Measurement
#define VOLT1           GPIOA, 1
#define VOLT2           GPIOA, 2
#define VOLT3           GPIOA, 3
#define VOLT4           GPIOC, 0
#define VOLT5           GPIOC, 1
#define VOLT6           GPIOC, 2
#define VOLT7           GPIOC, 3
#define VOLT8           GPIOB, 0
#define VOLT9           GPIOB, 1

// Radio: SPI, Sck,Miso,Mosi, Cs, GDO0
#define CC_Setup        SPI1, GPIOA, 5,6,7, GPIOA,4, GPIOA,0

#endif // GPIO

#if 1 // =========================== SPI =======================================
#define LEDS_SPI        SPI2
#endif

#if 1 // =========================== Timers ====================================
#define LEDS_TIM        TIM3
#define LEDS_TIM_IRQ_VECTOR VectorB4

#endif

#if I2C2_ENABLED // ====================== I2C ================================
#define I2C2_BAUDRATE   400000
#define I2C_PILL        i2c2
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Measuremen timing src
#define ADC_TIM             TIM2

// ADC channels
#define ADC_VOLTAGE_CH      4
#define ADC_CURRENT_CH      6
#define ADC_VREFINT_CH      17 // Constant

#define ADC_CHANNELS        { ADC_VOLTAGE_CH, ADC_CURRENT_CH, ADC_VREFINT_CH }
// Index in array
#define ADC_VOLTAGE_INDX    0
#define ADC_CURRENT_INDX    1
#define ADC_VREFINT_INDX    2

#define ADC_CHANNEL_CNT     3   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#endif

#if 1 // =========================== DMA =======================================
#define STM32_DMA_REQUIRED  TRUE
// ==== Uart ====
// Remap is made automatically if required
#define UART_DMA_TX     STM32_DMA_STREAM_ID(2, 7)
#define UART_DMA_RX     STM32_DMA_STREAM_ID(2, 5)
#define UART_DMA_CHNL   4
#define UART_DMA_TX_MODE(Chnl) (STM32_DMA_CR_CHSEL(Chnl) | DMA_PRIORITY_LOW | STM32_DMA_CR_MSIZE_BYTE | STM32_DMA_CR_PSIZE_BYTE | STM32_DMA_CR_MINC | STM32_DMA_CR_DIR_M2P | STM32_DMA_CR_TCIE)
#define UART_DMA_RX_MODE(Chnl) (STM32_DMA_CR_CHSEL(Chnl) | DMA_PRIORITY_MEDIUM | STM32_DMA_CR_MSIZE_BYTE | STM32_DMA_CR_PSIZE_BYTE | STM32_DMA_CR_MINC | STM32_DMA_CR_DIR_P2M | STM32_DMA_CR_CIRC)

// ==== SPI ====
#define LEDS_SPI_DMA_TX STM32_DMA_STREAM_ID(1, 4)
#define LEDS_DMA_TX_CHNL   0
#define LEDS_DMA_TX_MODE  \
        (STM32_DMA_CR_CHSEL(LEDS_DMA_TX_CHNL) | \
        DMA_PRIORITY_LOW |     \
        STM32_DMA_CR_MSIZE_BYTE | \
        STM32_DMA_CR_PSIZE_BYTE | \
        /*STM32_DMA_CR_TCIE |*/ \
        STM32_DMA_CR_MINC |    /* Mem pointer increase */  \
        STM32_DMA_CR_DIR_M2P   /* Mem to peripheral */ )

#if I2C1_ENABLED // ==== I2C1 ====
#define I2C1_DMA_TX     STM32_DMA1_STREAM6
#define I2C1_DMA_RX     STM32_DMA1_STREAM5
#endif
#if I2C2_ENABLED // ==== I2C2 ====
#define I2C2_DMA_TX     STM32_DMA1_STREAM7
#define I2C2_DMA_RX     STM32_DMA1_STREAM3
#endif

#if ADC_REQUIRED
#define ADC_DMA         STM32_DMA_STREAM_ID(2, 0)
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* DMA2 Stream0 Channel0 */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_CIRC |       /* Circulate */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA

#if 1 // ========================== USART ======================================
#define PRINTF_FLOAT_EN FALSE
#define UART_TXBUF_SZ   8192
#define UART_RXBUF_SZ   99

#define UARTS_CNT       1

#define CMD_UART_PARAMS \
    USART1, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN, \
    UART_DMA_TX, UART_DMA_RX, UART_DMA_TX_MODE(UART_DMA_CHNL), UART_DMA_RX_MODE(UART_DMA_CHNL)

#endif
