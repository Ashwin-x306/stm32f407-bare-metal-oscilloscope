#include<stm32f405xxx_timer_driver.h>
#define PR_NO 0   //  TOP MIST PRO 

void TIM3_PeriClockEnable(Timer_Handle_t *Timer_Handle, uint8_t EnorDi)
{
    if (EnorDi == ENABLE)RCC->APB1ENR |= (1U << 1);     
    else  RCC->APB1ENR &= ~(1U << 1) ; 
    return;
}


#define NVIC_IPR_BASEADDR        ((volatile uint32_t*)0xE000E400)
#define NO_PR_BITS_IMPLEMENTED   4


static void NVIC_SetPriority(uint8_t IRQ_Number, uint8_t IRQPriority)
{
    uint8_t iprx         = IRQ_Number / 4;
    uint8_t iprx_section = IRQ_Number % 4;
    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

    *(NVIC_IPR_BASEADDR + iprx) &= ~(0xFFU << shift_amount);        
    *(NVIC_IPR_BASEADDR + iprx) |= (IRQPriority << shift_amount);   
}


static void NVIC_EnableIRQ(uint8_t IRQ_Number)
{
    if (IRQ_Number < 32)
        (*NVIC_ISER0) |= (1U << IRQ_Number);
    else if (IRQ_Number < 64)
        (*NVIC_ISER1) |= (1U << (IRQ_Number - 32));
    else if (IRQ_Number < 96)
        (*NVIC_ISER2) |= (1U << (IRQ_Number - 64));
    else if (IRQ_Number < 128)
        (*NVIC_ISER3) |= (1U << (IRQ_Number - 96));
}
void TIM3_Init(Timer_Handle_t *Timer_Handle)
{
    TIM3_PeriClockEnable(Timer_Handle, ENABLE);

    TIM3_CR1 &= ~(1U << 0);   
    TIM3_CR1 &= ~(3U << 5);   

    if (Timer_Handle->TIM3_COUNTING_MODE == UPCOUNTING)
        TIM3_CR1 &= ~(1U << 4);
    else if (Timer_Handle->TIM3_COUNTING_MODE == DOWNCOUNTING)
        TIM3_CR1 |= (1U << 4);

    TIM3_CR1 |= (1U << 7);    

    TIM3_PSC = Timer_Handle->TIM3_PRES_Value;
    TIM3_ARR = Timer_Handle->TIM3_ARR_VALUE;


    TIM3_EGR  |= (1U << 0);   
    TIM3_SR   &= ~(1U << 0);  
    TIM3_DIER |= (1U << 0);   

    /* NVIC configuration */
    NVIC_SetPriority(TIM3_IRQn, PR_NO );   
    NVIC_EnableIRQ(TIM3_IRQn);        

    return;
}

void TIM3_StartCount(Timer_Handle_t *Timer_Handle){
    TIM3_CR1 |= (1U << 0);

    return;
}

void  TIM3_StopCount(Timer_Handle_t * Timer_Handle){
        TIM3_CR1 &= ~(1U << 0) ;  
        return;
}


void TIM3_ClearUpdateFlag(void ){
  //  cleared by the software  
    TIM3_SR &= ~(1U << 0);
    return;
}