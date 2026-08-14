#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>
#include<stm32f405xxx.h>

/* ============================================================
 * TIM3 BASE ADDRESS
 * ============================================================ */

#define TIM3_BASE_ADDR       0x40000400UL
// Timer 3 registers  

#define TIM3_IRQn  29 

#define TIM3_CR1             (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x00UL))
#define TIM3_CR2             (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x04UL))
#define TIM3_SMCR            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x08UL))
#define TIM3_DIER            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x0CUL))
#define TIM3_SR              (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x10UL))
#define TIM3_EGR             (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x14UL))
#define TIM3_CCMR1           (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x18UL))
#define TIM3_CCMR2           (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x1CUL))
#define TIM3_CCER            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x20UL))
#define TIM3_CNT             (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x24UL))
#define TIM3_PSC             (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x28UL))
#define TIM3_ARR             (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x2CUL))
#define TIM3_CCR1            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x34UL))
#define TIM3_CCR2            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x38UL))
#define TIM3_CCR3            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x3CUL))
#define TIM3_CCR4            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x40UL))
#define TIM3_DCR             (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x48UL))
#define TIM3_DMAR            (*(volatile uint32_t *)(TIM3_BASE_ADDR + 0x4CUL))

#define UPCOUNTING 1 
#define DOWNCOUNTING 0 



typedef struct {
 uint16_t TIM3_PRES_Value ; 
 uint16_t TIM3_COUNTING_MODE ; 
 uint16_t  TIM3_ARR_VALUE ; 
//  uint16_t  TIM3_ALIGN_MODE ;  EDGE ALIGNEMENT MODE ;


}Timer_Handle_t ;

void TIM3_PeriClockEnable(Timer_Handle_t  *Timer_Handle ,uint8_t EnorDi ) ; 
void TIM3_Init(Timer_Handle_t *Timer_Handle ) ;
void  TIM3_StartCount(Timer_Handle_t *Timer_Handle ) ;
void TIM3_StopCount(Timer_Handle_t *Timer_Handle) ; 
uint8_t Get_Flag_Status(Timer_Handle_t *Timer_Handle) ; 
void  TIM3_ClearUpdateFlag(void ) ; 



#endif /* TIMER_DRIVER_H */