#include "USART2.h"

// UART Ports:
// ===================================================
// PA2 = USART2_TX (AF7)  
// PA3 = USART2_RX (AF7)

int flag = 0;

// This function initializes the USART2 module
void USART2_Init(void) {
	// Enable the USART2 clock in the APB1 peripheral clock enable register
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	//pins initialized in gpio.c

	// Set up USART2 with the specified configurations
	// USART 2 is defined within header file, so we are able to pass it into USART_Init
	// As a pointer to USART2, this modularizes USARTx function to work with any UART we pass in
	USART_Init(USART2);

}

// This function initializes USART module with specified settings for communication.
// This function is modular and can be utilized with any USART module passed as an argument.
void USART_Init (USART_TypeDef * USARTx) {
	// data format to be set: 8 data bits, no parity, 1 start bit, and 1 stop bit		
	// baud rate to be set: 9600
	
	// Disabling USART to allow configuration
	USARTx->CR1 &= ~USART_CR1_UE;  
	
	// Configuring word length: 8 data bits
	// M bit settings: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits
	USARTx->CR1 &= ~USART_CR1_M;   
	
	// Configuring the number of stop bits: 1 stop bit
	// STOP bits settings: 00 = 1 Stop bit, 01 = 0.5 Stop bit, 10 = 2 Stop bits, 11 = 1.5 Stop bits
	USARTx->CR2 &= ~USART_CR2_STOP;
	
	// Setting the oversampling mode to 16 
	// OVER8 = 0 for oversampling by 16, 1 for oversampling by 8
	USARTx->CR1 &= ~USART_CR1_OVER8;  
	
                                    
	// Setting the baud rate to 9600 using the default APB frequency of 4,000,000 Hz
	// Calculation depends on the oversampling mode: if OVER8 = 0 (16x), BRR = f_CK / USARTDIV
	// For 9600 baud with f_CK = 4,000,000 Hz and 16x oversampling: USARTDIV = 4,000,000 / 9,600 = 417

	// For 9600 baud
	//USARTx->BRR = 417;
	// can transmit 960 bytes/s

	//For 19200 baud
	//USARTx->BRR = 209;
	// can transmit 1920 bytes/s


	//For 115200 baud:
	USARTx->BRR = 35;
	//can transmit 11520 bytes/s

	//For 230400 baud:
	//USARTx->BRR = 17;

	// Enabling the transmitter and receiver
	USARTx->CR1  |= (USART_CR1_RE | USART_CR1_TE);
	
	// Enabling DMA for transmission
	USARTx->CR3 |= USART_CR3_DMAT;

	// Enabling USART after configuration is completed
	USARTx->CR1  |= USART_CR1_UE; 
	
	// Ensuring the USART is ready for transmission and reception
	// Wait for TEACK: Transmitter Enable Acknowledge Flag
	while ( (USARTx->ISR & USART_ISR_TEACK) == 0); 
	// Wait for REACK: Receiver Enable Acknowledge Flag
	while ( (USARTx->ISR & USART_ISR_REACK) == 0); 
}
