/*
 ******************************************************************************
 * @file    stm32f405xxx_gpio_driver.c
 * @brief   GPIO Driver Implementation for STM32F405xxx
 *
 * Created on : Jun 21, 2026
 * Author     : Ashwin
 ******************************************************************************
 */



#include <stm32f405xxx_gpio_driver.h>


/* ============================================================================
 * @brief  Enables the clock of the respective gpio port
 *
 * @param[in]  pGPIOx   Pointer to GPIO port base address
 * @param[in]  EnOrDi   1 = Enable, 0 = Disable
 *
 * @return None
 * ============================================================================ */

uint8_t GPIO_BASEADDR_TO_CODE(GPIO_RegDef_t *gpio_base_addr)
{
	if(gpio_base_addr == GPIOA )return 0  ;
	if(gpio_base_addr == GPIOB )return 1  ;
	if(gpio_base_addr == GPIOC )return 2  ;
	if(gpio_base_addr == GPIOD )return 3  ;
	if(gpio_base_addr == GPIOE )return 4  ;
	if(gpio_base_addr == GPIOF )return 5  ;
	if(gpio_base_addr == GPIOG )return 6  ;
	if(gpio_base_addr == GPIOH )return 7  ;
	if(gpio_base_addr == GPIOI )return 8 ;
    return 9 ;

}

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi)
{
    if (EnOrDi == 1)
    {
        if (pGPIOx == GPIOA)  RCC->AHB1ENR |= (1 << 0);
        if (pGPIOx == GPIOB)  RCC->AHB1ENR |= (1 << 1);
        if (pGPIOx == GPIOC)  RCC->AHB1ENR |= (1 << 2);
        if (pGPIOx == GPIOD)  RCC->AHB1ENR |= (1 << 3);
        if (pGPIOx == GPIOE)  RCC->AHB1ENR |= (1 << 4);
        if (pGPIOx == GPIOF)  RCC->AHB1ENR |= (1 << 5);
        if (pGPIOx == GPIOG)  RCC->AHB1ENR |= (1 << 6);
        if (pGPIOx == GPIOH)  RCC->AHB1ENR |= (1 << 7);
        if (pGPIOx == GPIOI)  RCC->AHB1ENR |= (1 << 8);
    }
    else
    {
        /* Disable case */
        if (pGPIOx == GPIOA)  RCC->AHB1ENR &= ~(1 << 0);
        if (pGPIOx == GPIOB)  RCC->AHB1ENR &= ~(1 << 1);
        if (pGPIOx == GPIOC)  RCC->AHB1ENR &= ~(1 << 2);
        if (pGPIOx == GPIOD)  RCC->AHB1ENR &= ~(1 << 3);
        if (pGPIOx == GPIOE)  RCC->AHB1ENR &= ~(1 << 4);
        if (pGPIOx == GPIOF)  RCC->AHB1ENR &= ~(1 << 5);
        if (pGPIOx == GPIOG)  RCC->AHB1ENR &= ~(1 << 6);
        if (pGPIOx == GPIOH)  RCC->AHB1ENR &= ~(1 << 7);
        if (pGPIOx == GPIOI)  RCC->AHB1ENR &= ~(1 << 8);
    }
}


