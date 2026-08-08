/*
 * stm32f405xxx.h
 *
 *  Created on: Jun 20, 2026
 *  Author: Ashwin S
 *
 */

#ifndef STM32F405XXX_H_
#define STM32F405XXX_H_

#include<stdint.h>

#include<stddef.h>

#define __vo volatile

/* NVIC ISERx Registers (Interrupt Set Enable Registers) */

#define NVIC_ISER0      ((__vo uint32_t*)0xE000E100)
#define NVIC_ISER1      ((__vo uint32_t*)0xE000E104)
#define NVIC_ISER2      ((__vo uint32_t*)0xE000E108)
#define NVIC_ISER3      ((__vo uint32_t*)0xE000E10C)
#define ENABLE           1

/* NVIC ICERx Registers (Interrupt Clear Enable Registers) */

#define NVIC_ICER0      ((__vo uint32_t*)0xE000E180)
#define NVIC_ICER1      ((__vo uint32_t*)0xE000E184)
#define NVIC_ICER2      ((__vo uint32_t*)0xE000E188)
#define NVIC_ICER3      ((__vo uint32_t*)0xE000E18C)



#define FLASH_ACR_ADDR 0x40023C00
#define FLASH_ACR (*(volatile uint32_t *)FLASH_ACR_ADDR)

/* NVIC Priority Registers Base Address */

#define NVIC_PR_BASEADDR ((__vo uint32_t*)0xE000E400)


#define FLASH_BASEADDR                 (uint32_t)0x08000000
#define SRAM1_BASEADDR                 (uint32_t)0x20000000
#define SRAM                           SRAM1_BASEADDR
#define SRAM2_BASEADDR                 (uint32_t)0x2001C000
#define SRAM3_BASEADDR                 (uint32_t)0x20020000
#define SYSMEM_BASEADDR                (uint32_t)0x1FFF0000   //  ROM BASE ADDRESS


#define APB1_BASEADDR                  (uint32_t)0x40000000
#define PERI_BASEADDR                  APB1_BASEADDR

#define APB2_BASEADDR                  (uint32_t)0x40010000
#define AHB1_BASEADDR                  (uint32_t)0x40020000
#define AHB2_BASEADDR                  (uint32_t)0x50000000
/*BASE ADDRESSES OF THE PERI ON THE AHB1 BUS */
#define GPIOA_BASEADDR                 (AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR                 (AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR                 (AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR                 (AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR                 (AHB1_BASEADDR + 0x1000)
#define GPIOF_BASEADDR                 (AHB1_BASEADDR + 0x1400)
#define GPIOG_BASEADDR                 (AHB1_BASEADDR + 0x1800)
#define GPIOH_BASEADDR                 (AHB1_BASEADDR + 0x1C00)
#define GPIOI_BASEADDR                 (AHB1_BASEADDR + 0x2000)


#define RCC_BASEADDR                   (AHB1_BASEADDR + 0x3800)

/*BASE ADDRESSES OF THE PERI ON THE AHB2 BUS */
/* USB OTG FS */
#define OTG_FS_BASEADDR  (AHB2_BASEADDR + 0x0000)
/* DCMI */
#define DCMI_BASEADDR    (AHB2_BASEADDR + 0x50000)
/* CRYP */
#define CRYP_BASEADDR    (AHB2_BASEADDR + 0x60000)
/* HASH */
#define HASH_BASEADDR    (AHB2_BASEADDR + 0x60400)
/* RNG */
#define RNG_BASEADDR     (AHB2_BASEADDR + 0x60800)



//**************************  base address of the APB1 PERIPHERALS********************************

#define TIM2_BASEADDR         (APB1_BASEADDR + 0x0000)
#define TIM3_BASEADDR         (APB1_BASEADDR + 0x0400)
#define TIM4_BASEADDR         (APB1_BASEADDR + 0x0800)
#define TIM5_BASEADDR         (APB1_BASEADDR + 0x0C00)
#define TIM6_BASEADDR         (APB1_BASEADDR + 0x1000)
#define TIM7_BASEADDR         (APB1_BASEADDR + 0x1400)

#define TIM12_BASEADDR      (APB1_BASEADDR + 0x1800)
#define TIM13_BASEADDR      (APB1_BASEADDR + 0x1C00)
#define TIM14_BASEADDR      (APB1_BASEADDR + 0x2000)

#define RTC_BKP_BASEADDR    (APB1_BASEADDR + 0x2800)
#define WWDG_BASEADDR       (APB1_BASEADDR + 0x2C00)
#define IWDG_BASEADDR       (APB1_BASEADDR + 0x3000)

