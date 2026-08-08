/*
 * system_clock.h
 *
 * Bare-metal system clock setup for STM32F407 Discovery boards
 * (8 MHz HSE crystal -> 168 MHz SYSCLK via the main PLL).
 */

#ifndef SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

#include <stdint.h>

/* Holds the resulting core clock in Hz after SystemClock_Config_168MHz()
 * runs. Useful anywhere you need the real clock speed (SysTick reloads,
 * UART baud rate dividers, etc.) instead of hardcoding 168000000. */
extern uint32_t SystemCoreClock;

/* Configures HSE -> PLL -> SYSCLK = 168 MHz, HCLK = 168 MHz,
 * APB1 = 42 MHz, APB2 = 84 MHz. Call this first thing in main(),
 * before touching any other peripheral. Falls back silently to HSI
 * (16 MHz, SystemCoreClock left at 16000000) if the external 8 MHz
 * crystal doesn't start - see the HSE timeout note inside. */
void SystemClock_Config_168MHz(void);

#endif /* SYSTEM_CLOCK_H */