/* ============================================================================
 * @brief
 *
 * @param[in]  pGPIOHandle   Pointer to GPIO handle structure
 *
 * @return None
 * ============================================================================ */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
    uint32_t temp = 0;

    // ENABLE THE CLOCK  ;

    GPIO_PeriClockControl(pGPIOHandle->pGPIOx , ENABLE ) ;

    /* 1. Configure GPIO mode */
    if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
    {
        temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode)
               << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

        pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
        pGPIOHandle->pGPIOx->MODER |=  temp;
    }
    else
    {
        /* Interrupt mode configuration */
         if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode ==  GPIO_MODE_IT_FT){
        	 // configure for the falling edge
        	 EXTI->FTSR |=(1<<(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)) ;
        	 EXTI->RTSR &=~(1<<(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)) ;
         }
         else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode ==  GPIO_MODE_IT_RT){
                 	 // configure for the RISING EDGE
        	 EXTI->RTSR |=(1<<(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)) ;
        	 EXTI->FTSR &=~(1<<(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)) ;
          }
         else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode ==  GPIO_MODE_IT_RFT){
                 	 // configure for the BOTH FALLING AND THE RISING EDGE
        	 EXTI->FTSR |=(1<<(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)) ;
        	 EXTI->RTSR |=(1<<(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)) ;

         }

         uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber /4 ;
         uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber %4 ;


         uint8_t  portcode =GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);


          //  ENABLE THE SYSCFG CLK  ;
         // 2. configure the port selection in the SYSCFG_REGISTER

         RCC->APB2ENR |= (1 << 14);
         SYSCFG->EXTICR[temp1] &= ~(0xF << (temp2 * 4));
         SYSCFG->EXTICR[temp1] |= (portcode << (temp2 * 4)) ;
         // 3. ENALE THE EXTI INTERUPPT DELIVERY LINE  IN SYSCFG REGISTER ;
         EXTI->IMR |= (1<<pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) ;
 }


    /* 2. Configure speed */
    temp = 0;

    temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed)
           << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

    pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->OSPEEDR |=  temp;

    /* 3. Configure pull-up / pull-down */
    temp = 0;

    temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl)
           << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

    pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->PUPDR |=  temp;

    /* 4. Configure output type */


    if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_OUT
    		|| pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN ){


    temp = 0;

    temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType)
           << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

    pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->OTYPER |=  temp;
    }
    else pGPIOHandle->pGPIOx->OTYPER &= (0xFFFFFFFF) ; //  NO CHANGES  ;


    /* 5. Configure alternate function register (if applicable) */
    if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == 2)
    {
        // configure the alt function register
        uint8_t temp1, temp2;

        temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
        temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

        pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));
        pGPIOHandle->pGPIOx->AFR[temp1] |=  (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
    }
}


/* ============================================================================
 * @brief  Resets the io port
 *
 * @param[in]  pGPIOx   Pointer to GPIO port base address
 *
 * @return None
 * ============================================================================ */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
    if (pGPIOx == GPIOA)  { RCC->AHB1RSTR |=  (1 << 0);  RCC->AHB1RSTR &= ~(1 << 0); }
    if (pGPIOx == GPIOB)  { RCC->AHB1RSTR |=  (1 << 1);  RCC->AHB1RSTR &= ~(1 << 1); }
    if (pGPIOx == GPIOC)  { RCC->AHB1RSTR |=  (1 << 2);  RCC->AHB1RSTR &= ~(1 << 2); }
    if (pGPIOx == GPIOD)  { RCC->AHB1RSTR |=  (1 << 3);  RCC->AHB1RSTR &= ~(1 << 3); }
    if (pGPIOx == GPIOE)  { RCC->AHB1RSTR |=  (1 << 4);  RCC->AHB1RSTR &= ~(1 << 4); }
    if (pGPIOx == GPIOF)  { RCC->AHB1RSTR |=  (1 << 5);  RCC->AHB1RSTR &= ~(1 << 5); }
    if (pGPIOx == GPIOG)  { RCC->AHB1RSTR |=  (1 << 6);  RCC->AHB1RSTR &= ~(1 << 6); }
    if (pGPIOx == GPIOH)  { RCC->AHB1RSTR |=  (1 << 7);  RCC->AHB1RSTR &= ~(1 << 7); }
    if (pGPIOx == GPIOI)  { RCC->AHB1RSTR |=  (1 << 8);  RCC->AHB1RSTR &= ~(1 << 8); }
}


