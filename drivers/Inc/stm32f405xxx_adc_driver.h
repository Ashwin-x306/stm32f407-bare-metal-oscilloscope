#ifndef INC_STM32F405XXX_ADC_DRIVER_H_
#define INC_STM32F405XXX_ADC_DRIVER_H_
#include"stm32f405xxx.h"  



#define ADC_RES_12  0
#define ADC_RES_10  1
#define ADC_RES_8   2
#define ADC_RES_6   3


#define ADC_SR_AWD        (1U << 0)   // Analog Watchdog Flag
#define ADC_SR_EOC        (1U << 1)   // End Of Conversion
#define ADC_SR_JEOC       (1U << 2)   // Injected Channel End Of Conversion
#define ADC_SR_JSTRT      (1U << 3)   // Injected Channel Start Flag
#define ADC_SR_STRT       (1U << 4)   // Regular Channel Start Flag
#define ADC_SR_OVR        (1U << 5)   // Overrun Flag

#define ADC_SAMPLE_TIME_3CYCLES        0x00U   // 000
#define ADC_SAMPLE_TIME_15CYCLES       0x01U   // 001
#define ADC_SAMPLE_TIME_28CYCLES       0x02U   // 010
#define ADC_SAMPLE_TIME_56CYCLES       0x03U   // 011
#define ADC_SAMPLE_TIME_84CYCLES       0x04U   // 100
#define ADC_SAMPLE_TIME_112CYCLES      0x05U   // 101
#define ADC_SAMPLE_TIME_144CYCLES      0x06U   // 110
#define ADC_SAMPLE_TIME_480CYCLES      0x07U   // 111

typedef struct{
 uint8_t ADC_Res ; 
 uint32_t ADC_NO_Channel  ; //  init set to one 
 uint8_t  ADC_Channel[16] ; // set to one if  using that channel else stays zero
 uint8_t ADC_Sample_Time[16] ;  
 


}ADC_PinConfig_t; 

typedef struct{

 ADC_RegDef_t *pADCx ; 
ADC_PinConfig_t ADC_PinConfig ;


}ADC_Handle_t;   
 
void ADC_Peripheral_control(ADC_Handle_t  * ADC_Handle , uint8_t EnorDi) ; 
void ADC_Init(ADC_Handle_t * ADC_Handle)  ; 

void ADC_StartConversion(ADC_Handle_t *ADC_Handle ) ; 
uint16_t  ADC_ReadValue(ADC_Handle_t  * ADC_Handle )  ; 
float ADC_ReadVoltage(ADC_Handle_t *ADC_Handle) ;

#endif
