/*
 * stm32f405xxx_spi_driver.c
 *
 *  Created on: Jul 2, 2026
 *      Author: Ashwin
 */
#include"stm32f405xxx.h"
#include"stm32f405xxx_spi_driver.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)  ;
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle) ;
static void spi_ovr_interrupt_handle(SPI_Handle_t *pSPIHandle)  ;

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx  , uint8_t EnORDi){

	 if(EnORDi == ENABLE ){
		 // ENABLE THE CLOCK
		if(pSPIx == SPI1) RCC->APB2ENR |=(1<<12) ;
		if(pSPIx == SPI2) RCC->APB1ENR |=(1<<14) ;
		if(pSPIx == SPI3) RCC->APB1ENR |=(1<<15) ;
		if(pSPIx == SPI4) RCC->APB2ENR |=(1<<13) ;

 }
	 else {
		 // DISABLE
			if(pSPIx == SPI1) RCC->APB2ENR &=~(1<<12) ;
			if(pSPIx == SPI2) RCC->APB1ENR &=~(1<<14) ;
			if(pSPIx == SPI3) RCC->APB1ENR &=~(1<<15) ;
			if(pSPIx == SPI4) RCC->APB2ENR &=~(1<<13) ;

	 }
}

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
    if(EnOrDi == ENABLE)
    {
        pSPIx->CR1 |= (1 <<8 );
    }
    else
    {
        pSPIx->CR1 &= ~(1 << 8);
    }
}
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pSPIx->CR2 |= (1 << 2);    // Set SSOE bit
    }
    else
    {
        pSPIx->CR2 &= ~(1 << 2);   // Clear SSOE bit
    }
}
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx ,uint8_t ENorDI){
	if(ENorDI ==ENABLE ){
		pSPIx->CR1 |= (1<<6) ;
	}
	else {
		pSPIx->CR1 &= ~(1<<6) ;
	}
}

void SPI_Init(SPI_Handle_t *pSPIHandle){


	// ENABLE CLOCK CONTROL ;
	SPI_PeriClockControl(pSPIHandle->pSPI  ,  ENABLE ) ;  //  base address of the SPI respective base address  ;
     // SPE REGISTER CONFIGURATION

    if(pSPIHandle->SPI_Pinconfig.DeviceMode == SPI_DEVICE_MODE_MASTER)
        pSPIHandle->pSPI->CR1 |= (1 << 2);      // MSTR
    else
        pSPIHandle->pSPI->CR1 &= ~(1 << 2);


    if(pSPIHandle->SPI_Pinconfig.BusConfig == SPI_BUS_CONFIG_FD)
    {
        // Full Duplex
        pSPIHandle->pSPI->CR1 &= ~(1 << 15);     // BIDIMODE = 0
        pSPIHandle->pSPI->CR1 &= ~(1 << 10);     // RXONLY = 0
    }

    else if(pSPIHandle->SPI_Pinconfig.BusConfig == SPI_BUS_CONFIG_HD)
    {
        // Half Duplex
        pSPIHandle->pSPI->CR1 |= (1 << 15);      // BIDIMODE = 1
    }

    else if(pSPIHandle->SPI_Pinconfig.BusConfig == SPI_BUS_CONFIG_S_RXONLY)
    {
        // Simplex Receive Only
        pSPIHandle->pSPI->CR1 &= ~(1 << 15);     // BIDIMODE = 0
        pSPIHandle->pSPI->CR1 |= (1 << 10);      // RXONLY = 1
    }


    pSPIHandle->pSPI->CR1 &= ~(7 << 3);          // Clear BR bits
    pSPIHandle->pSPI->CR1 |= (pSPIHandle->SPI_Pinconfig.SclkSpeed << 3);



    if(pSPIHandle->SPI_Pinconfig.DFF == SPI_DFF_16BITS)
        pSPIHandle->pSPI->CR1 |= (1 << 11);
    else
        pSPIHandle->pSPI->CR1 &= ~(1 << 11);



    if(pSPIHandle->SPI_Pinconfig.CPOL == SPI_CPOL_HIGH)
        pSPIHandle->pSPI->CR1 |= (1 << 1);
    else
        pSPIHandle->pSPI->CR1 &= ~(1 << 1);



    if(pSPIHandle->SPI_Pinconfig.CPHA == SPI_CPHA_HIGH)
        pSPIHandle->pSPI->CR1 |= (1 << 0);
    else
        pSPIHandle->pSPI->CR1 &= ~(1 << 0);


    if(pSPIHandle->SPI_Pinconfig.SSM == SPI_SSM_EN){
        pSPIHandle->pSPI->CR1 |= (1 << 9);
    pSPIHandle->pSPI->CR1 |= (1 << 8);
}
    else
        pSPIHandle->pSPI->CR1 &= ~(1 << 9);
}
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx , uint8_t FlagName ){
	if(pSPIx->SR  &(1<<FlagName))return 1 ;
	return 0 ;
}
//  BLOCKING CALL



