#include <sensor.h>
#include "stm32l476xx.h"
#include "GameFrame.h"

uint32_t ticks = 0;
uint16_t previous_CCR = 0;
uint16_t current_CCR = 0;
uint32_t overflow = 0;
uint8_t pulse_state = 1;
uint16_t timeCapture = 0;
volatile char input = 0;

void TIM4_CH1_Init(void) {

	//1. Enable the clock to TIM4 by configuring RCC_APB1ENR1 register
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

	//2. Configure TIM4 counting mode to upcounting (TIMx_CR1)
	TIM4->CR1 &= ~TIM_CR1_DIR;

	//3. Configure TIM4 Prescalar (TIMx_PSC) and ARR (TIMx_ARR) to generate a desired counter period
	TIM4->PSC = CNT_PSC - 1; // Set up an appropriate prescaler to slow down the clock or timer counter
	TIM4->ARR = 999;

	//4. Configure TIMx_CCMR1 CC1S[1:0] bits for TIM4 Channel 1
	//   to set the Capture/Compare channel direction as input,
	//   and input channel (CC1) is mapped on Timer Input 1 (TI1)
	//   00 = output
	//   01 = input, CC1 is mapped on timer Input 1 (TI1)
	//   10 = input, CC1 is mapped on timer Input 2 (TI2)
	//   11 = input, CC1 is mapped on slave timer
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;

	// 5. Disable digital filtering by configuring TIMx_CCMR1 IC1F[3:0] bits
	//    because we want to capture every event
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1F;

	// 6. Program the input prescaler by configuring TIMx_CCMR1 IC1PSC[1:0] bits
	// To capture each valid transition, set the input prescaler to zero;
	TIM4->CCMR1 &= ~(TIM_CCMR1_IC1PSC); // Clear filtering because we need to capture every event

	// 7. Select the active edge(s) for trigger of input capture operaiton
	//    by configuring TIMx_CCER CC1NP and CC1P bits
	// CC1NP=0, CC1P=0: rising edge,
	// CC1NP=0, CC1P=1: falling edge,
	// CC1NP=1, CC1P=0: reserved,
	// CC1NP=1, CC1P=1: both edges
	TIM4->CCER |= (TIM_CCER_CC1NP | TIM_CCER_CC1P);	// Both rising and falling edges.

	// 8. Enable Input Capture mode by setting TIMx_CCER CC1E bit
	TIM4->CCER |= TIM_CCER_CC1E;

	// 9. Enable related interrupts by
	//		i. configuring TIMx_DIER register
	TIM4->DIER |= TIM_DIER_CC1IE;// Enable Capture/Compare interrupts for channel 1
	TIM4->DIER |= TIM_DIER_UIE;			// Enable update interrupts

	//    ii. enable TIM4 interrupt source in NVIC
	NVIC_EnableIRQ(TIM4_IRQn);      // Enable TIM4 interrupt in NVIC

	//10. Set TIMx_CR1 CEN bit to enable the TIM4 counter
	TIM4->CR1 |= TIM_CR1_CEN;				// Enable the counter
}

void TIM2_CH3_Init() {
	//1. Enable the clock to TIM2 by configuring RCC_APB1ENR1 register
	RCC->APB1ENR1 |= (0x1UL << 0);			//page 256

	//2. Configure TIM2 counting mode to upcounting (TIMx_CR1)
	TIM2->CR1 &= ~(0x1UL << 4);

	//3. Configure TIM2 Prescaler (TIMx_PSC) and ARR (TIMx_ARR) to generate a desired counter period
	//TIM2->PSC &= ~(0xFFFF);

	//scales down to 100khz = 10us
	//overflow at 10us*10k = 100ms
	TIM2->PSC = CNT_PSC-1;
	TIM2->ARR = 999;

	//4. Configure TIMx_CCMR2 CC2S[1:0] bits to output mode for TIM2 Channel 3
	TIM2->CCMR2 &= ~(0x3UL << 0);			//page 1069

	//5. Configure TIMx_CCMR2 OC3M[3:0] bits to PWM mode 1 ('0110') for TIM2 Channel 3
	TIM2->CCMR2 &= ~(0x1UL << 16);

	TIM2->CCMR2 &= ~(0x7UL << 4);
	TIM2->CCMR2 |= (0b110 << 4);		//page 1

	//6. Set TIMx_CCER CC3E bit to enable output signal on Channel 3
	TIM2->CCER |= (0x1UL << 8);			//page 1075

	//7. Set TIMx_CR1 CEN bit to enable the TIM2 counter
	TIM2->CR1 |= (0x1UL << 0);

	TIM2->CCR3 = 20U;

}

void TIM4_IRQHandler(void) {

	// Check if counter overflow interrupt is triggered
	if ((TIM4->SR & TIM_SR_UIF) == TIM_SR_UIF) {

		overflow++;
		// Clear the UIF Flag
		TIM4->SR &= ~TIM_SR_UIF;
	}

	// Check if input capture interrupt is triggered
	else if ((TIM4->SR & TIM_SR_CC1IF) == TIM_SR_CC1IF) {
		// Reading CCR1 clears CC1IF

		timeCapture = TIM4->CCR1;

		//tracking pulse state in software
		//rising edge event
		if (pulse_state == 1) {
			previous_CCR = timeCapture;
			overflow = 0;
			pulse_state = 0;
		}

		//falling edge event
		else {
			current_CCR = timeCapture;
			ticks = ((current_CCR - previous_CCR) + (overflow * 10000));
			pulse_state = 1;
		}
	}
}


void activate_measuring(){
	TIM4_CH1_Init();
	TIM2_CH3_Init();


}

void deactivate_measuring(){
	TIM4->CR1 &= ~TIM_CR1_CEN;				// Enable the counter
	TIM2->CR1 &= ~(0x1UL << 0);


}

void init_sensor(){
	//had other function calls, keep in case needed
	activate_measuring();
}

int get_ticks(){
	return ticks;
}