#define SPI2_BASEADDR    (APB1_BASEADDR + 0x3800)
#define SPI3_BASEADDR    (APB1_BASEADDR + 0x3C00)

#define USART2_BASEADDR  (APB1_BASEADDR + 0x4400)
#define USART3_BASEADDR  (APB1_BASEADDR + 0x4800)
#define UART4_BASEADDR   (APB1_BASEADDR + 0x4C00)
#define UART5_BASEADDR   (APB1_BASEADDR + 0x5000)

#define I2C1_BASEADDR    (APB1_BASEADDR + 0x5400)
#define I2C2_BASEADDR    (APB1_BASEADDR + 0x5800)
#define I2C3_BASEADDR    (APB1_BASEADDR + 0x5C00)

#define CAN1_BASEADDR    (APB1_BASEADDR + 0x6400)
#define CAN2_BASEADDR    (APB1_BASEADDR + 0x6800)

#define PWR_BASEADDR     (APB1_BASEADDR + 0x7000)
#define DAC_BASEADDR     (APB1_BASEADDR + 0x7400)




// **************************** BASE ADDRESSES OF APBE PERIPHERALS***********************************
#define TIM1_BASEADDR    (APB2_BASEADDR + 0x0000)
#define TIM8_BASEADDR    (APB2_BASEADDR + 0x0400)

#define USART1_BASEADDR  (APB2_BASEADDR + 0x1000)
#define USART6_BASEADDR  (APB2_BASEADDR + 0x1400)

#define ADC1_BASEADDR    (APB2_BASEADDR + 0x2000)
#define ADC2_BASEADDR    (APB2_BASEADDR + 0x2100)
#define ADC3_BASEADDR    (APB2_BASEADDR + 0x2200)

#define SDIO_BASEADDR    (APB2_BASEADDR + 0x2C00)

#define SPI1_BASEADDR    (APB2_BASEADDR + 0x3000)

#define SYSCFG_BASEADDR  (APB2_BASEADDR + 0x3800)
#define EXTI_BASEADDR    (APB2_BASEADDR + 0x3C00)

#define TIM9_BASEADDR    (APB2_BASEADDR + 0x4000)
#define TIM10_BASEADDR   (APB2_BASEADDR + 0x4400)
#define TIM11_BASEADDR   (APB2_BASEADDR + 0x4800)

/* EXTI IRQ Numbers */

#define IRQ_NO_EXTI0          6 
#define IRQ_NO_EXTI1          7
#define IRQ_NO_EXTI2          8
#define IRQ_NO_EXTI3          9
#define IRQ_NO_EXTI4          10

#define IRQ_NO_EXTI9_5        23

#define IRQ_NO_EXTI15_10      40
#define ENABLE                1
#define DISABLE               0

#define SPI1_BASEADDR    (APB2_BASEADDR + 0x3000)
#define SPI2_BASEADDR    (APB1_BASEADDR + 0x3800)
#define SPI3_BASEADDR    (APB1_BASEADDR + 0x3C00)
#define SPI4_BASEADDR    (APB2_BASEADDR + 0x3400)

#define IRQ_NO_I2C1_EV          31
#define IRQ_NO_I2C1_ER          32

#define IRQ_NO_I2C2_EV          33
#define IRQ_NO_I2C2_ER          34
#define IRQ_NO_I2C3_EV          72
#define IRQ_NO_I2C3_ER          73

#define NVIC_PR_BASE_ADDR   ((volatile uint32_t*)0xE000E400)

typedef struct
{
    volatile uint32_t MODER;      // GPIO port mode register
                                   // 00: Input
                                   // 01: Output
                                   // 10: Alternate Function
                                   // 11: Analog
   volatile uint32_t OTYPER;     // GPIO output type register
                                   // 0: Push-Pull
                                   // 1: Open-Drain

    volatile uint32_t OSPEEDR;    // GPIO output speed register
                                   // Low, Medium, Fast, High speed

    volatile uint32_t PUPDR;      // GPIO pull-up/pull-down register
                                   // 00: No pull
                                   // 01: Pull-up
                                   // 10: Pull-down

    volatile uint32_t IDR;        // GPIO input data register
                                   // Read input pin states

    volatile uint32_t ODR;        // GPIO output data register
                                   // Write output pin states

    volatile uint32_t BSRR;       // GPIO bit set/reset register
                                   // Atomic pin set/reset

    volatile uint32_t LCKR;       // GPIO configuration lock register
                                   // Lock GPIO configuration

    volatile uint32_t AFR[2];     // Alternate Function Registers
                                   // AFR[0] -> AFRL (Pins 0-7)
                                   // AFR[1] -> AFRH (Pins 8-15)
                                   // AF0-AF15 selection

} GPIO_RegDef_t;