void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
    if(pSPIx == SPI1)
    {
        RCC->APB2RSTR |= (1<<12);
        RCC->APB2RSTR &= ~(1<<12);
    }
    else if(pSPIx == SPI2)
    {
        RCC->APB1RSTR |= (1<<14);
        RCC->APB1RSTR &= ~(1<<14);
    }
    else if(pSPIx == SPI3)
    {
        RCC->APB1RSTR |= (1<<15);
        RCC->APB1RSTR &= ~(1<<15);
    }
    else if(pSPIx == SPI4)
    {
        RCC->APB2RSTR |= (1<<13);
        RCC->APB2RSTR &= ~(1<<13);
    }
}
void SPI_SendData(SPI_RegDef_t *pSPIx , uint8_t *pTxBuffer ,  uint32_t len   ) {

	while(len>0){

		while(!SPI_GetFlagStatus(pSPIx , 1 )) ;  //  wait for status flag to become 1 ;
 	    if(pSPIx->CR1 & (1<<11)){
 	    	 //  THE BIT IS SET AND 16 BIT DATA FRAME
 	    	pSPIx->DR = *((uint16_t*)pTxBuffer) ;
 	    	len = len-2 ; //  2 bytes of data is sent
 	    	pTxBuffer += 2;
 	    }
 	    else {
 	    	pSPIx->DR = *(pTxBuffer) ;
 	    	len = len -1 ;
 	    	pTxBuffer++ ;

 	    }
	}
	 while (pSPIx->SR & (1 << 7));
}
void SPI_ReceiveData(SPI_RegDef_t *pSPIx , uint8_t *pRxBuffer ,  uint32_t len   ) {

	while(len>0){

		while(!SPI_GetFlagStatus(pSPIx , 0 )) ;  //  wait for status flag to become 1 ;
 	    if(pSPIx->CR1 & (1<<11)){
 	    	 //  THE BIT IS SET AND 16 BIT DATA FRAME
 	       *((uint16_t*)pRxBuffer)  = pSPIx->DR;
 	    	len = len-2 ; //  2 bytes of data received
 	    	pRxBuffer += 2;
 	    }
 	    else {
 	    	*(pRxBuffer)  = pSPIx->DR;
 	    	len = len -1 ;
 	    	pRxBuffer++ ;
 	    }
	}
}
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle , uint8_t *pTxBuffer ,  uint32_t len   ) {

	uint8_t state  = pSPIHandle->TxState ;
		if(state !=SPI_BUSY_IN_TX){
         pSPIHandle->pTxBuffer= pTxBuffer ;  //  address of the data is stored in the global varaible ;
         pSPIHandle->TxLen = len ;
         pSPIHandle->TxState = SPI_BUSY_IN_TX ;
         pSPIHandle->pSPI->CR2 |=(1<<7) ;
	}
return state ;
}
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle , uint8_t *pRxBuffer ,  uint32_t len   ){

	uint8_t state = pSPIHandle->RxState ;


	if(state  !=SPI_BUSY_IN_RX){
        pSPIHandle->pRxBuffer= pRxBuffer ;  //  address of the data is stored in the global varaible ;
        pSPIHandle->RxLen = len ;
        pSPIHandle->RxState = SPI_BUSY_IN_RX ;
        pSPIHandle->pSPI->CR2 |=(1<<6) ;

	}
	return state;
}
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
    uint8_t temp1, temp2;

    // TXE
    temp1 = pSPIHandle->pSPI->SR & (1 << 1);
    temp2 = pSPIHandle->pSPI->CR2 & (1 << 7);

    if(temp1 && temp2)
    {
        // Handle TXE interrupt
    	spi_txe_interrupt_handle(pSPIHandle ) ;

    }

    // RXNE
    temp1 = pSPIHandle->pSPI->SR & (1 << 0);
    temp2 = pSPIHandle->pSPI->CR2 & (1 << 6);

    if(temp1 && temp2)
    {
        // Handle RXNE interrupt
    	spi_rxne_interrupt_handle(pSPIHandle ) ;

    }

    // OVR
    temp1 = pSPIHandle->pSPI->SR & (1 << 6);
    temp2 = pSPIHandle->pSPI->CR2 & (1 << 5);

    if(temp1 && temp2)
    {
        // Handle Overrun interrupt
    	spi_ovr_interrupt_handle (pSPIHandle) ;
    }
}
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    if(pSPIHandle->pSPI->CR1 & (1 << 11))
    {
        // 16-bit DFF
        pSPIHandle->pSPI->DR = *((uint16_t *)pSPIHandle->pTxBuffer);

        pSPIHandle->pTxBuffer += 2;
        pSPIHandle->TxLen -= 2;
    }
    else
    {
        // 8-bit DFF
        pSPIHandle->pSPI->DR = *pSPIHandle->pTxBuffer;

        pSPIHandle->pTxBuffer++;
        pSPIHandle->TxLen--;
    }

    if(pSPIHandle->TxLen == 0)
    {
        // Disable TXE interrupt
        pSPIHandle->pSPI->CR2 &= ~(1 << SPI_CR2_TXEIE);

        pSPIHandle->pTxBuffer = NULL;
        pSPIHandle->TxLen = 0;
        pSPIHandle->TxState = SPI_READY;

        // Optional callback
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
    }
}
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    if(pSPIHandle->pSPI->CR1 & (1 << 11))
    {
        // 16-bit DFF
        *((uint16_t *)pSPIHandle->pRxBuffer) =
                (uint16_t)pSPIHandle->pSPI->DR;

        pSPIHandle->pRxBuffer += 2;
        pSPIHandle->RxLen -= 2;
    }
    else
    {
        // 8-bit DFF
        *(pSPIHandle->pRxBuffer) =
                (uint8_t)pSPIHandle->pSPI->DR;

        pSPIHandle->pRxBuffer++;
        pSPIHandle->RxLen--;
    }

    if(pSPIHandle->RxLen == 0)
    {
        // Disable RXNE interrupt
        pSPIHandle->pSPI->CR2 &= ~(1 << SPI_CR2_RXNEIE);

        pSPIHandle->pRxBuffer = NULL;
        pSPIHandle->RxLen = 0;
        pSPIHandle->RxState = SPI_READY;

        // Optional callback
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
    }
}
static void spi_ovr_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    uint8_t temp;

    if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
    {
        temp = pSPIHandle->pSPI->DR;
        temp = pSPIHandle->pSPI->SR;
    }

    (void)temp;

    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}


void SPI_ClearOVRFlag(SPI_RegDef_t  *pSPIx){
	uint8_t temp ;
	temp = pSPIx->DR  ;
	temp = pSPIx->SR ;


	(void )temp ;
}

__attribute__((weak))  void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle , uint8_t AppEv  ){
	// overrides ;
}
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            *NVIC_ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber <= 63)
        {
            *NVIC_ISER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber <= 95)
        {
        	// comes under lot of things
            *NVIC_ISER2 |= (1 << (IRQNumber % 64));
        }
    }
    else
    {
        if(IRQNumber <= 31)
        {
            *NVIC_ICER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber <= 63)
        {
            *NVIC_ICER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber <= 95)
        {
            *NVIC_ICER2 |= (1 << (IRQNumber % 64));
        }
    }
}
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
    uint8_t iprx       = IRQNumber / 4;
    uint8_t iprx_sec   = IRQNumber % 4;

    uint8_t shift = (8 * iprx_sec) + (8 -4);

    *(NVIC_PR_BASEADDR + iprx) &= ~(0xF << shift);
    *(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift);
}
