/*
 * sys_clock.c
 *
 *  Created on: Jul 29, 2026
 *      Author: Ashwin
 */


/*
 * system_clock.c
 *
 * STM32F407 Discovery: 8 MHz HSE -> 168 MHz SYSCLK.
 *
 *   VCO_in  = HSE / PLLM   = 8 MHz / 8   = 1 MHz     (must be 1-2 MHz)
 *   VCO_out = VCO_in * PLLN = 1 MHz * 336 = 336 MHz    (must be 192-432 MHz)
 *   SYSCLK  = VCO_out / PLLP = 336 MHz / 2 = 168 MHz
 *   USB/SDIO clk = VCO_out / PLLQ = 336 MHz / 7 = 48 MHz  (needs to be 48 MHz)
 *
 *   HCLK  (AHB)  = SYSCLK / 1 = 168 MHz  (max 168 MHz)
 *   PCLK1 (APB1) = HCLK   / 4 =  42 MHz  (max 42 MHz)
 *   PCLK2 (APB2) = HCLK   / 2 =  84 MHz  (max 84 MHz)
 *
 * Register-level only, uses the RCC struct/macro already declared in
 * stm32f405xxx.h (the same header your GPIO/SPI drivers use). PWR and
 * FLASH register access are declared locally below, guarded with
 * #ifndef, so this file compiles whether or not your header already
 * defines them.
 */

#include "stm32f405xxx.h"
#include "sys_clock.h"

#ifndef PWR
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CSR;
} PWR_RegDef_t;
#define PWR ((PWR_RegDef_t *)0x40007000UL)
#endif

#ifndef FLASH
typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t OPTCR;
} FLASH_RegDef_t;
#define FLASH ((FLASH_RegDef_t *)0x40023C00UL)
#endif

/* RCC->APB1ENR bit for the power interface clock */
#define RCC_APB1ENR_PWREN   (1UL << 28)

/* PWR->CR: voltage regulator output scaling. Bit 14 (VOS) = 1 selects
 * Scale 1 mode, required to reach 168 MHz on STM32F405/407. */
#define PWR_CR_VOS           (1UL << 14)

/* RCC->CR */
#define RCC_CR_HSEON         (1UL << 16)
#define RCC_CR_HSERDY        (1UL << 17)
#define RCC_CR_PLLON         (1UL << 24)
#define RCC_CR_PLLRDY        (1UL << 25)

/* RCC->PLLCFGR */
#define RCC_PLLCFGR_PLLSRC_HSE (1UL << 22)

/* RCC->CFGR SW / SWS field values */
#define RCC_CFGR_SW_HSI      0x0UL
#define RCC_CFGR_SW_HSE      0x1UL
#define RCC_CFGR_SW_PLL      0x2UL
#define RCC_CFGR_SWS_PLL     0x2UL

/* FLASH->ACR */
#define FLASH_ACR_LATENCY_5WS (5UL << 0)
#define FLASH_ACR_PRFTEN      (1UL << 8)
#define FLASH_ACR_ICEN        (1UL << 9)
#define FLASH_ACR_DCEN        (1UL << 10)

uint32_t SystemCoreClock = 16000000UL; /* default: HSI, updated below on success */

void SystemClock_Config_168MHz(void)
{
    /* 1. Turn on the external 8 MHz crystal (HSE) and wait for it to
     *    stabilise. On the F407 Discovery this is a real crystal, so it
     *    should always come up - the timeout below just avoids a dead
     *    hang if the board is ever used without one. */
    RCC->CR |= RCC_CR_HSEON;

    uint32_t timeout = 100000UL;
    while (!(RCC->CR & RCC_CR_HSERDY) && timeout--) {
        /* spin */
    }
    if (!(RCC->CR & RCC_CR_HSERDY)) {
        /* HSE never started - stay on the default 16 MHz HSI rather than
         * hang forever. SystemCoreClock is left at its 16 MHz default. */
        return;
    }

    /* 2. Enable the power interface clock, then select voltage Scale 1
     *    (needed for 168 MHz operation). */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* 3. Program the flash wait states / caches BEFORE raising SYSCLK.
     *    5 wait states is the RM0090 requirement for 168 MHz @ 3.3V,
     *    Scale 1. Prefetch + instruction/data cache are optional but
     *    recommended at this clock speed. */
    FLASH->ACR = FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN
               | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

    /* 4. Configure the main PLL: source = HSE, M = 8, N = 336, P = 2, Q = 7 */
    RCC->PLLCFGR = 0;
    RCC->PLLCFGR |= (8UL << 0);                 /* PLLM  = 8            */
    RCC->PLLCFGR |= (336UL << 6);                /* PLLN  = 336          */
    RCC->PLLCFGR |= (((2UL / 2UL) - 1UL) << 16); /* PLLP  = 2 -> field 0 */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;      /* PLL source = HSE     */
    RCC->PLLCFGR |= (7UL << 24);                 /* PLLQ  = 7 (48 MHz)   */

    /* 5. Turn the PLL on and wait for lock. */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
        /* spin */
    }

    /* 6. Set the bus prescalers: AHB /1 (168 MHz), APB1 /4 (42 MHz),
     *    APB2 /2 (84 MHz). Must be done while still running off
     *    HSE/HSI, before switching SYSCLK to the PLL. */
    RCC->CFGR &= ~(0xFUL << 4);      /* HPRE  = /1                 */
    RCC->CFGR &= ~(0x7UL << 10);
    RCC->CFGR |=  (0x5UL << 10);     /* PPRE1 = /4  (APB1, 42 MHz) */
    RCC->CFGR &= ~(0x7UL << 13);
    RCC->CFGR |=  (0x4UL << 13);     /* PPRE2 = /2  (APB2, 84 MHz) */

    /* 7. Switch SYSCLK over to the PLL and wait for confirmation. */
    RCC->CFGR &= ~(0x3UL << 0);
    RCC->CFGR |=  RCC_CFGR_SW_PLL;
    while (((RCC->CFGR >> 2) & 0x3UL) != RCC_CFGR_SWS_PLL) {
        /* spin */
    }

    SystemCoreClock = 168000000UL;
}
