#include "DMA1.h"
#include <stdlib.h>
#include "GameFrame.h"

// Configuring DMA1 Channel 7 as the USART2 TX channel.
void DMA1_ch7_cfg(void)
{
	// Enable the DMA1 clock in the AHB1 peripheral clock enable register
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	
	// Map USART2 TX to DMA1 Channel 7
	// C7S[3:0] = '0010'
	DMA1_CSELR->CSELR &= ~DMA_CSELR_C7S;
	DMA1_CSELR->CSELR |= 0b0010 << DMA_CSELR_C7S_Pos;
	
	// Disabling DMA1 Channel 7 to allow configuration
	DMA1_Channel7->CCR &= ~DMA_CCR_EN;
	
	// Peripheral data size for each DMA transfer: '00' equals 8 bits.
	DMA1_Channel7->CCR &= ~DMA_CCR_PSIZE;
	// Memory data size for each DMA transfer: '00' equals 8 bits.
	DMA1_Channel7->CCR &= ~DMA_CCR_MSIZE;
	// Disable peripheral increment mode (no incrementation after each transfer)
	DMA1_Channel7->CCR &= ~DMA_CCR_PINC;
	// Enable memory increment mode (memory address incremented by 1 after each transfer)
	DMA1_Channel7->CCR |= DMA_CCR_MINC;
	// Configure transfer direction: '1' indicates from memory to peripheral.
	DMA1_Channel7->CCR |= DMA_CCR_DIR;

	// Peripheral address: the address of the USART2 TDR register
	DMA1_Channel7->CPAR = (uint32_t)&(USART2->TDR);
	// Memory address: the address of the memory buffer array
	DMA1_Channel7->CMAR = (uint32_t)frameString;
	
	// Disable Circular Mode for DMA1 Channel 7
	DMA1_Channel7->CCR &= ~DMA_CCR_CIRC;
	
	// Enable the transfer complete interrupt for DMA1 Channel 7
	DMA1_Channel7->CCR |= DMA_CCR_TCIE;
	// Enable the DMA1 Channel7 interrupt in the NVIC
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
	//DMA is enabled in GameFrame.c


}

void Enable_DMA(uint16_t lengthInBytes) {
	// we want to transmit the contents of the entire buffer which can vary in length

	DMA_busy = 1;

	// Number of data to transfer
	// Register only holds up to 2^16-1
	DMA1_Channel7->CNDTR = lengthInBytes;
	// Enable DMA1 Channel 7 after completing all configurations.
	DMA1_Channel7->CCR |= DMA_CCR_EN;

	// use Enable_DMA(strlen(<Buffer Address>)); to call this function
}

//only called within handler, using inline register assignment
void Disable_DMA(void) {
	// Disable DMA1 Channel 7 after buffer transfer complete.
	//DMA1_Channel7->CCR &= ~DMA_CCR_EN;
}



// Interrupt handler for DMA1 Channel 7
void DMA1_CH7_IRQHandler(void)
{
	
	// Check whether the DMA1 Channel 7 Transfer Complete interrupt has been triggered
	if((DMA1->ISR&DMA_ISR_TCIF7)==DMA_ISR_TCIF7)
	{

		//Disable_DMA();
		DMA1_Channel7->CCR &= ~DMA_CCR_EN;

		// Clear interrupt by writing 1 to the IFCR register
		DMA1->IFCR |= DMA_IFCR_CTCIF7;
		DMA_busy = 0;
		
	}
}