typedef struct
{
    volatile uint32_t CR;          // Clock Control Register
                                   // HSI, HSE, PLL enable/status

    volatile uint32_t PLLCFGR;     // PLL Configuration Register
                                   // PLL source and multiplication factors

    volatile uint32_t CFGR;        // Clock Configuration Register
                                   // System clock selection
                                   // AHB/APB

    volatile uint32_t CIR;         // Clock Interrupt Register

    volatile uint32_t AHB1RSTR;    // AHB1 Peripheral Reset Register
                                   // GPIOA-I, DMA1, DMA2 reset

    volatile uint32_t AHB2RSTR;    // AHB2 Peripheral Reset Register

    volatile uint32_t AHB3RSTR;    // AHB3 Peripheral Reset Register

    uint32_t RESERVED0;

    volatile uint32_t APB1RSTR;    // APB1 Peripheral Reset Register
                                   // TIM2-7, USART2-5, I2C, SPI2, etc.

    volatile uint32_t APB2RSTR;    // APB2 Peripheral Reset Register
                                   // TIM1, USART1, SPI1, ADCs, etc.
    uint32_t RESERVED1[2];

    volatile uint32_t AHB1ENR;     // AHB1 Peripheral Clock Enable Register
                                   // GPIOAEN = bit0
                                   // GPIOBEN = bit1
                                   // GPIOCEN = bit2
                                   // ...
                                   // DMA1EN = bit21
                                   // DMA2EN = bit22

    volatile uint32_t AHB2ENR;     // AHB2 Peripheral Clock Enable Register

    volatile uint32_t AHB3ENR;     // AHB3 Peripheral Clock Enable Register

    uint32_t RESERVED2;

    volatile uint32_t APB1ENR;     // APB1 Peripheral Clock Enable Register

    volatile uint32_t APB2ENR;     // APB2 Peripheral Clock Enable Register

    uint32_t RESERVED3[2];

    volatile uint32_t AHB1LPENR;   // AHB1 Low Power Clock Enable Register

    volatile uint32_t AHB2LPENR;   // AHB2 Low Power Clock Enable Register

    volatile uint32_t AHB3LPENR;   // AHB3 Low Power Clock Enable Register

    uint32_t RESERVED4;

    volatile uint32_t APB1LPENR;   // APB1 Low Power Clock Enable Register

    volatile uint32_t APB2LPENR;   // APB2 Low Power Clock Enable Register

    uint32_t RESERVED5[2];

    volatile uint32_t BDCR;        // Backup Domain Control Register
                                   // RTC, LSE control

    volatile uint32_t CSR;         // Clock Control & Status Register
                                   // LSI control, reset flags

    uint32_t RESERVED6[2];

    volatile uint32_t SSCGR;       // Spread Spectrum Clock Generation Register

    volatile uint32_t PLLI2SCFGR;  // PLLI2S Configuration Register

    volatile uint32_t PLLSAICFGR;  // PLLSAI Configuration Register

    volatile uint32_t DCKCFGR;     // Dedicated Clock Configuration Register

} RCC_RegDef_t;

typedef struct
{
    volatile uint32_t IMR;      /* Interrupt Mask Register           Offset: 0x00 */
    volatile uint32_t EMR;      /* Event Mask Register               Offset: 0x04 */
    volatile uint32_t RTSR;     /* Rising Trigger Selection Register Offset: 0x08 */
    volatile uint32_t FTSR;     /* Falling Trigger Selection Register Offset: 0x0C */
    volatile uint32_t SWIER;    /* Software Interrupt Event Register Offset: 0x10 */
    volatile uint32_t PR;       /* Pending Register                  Offset: 0x14 */

} EXTI_RegDef_t;

typedef struct
{
    volatile uint32_t CR1;      // Control Register 1
    volatile uint32_t CR2;      // Control Register 2
    volatile uint32_t SR;       // Status Register
    volatile uint32_t DR;       // Data Register
    volatile uint32_t CRCPR;    // CRC Polynomial Register
    volatile uint32_t RXCRCR;   // RX CRC Register
    volatile uint32_t TXCRCR;   // TX CRC Register
    volatile uint32_t I2SCFGR;  // I2S Configuration Register
    volatile uint32_t I2SPR;    // I2S Prescaler Register

} SPI_RegDef_t;


