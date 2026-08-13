#include "stm32f405xxx.h"
#include "stm32f405xxx_gpio_driver.h"
#include "stm32f405xxx_spi_driver.h"
#include "stm32f405xxx_adc_driver.h"



#include"stm32f405xxx_timer_driver.h"
#include "ST7789.h"
#include "sys_clock.h"
//#include"fft.h"
#include <stdio.h>
#include<stdint.h>
#include<stdbool.h>
//#include<math.h>
//GLOBAL VARIABLE ;
#define SIGNAL_FREQ_HZ   1000     
//#define TEST_SAMPLE_RATE 100000   
#define ADC_MAX_VALUE    4095     
#define ADC_MID_VALUE    2048      
uint8_t found = DISABLE ;
uint8_t slide_trigger= DISABLE ; 
#define SAMPLES 50000
#define SCB_CPACR (*((volatile uint32_t *)0xE000ED88))
#define PIXELS_PER_DIV 40.0f
SPI_Handle_t SPIHandle;
LCD_Handle_t LCD;
#define DWT_CTRL   (*(volatile uint32_t*)0xE0001000)
#define DWT_CYCCNT (*(volatile uint32_t*)0xE0001004)
#define DEMCR      (*(volatile uint32_t*)0xE000EDFC) 
#define CLK_SPEED 168000000 




#define THRESHOLD  1.8f 

#define PI 3.14159265358979323846f
#define TIM3_BASE_ADDR    0x40000400UL

#define TIM2_BASE_ADDR       0x40000000UL

#define TIM2_CR1             (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x00UL))

#define TIM2_CCMR1           (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x18UL))

#define TIM2_CCER            (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x20UL))

#define TIM2_EGR             (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x14UL))

#define TIM2_CNT             (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x24UL))

#define TIM2_PSC             (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x28UL))

#define TIM2_ARR             (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x2CUL))

#define TIM2_CCR1            (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x34UL))

#define TIM2_CCR2            (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x38UL))

#define TIM2_CCR3            (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x3CUL))

#define TIM2_CCR4            (*(volatile uint32_t *)(TIM2_BASE_ADDR + 0x40UL))


uint16_t counter = 1;
#define COUNTER_MAX 20  

#define COUNTER_MIN 1 

uint8_t  SLIDE = 5;  
uint8_t  time_trigger = DISABLE ;
uint16_t shift = 100 ; 
#define SHIFT_MIN 0
#define SHIFT_MAX 300  
uint8_t SAMPLE_TRIG =false;  
static volatile uint32_t last_press_time =0  ; 
static volatile uint32_t last_press_time_down = 0;
static volatile uint32_t last_press_time_up   = 0;
static volatile uint32_t last_press_time_time = 0;

static uint32_t last_press_time_6 = 0;
static uint32_t last_press_time_7 = 0;
#define DEBOUNCE_CYCLES (CLK_SPEED / 5)   
uint8_t  VOLTAGE_PER_DIV_INC = DISABLE ; 
uint8_t  VOLTAGE_PER_DIV_DEC = DISABLE ; 
const float volt_div_table[] =
{
    0.01f,
    0.02f,
    0.05f,
    0.10f,
    0.20f,
    0.50f,
    1.0f,
    2.0f,
    5.0f
};
uint32_t INDEX  = 0 ;
int8_t volt_div_index = 8;  
static void cycle_init (void){

    DEMCR |= (1 << 24);     
    DWT_CYCCNT = 0;
    DWT_CTRL |= 1;   

} 
// void TIM2_GPIO_Init(void)
// {
//     GPIO_Handle_t GPIOHandle;

//     GPIOHandle.pGPIOx = GPIOB;

//     GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
//     GPIOHandle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;

//     /* PB3 → TIM2_CH2 */
//     GPIOHandle.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF1;

//     GPIOHandle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
//     GPIOHandle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//     GPIOHandle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

//     GPIO_Init(&GPIOHandle);
// }

// void TIM2_PWM_Init(void)
// {
//     RCC->APB1ENR |= (1U << 0);

//     TIM2_PSC  = 83;      // 1 MHz counting clock (unchanged)
//     TIM2_ARR  = 1999;    // 500 Hz period ← changed from 999
//     TIM2_CCR2 = 999;     // 50% duty ← changed from 500

//     /* PWM Mode 1, CH2 */
//     TIM2_CCMR1 &= ~(7U << 12);
//     TIM2_CCMR1 |=  (6U << 12);