/* ============================================================================
 * @brief
 *
 * @param[in]  pGPIOx     Pointer to GPIO port base address
 * @param[in]  pinNumber  Pin number to read (0..15)
 *
 * @return uint8_t        Logic level: 0 or 1
 * ============================================================================ */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber)
{
    uint8_t value = (uint8_t)(((pGPIOx->IDR) >> pinNumber) & (0x00000001));
    return value;
}


/* ============================================================================
 * @brief
 *
 * @param[in]  pGPIOx   Pointer to GPIO port base address
 *
 * @return uint16_t     Value of the IDR register
 * ============================================================================ */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
    uint16_t value = pGPIOx->IDR;
    return value;
}


/* ============================================================================
 * @brief
 *
 * @param[in]  pGPIOx     Pointer to GPIO port base address
 * @param[in]  pinNumber  Pin number to write (0..15)
 * @param[in]  value      1 = SET, 0 = RESET
 *
 * @return None
 * ============================================================================ */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber, uint8_t value)
{
    if (value == 1)
        pGPIOx->ODR |=  (1 << pinNumber);
    else
        pGPIOx->ODR &= ~(1 << pinNumber);
}


/* ============================================================================
 * @brief
 *
 * @param[in]  pGPIOx   Pointer to GPIO port base address
 * @param[in]  value    16-bit value to write to ODR
 *
 * @return None
 * ============================================================================ */
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value)
{
    pGPIOx->ODR = value;
}


/* ============================================================================
 * @brief
 *
 * @param[in]  pGPIOx     Pointer to GPIO port base address
 * @param[in]  pinNumber  Pin number to toggle (0..15)
 *
 * @return None
 * ============================================================================ */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t pinNumber)
{
    /* BUG FIX #3: Was incorrectly reading IDR (input) — must read ODR (output) to toggle correctly */
    uint8_t value = (uint8_t)((pGPIOx->ODR >> pinNumber) & 0x1);

    if (value == 1)
        pGPIOx->ODR &= ~(1 << pinNumber);
    else
        pGPIOx->ODR |=  (1 << pinNumber);
}


/* ============================================================================
 * @brief
 *
 * @param[in]  IRQ_Number          IRQ number
 * @param[in]  IRQPriorityNumber   Priority level
 * @param[in]  EnOrDi              1 = Enable, 0 = Disable
 *
 * @return None
 * ============================================================================ */
void GPIO_IRQConfig(uint8_t IRQ_Number,uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        if(IRQ_Number <= 31)
        {
            *NVIC_ISER0 |= (1 << IRQ_Number);
        }
        else if(IRQ_Number < 64)
        {
            *NVIC_ISER1 |= (1 << (IRQ_Number % 32));
        }
        else if(IRQ_Number < 96)
        {
            *NVIC_ISER2 |= (1 << (IRQ_Number % 64));
        }
    }
    else
    {
        if(IRQ_Number <= 31)
        {
            *NVIC_ICER0 |= (1 << IRQ_Number);
        }
        else if(IRQ_Number < 64)
        {
            *NVIC_ICER1 |= (1 << (IRQ_Number % 32));
        }
        else if(IRQ_Number < 96)
        {
            *NVIC_ICER2 |= (1 << (IRQ_Number % 64));
        }
    }
}

void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
    uint8_t iprx         = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    uint8_t shift_amount = (8 * iprx_section) + 4;

    /* Clear the implemented priority bits */
    *(NVIC_PR_BASEADDR + iprx) &=
            ~(0xF << shift_amount);

    /* Set the new priority */
    *(NVIC_PR_BASEADDR + iprx) |=
            (IRQPriority << shift_amount);
}

/* ============================================================================
 * @brief
 *
 * @param[in]  PinNumber   GPIO pin number whose interrupt fired
 *
 * @return None
 * ============================================================================ */
void GPIO_IRQHandler(uint8_t PinNumber){
	// clear  the pr register bit by writing '1' to it ;

if(EXTI->PR  & (1<< PinNumber )){
	EXTI->PR|= (1<<PinNumber ) ;
}

}
