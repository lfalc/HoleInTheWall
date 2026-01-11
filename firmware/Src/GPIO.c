#include "GPIO.h"

#define TX_PIN 2
#define RX_PIN 3
#define PB6 6
#define PB10 10

void GameTimer_PinInit() {

	//PA3
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	GPIOA->MODER &= ~GPIO_MODER_MODE3_Msk;
	GPIOA->MODER |= 0b10 << GPIO_MODER_MODE3_Pos;

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL3_Msk);
	GPIOA->AFR[0] |= (0b0010 << GPIO_AFRL_AFSEL3_Pos);

	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT3_Msk);
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD3_Msk);
}

// TIM4_CH1 on PB6
void configure_echo_pin() {
	// 1. Enable the clock to GPIO Port B
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; 		// page 252

	// 2. Configure the GPIO Mode to 'Alternative Function' mode
	GPIOB->MODER &= ~(0b11 << (2 * PB6));			// page 305
	GPIOB->MODER |= 0b10 << (2 * PB6);      	// Alternative Function(10)

	// 3. Assign the pin's alternate function to TIM4_CH1.
	GPIOB->AFR[0] &= ~(0xF << 4 * PB6);
	GPIOB->AFR[0] |= 0b0010 << 4 * PB6;
}

//TIM2_CH3 on PB10
void configure_trigger_pin() {
	// 1. Enable the clock to GPIO Port B
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; 		// page 252

	// 2. Configure GPIO Mode to 'Alternative Function' mode: Input(00), Output(01), Alternative Function(10), Analog(11)
	GPIOB->MODER &= ~(0b11 << (2 * PB10));
	GPIOB->MODER |= 0b10 << (2 * PB10);      // Alternative Function(10)

	// 3. Select PA2's alternative function as TIM2_CH3, which is 'AF1', by configuring the GPIOA_AFRL register
	GPIOB->AFR[1] &= ~(0xF << 4 * 2);
	GPIOB->AFR[1] |= 0b0001 << 4 * 2;

	// 4. Configure GPIO Output Type to 'Push-Pull': Output push-pull (0), Output open drain (1)
	GPIOB->OTYPER &= ~(1 << PB10);      // Push-pull

	// 5. Configure GPIO Push-Pull to 'No Pull-up or Pull-down': No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOB->PUPDR &= ~(0b11 << (2 * PB10));  // No pull-up, no pull-down

}

void USART_PinInit() {

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	// Configure GPIOA pins for USART2 functionality.
	// PA2 is configured as USART2_TX (AF7).
	// PA3 is configured as USART2_RX (AF7).
	// MODER: Input(00), Output(01), AlterFunc(10), Analog(11)
	GPIOA->MODER   &= ~(3<<(2*TX_PIN) | 3<<(2*RX_PIN));	// Clear the mode bits for PA2 and PA3.
	GPIOA->MODER   |=   2<<(2*TX_PIN) | 2<<(2*RX_PIN); // Set PA2 and PA3 to Alternate Function mode ('10').
	GPIOA->AFR[0]  &= ~(0xF<<(4*TX_PIN) | 0xF<<(4*RX_PIN));	// Clear alternate function selection bits for PA2 and PA3.
	GPIOA->AFR[0]  |=   7<<(4*TX_PIN) | 7<<(4*RX_PIN); // Set alternate function 7 (USART2) for PA2 and PA3.
}

void configure_GPIO_pin(void){

	USART_PinInit();
	GameTimer_PinInit();
	configure_echo_pin();
	configure_trigger_pin();

}


