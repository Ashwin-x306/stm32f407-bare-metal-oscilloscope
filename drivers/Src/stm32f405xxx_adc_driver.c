/*
 * stm32f405xxx_adc_driver.h
 *
 *  Created on: Jul 30, 2026
 *      Author: Ashwin
 */
#include"stm32f405xxx.h"
#include "stm32f405xxx_adc_driver.h"
#define ADC_CCR   ((volatile uint32_t *)(ADC1_BASEADDR + 0x300 + 0x04))

static uint8_t  GetFlagStatus(ADC_Handle_t *ADC_Handle , uint8_t FlagName ){

 uint8_t status = ADC_Handle->pADCx->SR   &(FlagName) ;
 return status ; 

}
void ADC_Peripheral_control(ADC_Handle_t *ADC_Handle, uint8_t EnorDi){
 //  every peripheral is in APB2 BUS 
 if(EnorDi == ENABLE ){   
 if(ADC_Handle->pADCx == ADC1 ) RCC->APB2ENR|= (1<<8) ; 
 if(ADC_Handle->pADCx == ADC2 ) RCC->APB2ENR|= (1<<9) ; 
 if(ADC_Handle->pADCx == ADC3 ) RCC->APB2ENR|= (1<<10) ;
 }
 else{

    //  disable the clock  ; 
     if(ADC_Handle->pADCx == ADC1 ) RCC->APB2ENR     &= ~(1<<8) ;
     if(ADC_Handle->pADCx == ADC2 ) RCC->APB2ENR     &= ~(1<<9) ; 
     if(ADC_Handle->pADCx == ADC3 ) RCC->APB2ENR     &= ~(1<<10) ;

 }

}

void ADC_Init(ADC_Handle_t * ADC_Handle){ 

    ADC_Peripheral_control(ADC_Handle ,  ENABLE )  ;  
    uint32_t tempreg = 0 ; 
    tempreg|= (ADC_Handle->ADC_PinConfig.ADC_Res<<24) ;  
    ADC_Handle->pADCx->CR1 = tempreg ; 

    tempreg = 0 ; 
    tempreg|= (1<<0)  ;  //adon  
   // tempreg |=(1<<1) ; //  continous  conversion ;
    tempreg |=(1<<10) ; // END OF THE CONVERSION  ; 
    // tempreg|= (1<<30) ;  //  start conversion of the regular channel 
    ADC_Handle->pADCx->CR2 |= tempreg ; 
    for(uint8_t i =0 ;  i<16 ; i++){
      if(ADC_Handle->ADC_PinConfig.ADC_Channel[i] == ENABLE ){
          if(i<=9){
        	  ADC_Handle->pADCx->SMPR2|= (ADC_Handle->ADC_PinConfig.ADC_Sample_Time[i]<<(3*i)) ;
          }
          else{

          ADC_Handle->pADCx->SMPR1 |= (ADC_Handle->ADC_PinConfig.ADC_Sample_Time[i -10]<<(3*(i-10))) ; 
         }
          
        }
    }
uint8_t seq = 0;      
uint32_t sqr1 = 0;
uint32_t sqr2 = 0;
uint32_t sqr3 = 0; 
for(uint8_t ch = 0; ch < 16; ch++){
    if(ADC_Handle->ADC_PinConfig.ADC_Channel[ch] == ENABLE){
        if(seq < 6)sqr3 |= (ch << (5 * seq)); 
        else if(seq < 12) sqr2 |= (ch << (5 * (seq - 6)));
        else sqr1 |= (ch << (5 * (seq - 12)));
        seq++;
    }

}
if(seq > 0){
    sqr1 |= ((seq - 1) << 20);
}
ADC_Handle->pADCx->SQR1 = sqr1 ; 
ADC_Handle->pADCx->SQR2 = sqr2; 
ADC_Handle->pADCx->SQR3 = sqr3 ; 
*ADC_CCR |= (1<<16) ;

}
void ADC_StartConversion(ADC_Handle_t *ADC_Handle ) {
     ADC_Handle->pADCx->CR2|= (1<<30) ; 
}
uint16_t ADC_ReadValue(ADC_Handle_t *ADC_Handle) {
    while(!GetFlagStatus(ADC_Handle, ADC_SR_EOC));
    uint16_t val = ADC_Handle->pADCx->DR;


    if (ADC_Handle->pADCx->SR & ADC_SR_OVR) {
        ADC_Handle->pADCx->SR &= ~ADC_SR_OVR;
    }

    return val;
}
float ADC_ReadVoltage(ADC_Handle_t *ADC_Handle)
{
    uint8_t res = ADC_Handle->ADC_PinConfig.ADC_Res;

    if(res == ADC_RES_12) return ((float)ADC_ReadValue(ADC_Handle) / 4095.0f) * 3.3f;

    if(res == ADC_RES_10) return ((float)ADC_ReadValue(ADC_Handle) / 1023.0f) * 3.3f;

    if(res == ADC_RES_8)  return ((float)ADC_ReadValue(ADC_Handle) / 255.0f) * 3.3f;

    if(res == ADC_RES_6)  return ((float)ADC_ReadValue(ADC_Handle) / 63.0f) * 3.3f;

    return 0.0f ;

}