typedef struct
{
    volatile uint32_t CR1;      // 0x00 : Control Register 1
    volatile uint32_t CR2;      // 0x04 : Control Register 2
    volatile uint32_t OAR1;     // 0x08 : Own Address Register 1
    volatile uint32_t OAR2;     // 0x0C : Own Address Register 2
    volatile uint32_t DR;       // 0x10 : Data Register
    volatile uint32_t SR1;      // 0x14 : Status Register 1
    volatile uint32_t SR2;      // 0x18 : Status Register 2
    volatile uint32_t CCR;      // 0x1C : Clock Control Register
    volatile uint32_t TRISE;    // 0x20 : TRISE Register
    volatile uint32_t FLTR;     // 0x24 : Filter Register

} I2C_RegDef_t;


typedef struct
{
    volatile uint32_t MEMRMP;       /* Offset : 0x00 */
    volatile uint32_t PMC;          /* Offset : 0x04 */

    volatile uint32_t EXTICR[4];    /* Offset : 0x08 - 0x14
                                       EXTICR1
                                       EXTICR2
                                       EXTICR3
                                       EXTICR4 */

    uint32_t RESERVED[2];           /* Offset : 0x18 - 0x1C */



    volatile uint32_t CMPCR;        /* Offset : 0x20 */
    uint32_t RESERVED2[2] ;

    volatile  uint32_t CFGR ;

} SYSCFG_RegDef_t; 

typedef struct
{
    volatile uint32_t SR;        // 0x00 : Status Register
    volatile uint32_t CR1;       // 0x04 : Control Register 1
    volatile uint32_t CR2;       // 0x08 : Control Register 2
    volatile uint32_t SMPR1;     // 0x0C : Sample Time Register 1
    volatile uint32_t SMPR2;     // 0x10 : Sample Time Register 2
    volatile uint32_t JOFR1;     // 0x14 : Injected Channel Data Offset Register 1
    volatile uint32_t JOFR2;     // 0x18 : Injected Channel Data Offset Register 2
    volatile uint32_t JOFR3;     // 0x1C : Injected Channel Data Offset Register 3
    volatile uint32_t JOFR4;     // 0x20 : Injected Channel Data Offset Register 4
    volatile uint32_t HTR;       // 0x24 : Watchdog Higher Threshold Register
    volatile uint32_t LTR;       // 0x28 : Watchdog Lower Threshold Register
    volatile uint32_t SQR1;      // 0x2C : Regular Sequence Register 1
    volatile uint32_t SQR2;      // 0x30 : Regular Sequence Register 2
    volatile uint32_t SQR3;      // 0x34 : Regular Sequence Register 3
    volatile uint32_t JSQR;      // 0x38 : Injected Sequence Register
    volatile uint32_t JDR1;      // 0x3C : Injected Data Register 1
    volatile uint32_t JDR2;      // 0x40 : Injected Data Register 2
    volatile uint32_t JDR3;      // 0x44 : Injected Data Register 3
    volatile uint32_t JDR4;      // 0x48 : Injected Data Register 4
    volatile uint32_t DR;        // 0x4C : Regular Data Register

} ADC_RegDef_t;
#define GPIOA   ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB   ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC   ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD   ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE   ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF   ((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG   ((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH   ((GPIO_RegDef_t*)GPIOH_BASEADDR)
#define GPIOI   ((GPIO_RegDef_t*)GPIOI_BASEADDR)

#define RCC     ((RCC_RegDef_t*)RCC_BASEADDR)

#define EXTI    ((EXTI_RegDef_t*)EXTI_BASEADDR )

#define SPI1    ((SPI_RegDef_t *)SPI1_BASEADDR)
#define SPI2    ((SPI_RegDef_t *)SPI2_BASEADDR)
#define SPI3    ((SPI_RegDef_t *)SPI3_BASEADDR)
#define SPI4    ((SPI_RegDef_t *)SPI4_BASEADDR)


#define I2C1   ((I2C_RegDef_t *)I2C1_BASEADDR)
#define I2C2   ((I2C_RegDef_t *)I2C2_BASEADDR)
#define I2C3   ((I2C_RegDef_t *)I2C3_BASEADDR)

/* ADC Peripheral Base Addresses */

#define ADC1    ((ADC_RegDef_t *)ADC1_BASEADDR)
#define ADC2    ((ADC_RegDef_t *)ADC2_BASEADDR)
#define ADC3    ((ADC_RegDef_t *)ADC3_BASEADDR)

#define SYSCFG  ((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)



uint8_t GPIO_BASEADDR_TO_CODE(GPIO_RegDef_t *gpio_base_addr) ;




#endif /* STM32F405XXX_H_ */