//     /* CH2 preload */
//     TIM2_CCMR1 |= (1U << 11);

//     /* Enable CH2 */
//     TIM2_CCER |= (1U << 4);

//     /* ARR preload */
//     TIM2_CR1 |= (1U << 7);

//     /* Generate update */
//     TIM2_EGR |= (1U << 0);

//     /* Start */
//     TIM2_CR1 |= (1U << 0);
// }
static void SPI2_GPIO_Init(void)
{
    GPIO_Handle_t GPIOHandle;

    GPIOHandle.pGPIOx = GPIOB;
    GPIOHandle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    GPIOHandle.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_AF5;
    GPIOHandle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GPIOHandle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIOHandle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
    GPIO_Init(&GPIOHandle);

    GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
    GPIO_Init(&GPIOHandle);

    GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
    GPIO_Init(&GPIOHandle);
}

static void SPI2_Config(void)
{
    SPIHandle.pSPI = SPI2;

    SPIHandle.SPI_Pinconfig.DeviceMode = SPI_DEVICE_MODE_MASTER;
    SPIHandle.SPI_Pinconfig.BusConfig  = SPI_BUS_CONFIG_FD;
    SPIHandle.SPI_Pinconfig.SclkSpeed  = SPI_CLK_SPEED_DIV4;
    SPIHandle.SPI_Pinconfig.DFF        = SPI_DFF_8BITS;
    SPIHandle.SPI_Pinconfig.CPOL       = SPI_CPOL_LOW;
    SPIHandle.SPI_Pinconfig.CPHA       = SPI_CPHA_LOW;
    SPIHandle.SPI_Pinconfig.SSM        = SPI_SSM_EN;

    SPI_Init(&SPIHandle);
    SPI_SSIConfig(SPI2, ENABLE);
}

static void LCD_Pins_Init(void)
{
    GPIO_Handle_t GPIOHandle;
 

    GPIOHandle.pGPIOx = GPIOD;
    GPIOHandle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GPIOHandle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GPIOHandle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIOHandle.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
    GPIO_Init(&GPIOHandle);

    GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
    GPIO_Init(&GPIOHandle);

    GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
    GPIO_Init(&GPIOHandle);

    GPIOHandle.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
    GPIO_Init(&GPIOHandle);
    GPIO_WriteToOutputPin(GPIOD, GPIO_PIN_NO_0, ENABLE);

    GPIO_WriteToOutputPin(GPIOD, GPIO_PIN_NO_3, ENABLE);

    LCD.pSPIHandle = &SPIHandle;

    LCD.CSPort = GPIOD;
    LCD.CSPin  = GPIO_PIN_NO_0;

    LCD.DCPort = GPIOD;
    LCD.DCPin  = GPIO_PIN_NO_1;

    LCD.ResetPort = GPIOD;
    LCD.ResetPin  = GPIO_PIN_NO_2;

    LCD.BLPort = GPIOD;
    LCD.BLPin  = GPIO_PIN_NO_3;
}
void EXTI0_IRQHandler(void){

	GPIO_IRQHandler(GPIO_PIN_NO_0);
    uint32_t now = DWT_CYCCNT ; 
    if(now  - last_press_time  > DEBOUNCE_CYCLES*5){ 
        found =ENABLE ;
        last_press_time = DWT_CYCCNT ; 
    }
}
void EXTI2_IRQHandler(void){

	GPIO_IRQHandler(GPIO_PIN_NO_2);

	uint32_t now = DWT_CYCCNT;
	if ((now - last_press_time_down) > DEBOUNCE_CYCLES) {
	
		if (shift > SHIFT_MIN) shift = shift - 5;
		slide_trigger = ENABLE;
		last_press_time_down = now;
	}
}


