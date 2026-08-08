/*
 * stm32f405xxx_spi_driver.h
 *
 *  Created on: Jul 2, 2026
 *      Author: Ashwin
 */

#ifndef INC_STM32F405XXX_SPI_DRIVER_H_
#define INC_STM32F405XXX_SPI_DRIVER_H_

#include"stm32f405xxx.h"

typedef struct
{
    uint32_t DeviceMode;      // Master or Slave

    uint32_t BusConfig;       // Full Duplex, Half Duplex, Simplex RX

    uint32_t SclkSpeed;       // SPI clock speed

    uint32_t DFF;             // Data Frame Format (8-bit/16-bit)

    uint32_t CPOL;            // Clock Polarity

    uint32_t CPHA;            // Clock Phase

    uint32_t SSM;             // Software Slave Management

} SPI_PinConfig_t;

typedef struct{
    SPI_RegDef_t *pSPI ;
	SPI_PinConfig_t SPI_Pinconfig ;

    uint8_t  *pTxBuffer;   /*!< To store the application Tx buffer address */
    uint8_t  *pRxBuffer;   /*!< To store the application Rx buffer address */

    uint32_t TxLen;        /*!< To store Tx length */
    uint32_t RxLen;        /*!< To store Rx length */

    uint8_t  TxState;      /*!< To store Tx state */
    uint8_t  RxState;      /*!< To store Rx state */
}SPI_Handle_t ;

#define SPI_SR_RXNE      0
#define SPI_SR_TXE       1
#define SPI_SR_OVR       6

#define SPI_CR2_RXNEIE   6
#define SPI_CR2_TXEIE    7
#define SPI_CR2_ERRIE    5

#define SPI_READY 0
#define SPI_BUSY_IN_RX 1
#define SPI_BUSY_IN_TX 2


#define SPI_DEVICE_MODE_MASTER 1
#define SPI_DEVICE_MODE_SLACE  0


#define SPI_BUS_CONFIG_FD 1 // - FULL DUPLEX
#define SPI_BUS_CONFIG_HD 2// - HALF DUPLEX ;


#define SPI_BUS_CONFIG_S_RXONLY 3

#define SPI_CLK_SPEED_DIV2 0
#define SPI_CLK_SPEED_DIV4 1
#define SPI_CLK_SPEED_DIV8 2
#define SPI_CLK_SPEED_DIV16 3
#define SPI_CLK_SPEED_DIV32 4
#define SPI_CLK_SPEED_DIV64 5
#define SPI_CLK_SPEED_DIV128 6
#define SPI_CLK_SPEED_DIV256 7


#define SPI_DFF_8BITS 0
#define SPI_DFF_16BITS 1


#define SPI_CPOL_HIGH 1
#define SPI_CPOL_LOW  0


#define SPI_CPHA_HIGH 1
#define SPI_CPHA_LOW  0


#define SPI_SSM_EN  1
#define SPI_SSM_DI  0


#define SPI_EVENT_TX_CMPLT 1
#define SPI_EVENT_RX_CMPLT 1
#define SPI_EVENT_OVR_ERR 1



void SPI_PeriClockControl(SPI_RegDef_t *pSPIx   , uint8_t EnORDi);


void SPI_SSIConfig(SPI_RegDef_t *pSPIx  ,uint8_t  EnORDi ) ;
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi) ;

void SPI_SSOECONFIG(SPI_RegDef_t *pSPIx  , uint8_t EnOrDi ) ;
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx  , uint8_t EnorDi) ;


/*
 * Init and De-init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);



void SPI_SendData(SPI_RegDef_t *pSPIx , uint8_t *pTxBuffer ,  uint32_t len   ) ;
void SPI_ReceiveData(SPI_RegDef_t *pSPIx ,  uint8_t  *pRxBuffer , uint32_t len ) ;

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle , uint8_t *pTxBuffer ,  uint32_t len   ) ;
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,  uint8_t  *pRxBuffer , uint32_t len ) ;

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle , uint8_t AppEv  ) ;


void SPI_IRQConfig(uint8_t IRQ_Number ,   uint8_t EnOrDi);
void SPI_IRQHandling(SPI_Handle_t *pHandle);
void SPI_IRQPriorityConfig(uint8_t  IRQNumber , uint8_t IRQPriority) ;
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle) ;
void SPI_CloseReception(SPI_Handle_t *pSPIHandle) ;

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx ) ;



#endif /* INC_STM32F405XXX_SPI_DRIVER_H_ */
