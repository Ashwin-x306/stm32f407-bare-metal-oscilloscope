/*
 * stm32f405xxx_gpio_driver.h
 *
 *  Created on: Jun 21, 2026
 *      Author: Ashwin
 */

#ifndef STM32F405XXX_GPIO_DRIVER_H_
#define STM32F405XXX_GPIO_DRIVER_H_


#include"stm32f405xxx.h"






/*
 * Peripheral Clock setup
 */
#define GPIO_PIN_NO_0        0
#define GPIO_PIN_NO_1        1
#define GPIO_PIN_NO_2        2
#define GPIO_PIN_NO_3        3
#define GPIO_PIN_NO_4        4
#define GPIO_PIN_NO_5        5
#define GPIO_PIN_NO_6        6
#define GPIO_PIN_NO_7        7
#define GPIO_PIN_NO_8        8
#define GPIO_PIN_NO_9        9
#define GPIO_PIN_NO_10       10
#define GPIO_PIN_NO_11       11
#define GPIO_PIN_NO_12       12
#define GPIO_PIN_NO_13       13
#define GPIO_PIN_NO_14       14
#define GPIO_PIN_NO_15       15





#define GPIO_MODE_IN         0
#define GPIO_MODE_OUT        1
#define GPIO_MODE_ALTFN      2
#define GPIO_MODE_ANALOG     3


#define GPIO_MODE_IT_FT      4
#define GPIO_MODE_IT_RT      5
#define GPIO_MODE_IT_RFT     6

#define GPIO_OP_TYPE_PP      0
#define GPIO_OP_TYPE_OD      1


#define GPIO_SPEED_LOW       0
#define GPIO_SPEED_MEDIUM    1
#define GPIO_SPEED_FAST      2
#define GPIO_SPEED_HIGH      3

#define GPIO_NO_PUPD         0
#define GPIO_PIN_PU          1
#define GPIO_PIN_PD          2

#define SET                  ENABLE
#define RESET                DISABLE

#define GPIO_AF0             0
#define GPIO_AF1             1
#define GPIO_AF2             2
#define GPIO_AF3             3
#define GPIO_AF4             4
#define GPIO_AF5             5
#define GPIO_AF6             6
#define GPIO_AF7             7
#define GPIO_AF8             8
#define GPIO_AF9             9
#define GPIO_AF10            10
#define GPIO_AF11            11
#define GPIO_AF12            12
#define GPIO_AF13            13
#define GPIO_AF14            14
#define GPIO_AF15            15

typedef struct
{
    uint8_t GPIO_PinNumber;      // Possible values: GPIO_PIN_NO_0 to GPIO_PIN_NO_15

    uint8_t GPIO_PinMode;        // Input, Output, Alternate Function, Analog, Interrupt

    uint8_t GPIO_PinSpeed;       // Low, Medium, Fast, High

    uint8_t GPIO_PinPuPdControl; // No Pull-up/Pull-down, Pull-up, Pull-down

    uint8_t GPIO_PinOPType;      // Push-Pull or Open-Drain

    uint8_t GPIO_PinAltFunMode;  // Alternate Function Number (AF0-AF15)

} GPIO_PinConfig_t;
typedef struct
{
    GPIO_RegDef_t *pGPIOx;            // GPIO peripheral base address

    GPIO_PinConfig_t GPIO_PinConfig;  // GPIO pin configuration

} GPIO_Handle_t;
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx  , uint8_t EnORDi);

/*
 * Init and De-init
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);


uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx , uint8_t pinNumber );
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx , uint8_t pinNumber , uint8_t  value );
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx ,  uint16_t value );

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx ,  uint8_t pinNumber );

void GPIO_IRQConfig(uint8_t IRQ_Number ,   uint8_t EnOrDi);
void GPIO_IRQHandler(uint8_t PinNumber );
void GPIO_IRQPriorityConfig(uint8_t  IRQNumber , uint8_t IRQPriority) ;
#endif /* STM32F405XXX_GPIO_DRIVER_H_ */