void EXTI3_IRQHandler(void){

	GPIO_IRQHandler(GPIO_PIN_NO_3);
    uint32_t now = DWT_CYCCNT;
	if ((now - last_press_time_up) > DEBOUNCE_CYCLES) {
		if (shift < SHIFT_MAX) shift = shift + 5;
		slide_trigger = ENABLE;
		last_press_time_up = now;
	}
}
void EXTI4_IRQHandler(void){

	GPIO_IRQHandler(GPIO_PIN_NO_4);
    uint32_t now = DWT_CYCCNT;
	if ((now - last_press_time_time) > DEBOUNCE_CYCLES) {
		time_trigger = ENABLE;
if (counter < COUNTER_MAX) counter = counter + 1;
		last_press_time_time = now;
	}
}
void EXTI9_5_IRQHandler(void ){
    if(EXTI->PR & (1<<5)) {
        // INTERRUPT for pin NUMBER  5 ;
    GPIO_IRQHandler(GPIO_PIN_NO_5) ;
    uint32_t  now = DWT_CYCCNT ;
    if((now - last_press_time_time)>DEBOUNCE_CYCLES){
        time_trigger = ENABLE ;
        if(counter >COUNTER_MIN) counter = counter -  1 ;
        last_press_time_time = now ;
    }
}

    uint32_t now = DWT_CYCCNT;
  if(EXTI->PR & (1 << 8))
    {
        GPIO_IRQHandler(GPIO_PIN_NO_8);
        if((now - last_press_time_6) > DEBOUNCE_CYCLES){
            VOLTAGE_PER_DIV_INC = ENABLE;
        if(volt_div_index < (int8_t )((sizeof(volt_div_table) / sizeof(volt_div_table[0])) - 1))
                volt_div_index++;
        last_press_time_6 = now;
        }
    }

    if(EXTI->PR & (1 << 7)) {
        GPIO_IRQHandler(GPIO_PIN_NO_7);
         if((now - last_press_time_7) > DEBOUNCE_CYCLES){
            VOLTAGE_PER_DIV_DEC = ENABLE;
 if(volt_div_index > 0)volt_div_index--;
            last_press_time_7 = now;
        }
    }
}

void TIM3_IRQHandler(void ){ 

    TIM3_ClearUpdateFlag() ; 
    SAMPLE_TRIG =true ;   
    INDEX++ ; 
        
}

int main(void){

    // TIM2_GPIO_Init();
    // TIM2_PWM_Init();
    SCB_CPACR |= (0xF << 20) ;

    SystemClock_Config_168MHz();

    GPIO_PeriClockControl(GPIOA, ENABLE);
    GPIO_PeriClockControl(GPIOB, ENABLE);
    GPIO_PeriClockControl(GPIOD, ENABLE);

    SPI2_GPIO_Init();
    SPI2_Config();
    SPI_PeripheralControl(SPI2, ENABLE) ;
    LCD_Pins_Init();
    LCD_Init(&LCD);
    LCD_FillColor(&LCD, LCD_COLOR_BLACK);

    LCD_WriteText(&LCD,10,100,"oscilloscope",LCD_COLOR_WHITE,LCD_COLOR_BLACK,2);



    GPIO_Handle_t ADC_GPIO;
    ADC_GPIO.pGPIOx = GPIOA;
    ADC_GPIO.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    ADC_GPIO.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
    ADC_GPIO.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&ADC_GPIO);ADC_Handle_t ADC1Handle;
    ADC1Handle.pADCx = ADC1;
    for(int i=0;i<16;i++){
        ADC1Handle.ADC_PinConfig.ADC_Channel[i]=DISABLE;
        ADC1Handle.ADC_PinConfig.ADC_Sample_Time[i]=ADC_SAMPLE_TIME_3CYCLES;
    }

    ADC1Handle.ADC_PinConfig.ADC_Channel[1]=ENABLE;
    ADC1Handle.ADC_PinConfig.ADC_Res=ADC_RES_12;

    ADC1Handle.ADC_PinConfig.ADC_Sample_Time[1]=ADC_SAMPLE_TIME_84CYCLES;

    ADC_Init(&ADC1Handle);

  GPIO_Handle_t USER_BUTTON ;
  USER_BUTTON.pGPIOx  =GPIOA ;
  USER_BUTTON.GPIO_PinConfig.GPIO_PinMode =GPIO_MODE_IT_RT ;
  USER_BUTTON.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0 ;
  USER_BUTTON.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD ;
  USER_BUTTON.GPIO_PinConfig.GPIO_PinSpeed= GPIO_SPEED_FAST ;
  GPIO_Init(&USER_BUTTON ) ;
  GPIO_IRQConfig(IRQ_NO_EXTI0, ENABLE);
  GPIO_IRQPriorityConfig(IRQ_NO_EXTI0, 2);
GPIO_Handle_t  LED_USER ; 
LED_USER.pGPIOx = GPIOD ; 
LED_USER.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT ; 
LED_USER.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12 ; 
LED_USER.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP ;
LED_USER.GPIO_PinConfig.GPIO_PinSpeed  =GPIO_SPEED_FAST ; 
LED_USER.GPIO_PinConfig.GPIO_PinPuPdControl=  GPIO_NO_PUPD ; 
GPIO_Init(&LED_USER ) ; 

   

  //  slide down and slide up button  ; 
  GPIO_Handle_t  SLIDE_DOWN_BUTTON ; 
    SLIDE_DOWN_BUTTON.pGPIOx = GPIOA ; 
    SLIDE_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT ;
    SLIDE_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2 ;
    SLIDE_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD ;
    SLIDE_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST ;
    GPIO_Init(&SLIDE_DOWN_BUTTON) ;
    GPIO_IRQConfig(IRQ_NO_EXTI2, ENABLE) ;
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI2, 3) ;
    
    GPIO_Handle_t SLIDE_UP_BUTTON ;
    SLIDE_UP_BUTTON.pGPIOx = GPIOA ;
    SLIDE_UP_BUTTON.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT ;
    SLIDE_UP_BUTTON.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3 ;
    SLIDE_UP_BUTTON.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;
    SLIDE_UP_BUTTON.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST ;
    GPIO_Init(&SLIDE_UP_BUTTON) ;
    GPIO_IRQConfig(IRQ_NO_EXTI3, ENABLE) ;
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI3, 4) ;

    GPIO_Handle_t TIME_BUTTON ; 
    TIME_BUTTON.pGPIOx = GPIOA ;
    TIME_BUTTON.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT ;
    TIME_BUTTON.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_4 ;
    TIME_BUTTON.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD ;
    TIME_BUTTON.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST ;
    GPIO_Init(&TIME_BUTTON) ;
    GPIO_IRQConfig(IRQ_NO_EXTI4, ENABLE) ;
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI4, 5) ;
   
    // LCD_DrawLine(&LCD, 0,   160,  239, 160, LCD_COLOR_WHITE);
    // LCD_DrawLine(&LCD, 120,   0, 120, 319, LCD_COLOR_WHITE);

    GPIO_Handle_t TIME_DOWN_BUTTON ; 
    TIME_DOWN_BUTTON.pGPIOx = GPIOA ;
    TIME_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT ;
    TIME_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5 ;
    TIME_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD ;
    TIME_DOWN_BUTTON.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST ;
    GPIO_Init(&TIME_DOWN_BUTTON) ;
    GPIO_IRQConfig(IRQ_NO_EXTI9_5, ENABLE) ;
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, 6) ;




    GPIO_Handle_t VOLTAGE_BUTTON_DOWN ; 
    VOLTAGE_BUTTON_DOWN.pGPIOx = GPIOA; 
    VOLTAGE_BUTTON_DOWN.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT ;
    VOLTAGE_BUTTON_DOWN.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8 ;
    VOLTAGE_BUTTON_DOWN.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD ;
    VOLTAGE_BUTTON_DOWN.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST ;
    GPIO_Init(&VOLTAGE_BUTTON_DOWN) ;
    GPIO_IRQConfig(IRQ_NO_EXTI9_5, ENABLE) ;
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, 7) ;


     GPIO_Handle_t VOLTAGE_BUTTON_UP ; 
    VOLTAGE_BUTTON_UP.pGPIOx = GPIOA; 
    VOLTAGE_BUTTON_UP.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RT ;
    VOLTAGE_BUTTON_UP.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7 ;
    VOLTAGE_BUTTON_UP.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD ;
    VOLTAGE_BUTTON_UP.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST ;
    GPIO_Init(&VOLTAGE_BUTTON_UP) ;
    GPIO_IRQConfig(IRQ_NO_EXTI9_5, ENABLE) ;
    GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, 8) ;

    // timer init  section 

    Timer_Handle_t TIMER3; 
    TIMER3.TIM3_ARR_VALUE=  9 ; //  TO COUNT  UPTO 10us ;
    TIMER3.TIM3_COUNTING_MODE = UPCOUNTING ; 
    TIMER3.TIM3_PRES_Value = 83;  //  CLK_CNT = (CLK_OF THE MCU )  /(PRESCALER+1) ;
    TIM3_Init(&TIMER3) ;  //  PRIPPERTY IS  9 ; 

   while(1){
//  LCD_WriteText(&LCD , 100 , 210 , "V(in v) :", LCD_COLOR_WHITE ,  
               // LCD_COLOR_BLACK  ,1  ) ;
            GPIO_WriteToOutputPin(GPIOD ,  GPIO_PIN_NO_12 ,  DISABLE ) ; 
    	       static uint16_t sample_value [SAMPLES] ; 
               cycle_init();
                uint32_t start_time = DWT_CYCCNT;
                TIM3_StartCount(&TIMER3) ; 
                while(INDEX<SAMPLES) { 
    if(SAMPLE_TRIG ){
                      ADC_StartConversion(&ADC1Handle);
                      sample_value[INDEX] = ADC_ReadValue(&ADC1Handle);
                      SAMPLE_TRIG = false ; 
                }
    }
    uint32_t end_time = DWT_CYCCNT;
    INDEX = 0 ; 
    GPIO_WriteToOutputPin(GPIOD ,GPIO_PIN_NO_12, ENABLE ) ; 
    TIM3_StopCount(&TIMER3) ; 
    
 float T_window =0.5f;  
 float prev_temp_value = (float )sample_value[0]   *(3.3f)/(4095.0f) ; 
 uint32_t crossing = 0 ;  
 for(uint32_t  i = 1 ; i<SAMPLES ; i++){
     float invalue = (float )sample_value[i] *3.3f/4095.0f; //  12 bit res  
     if(invalue >=THRESHOLD  &&  prev_temp_value <THRESHOLD)crossing++ ; 
     prev_temp_value = invalue ; 

 }

 float frequceny_measured = (float )crossing/T_window ; 
 char str_freq[20] ;
 snprintf(str_freq ,  sizeof(str_freq) ,  "%.2f Hz" ,  frequceny_measured) ; 
   
 float total_time_s = (float)(end_time - start_time) / (float)CLK_SPEED;
                float sample_rate   = (float)SAMPLES / total_time_s;
                uint16_t v_max = sample_value[0];
                uint16_t v_min = sample_value[0];
                for (uint16_t i = 1; i < SAMPLES; i++) {
                    if (sample_value[i] > v_max) v_max = sample_value[i];
                    if (sample_value[i] < v_min) v_min = sample_value[i];
                }

                char str_one[20], str_two[20];
                float v_max_value = (v_max * 3.3f) / 4095.0f;  
                float v_min_value = (v_min * 3.3f) / 4095.0f;
                snprintf(str_one, sizeof(str_one), "%.2f", v_max_value);
                snprintf(str_two, sizeof(str_two), "%.2f", v_min_value);
                 LCD_FillColor(&LCD,  LCD_COLOR_BLACK) ;
    	    	for(uint16_t y = 0; y < 320; y += 40){
    	    	    for(uint16_t x = 0; x < 240; x += 8){
    	    	        LCD_DrawLine(&LCD, x, y, x + 3, y, LCD_COLOR_WHITE);
    	    	    }
    	    	}
    	      for(uint16_t x = 0; x < 240; x += 40){
    	    	    for(uint16_t y = 0; y < 320; y += 8	) {
    	    	        LCD_DrawLine(&LCD, x, y, x, y + 3, LCD_COLOR_WHITE);
    	    	    }
    	    	}
                    LCD_Fill(&LCD,200,0,239,319,LCD_COLOR_YELLOW);
                LCD_WriteText(&LCD, 20, 220, "Vmax", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 50, 220, str_one, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 90, 220, "Vmin", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 120, 220, str_two, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);

while(ENABLE){
     
    if(found ) break; 

    if(slide_trigger || time_trigger){
            slide_trigger = DISABLE ;
            time_trigger = DISABLE ;
               LCD_FillColor(&LCD,  LCD_COLOR_BLACK) ;
    	    	for(uint16_t y = 0; y < 320; y += 40){
    	    	    for(uint16_t x = 0; x < 240; x += 8){
    	    	        LCD_DrawLine(&LCD, x, y, x + 3, y, LCD_COLOR_WHITE);
    	    	    }
    	    	}
    	      for(uint16_t x = 0; x < 240; x += 40){
    	    	    for(uint16_t y = 0; y < 320; y += 8	) {
    	    	        LCD_DrawLine(&LCD, x, y, x, y + 3, LCD_COLOR_WHITE);
    	    	    }
    	    	}
    LCD_Fill(&LCD,200,0,239,319,LCD_COLOR_YELLOW);
                LCD_WriteText(&LCD, 20, 220, "Vmax", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 50, 220, str_one, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 90, 220, "Vmin", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 120, 220, str_two, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
    }
              float scale = (40.f/  volt_div_table[volt_div_index]) ;
    if(VOLTAGE_PER_DIV_INC){
        VOLTAGE_PER_DIV_INC = DISABLE ; 
        scale = (40.0f /volt_div_table[volt_div_index]) ;    
         LCD_FillColor(&LCD,  LCD_COLOR_BLACK) ;
    	    	for(uint16_t y = 0; y < 320; y += 40){
    	    	    for(uint16_t x = 0; x < 240; x += 8){
    	    	        LCD_DrawLine(&LCD, x, y, x + 3, y, LCD_COLOR_WHITE);
    	    	    }
    	    	}
    	      for(uint16_t x = 0; x < 240; x += 40){
    	    	    for(uint16_t y = 0; y < 320; y += 8	) {
    	    	        LCD_DrawLine(&LCD, x, y, x, y + 3, LCD_COLOR_WHITE);
    	    	    }
    	    	}
    LCD_Fill(&LCD,200,0,239,319,LCD_COLOR_YELLOW);
                LCD_WriteText(&LCD, 20, 220, "Vmax", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 50, 220, str_one, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 90, 220, "Vmin", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 120, 220, str_two, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
    }
    if(VOLTAGE_PER_DIV_DEC){
        VOLTAGE_PER_DIV_DEC = DISABLE ; 
        scale = (40.0f /volt_div_table[volt_div_index]) ;  
         LCD_FillColor(&LCD,  LCD_COLOR_BLACK) ;
    	    	for(uint16_t y = 0; y < 320; y += 40){
    	    	    for(uint16_t x = 0; x < 240; x += 8){
    	    	        LCD_DrawLine(&LCD, x, y, x + 3, y, LCD_COLOR_WHITE);
    	    	    }
    	    	}
    	      for(uint16_t x = 0; x < 240; x += 40){
    	    	    for(uint16_t y = 0; y < 320; y += 8	) {
    	    	        LCD_DrawLine(&LCD, x, y, x, y + 3, LCD_COLOR_WHITE);
    	    	    }
                    }       LCD_Fill(&LCD,200,0,239,319,LCD_COLOR_YELLOW);

                LCD_WriteText(&LCD, 20, 220, "Vmax", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 50, 220, str_one, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 90, 220, "Vmin", LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
                LCD_WriteText(&LCD, 120, 220, str_two, LCD_COLOR_BLACK, LCD_COLOR_YELLOW, 1);
               
    }
    float time_per_div = 0.001f* ((float)counter);        
    float samples_per_div = sample_rate * time_per_div;
    

    float samples_per_pixel = samples_per_div / PIXELS_PER_DIV;

   if (samples_per_pixel < 1.0f) samples_per_pixel = 1.0f;

                uint16_t prev_x = 0, prev_y = 0;
                bool first_point = true;
                char str_three[20];
snprintf(str_three,sizeof(str_three), "%.2f ms/div", time_per_div * 1000.0f);
LCD_WriteText(&LCD, 160, 220, str_three,LCD_COLOR_BLACK,LCD_COLOR_YELLOW, 1);
char str_four[20] ; 
snprintf(str_four,sizeof(str_four), "%.2f V/div", volt_div_table[volt_div_index]) ;

LCD_WriteText(&LCD, 240, 220, str_four,LCD_COLOR_BLACK,LCD_COLOR_YELLOW, 1);
LCD_WriteText(&LCD ,  20  ,20 ,  "Freq" ,  LCD_COLOR_WHITE ,  LCD_COLOR_BLACK ,1 ) ; 
LCD_WriteText(&LCD ,70  ,20 , str_freq , LCD_COLOR_WHITE ,  LCD_COLOR_BLACK , 1 ) ; 

float count = 0.0f;
for(uint16_t i = 0; i < LCD_HEIGHT; i++){
    uint16_t index = (uint16_t)count;

    if(index >= SAMPLES) break;
    float value_to_write =sample_value[index] * 3.3f / 4095.0f;
    uint16_t y = shift + (uint16_t)(value_to_write *scale );
    LCD_DrawPixel(&LCD, y, i, LCD_COLOR_GREEN);

    if(first_point){
        prev_x = y; prev_y = i; first_point = false;
    }
    else{
        LCD_DrawLine(&LCD, prev_x, prev_y, y, i,LCD_COLOR_GREEN);
        prev_x = y;
        prev_y = i;
    }

    count = count+ samples_per_pixel;
 }

}
 found = DISABLE;

    	   }
      return 0 ; 
    }
