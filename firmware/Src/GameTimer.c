
#include "GameTimer.h"
#include "stm32l476xx.h"
#include "GameFrame.h"
#include "sensor.h"
///sensor .h only used for CNT_PSC at the moment

#define PA3 3

void GameTimer_Enable(){
	TIM5->CR1 |= TIM_CR1_CEN;
}


void GameTimer_Disable() {
	TIM5->CR1 &= ~TIM_CR1_CEN;
}

void GameTimer_TimInit() {

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;

	//10us tick, 33.32ms period ~30fps
	TIM5->PSC = CNT_PSC -1;
	TIM5->ARR = 3332;
	//was 3k ARR

	TIM5->CCMR2 &= ~TIM_CCMR2_CC4S_Msk;
	TIM5->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
	TIM5->CCMR2 |= 0b011 << TIM_CCMR2_OC4M_Pos;
	TIM5->CCR4 = TIM5->ARR/2;

	TIM5->CCER |= TIM_CCER_CC4E;


	TIM5->DIER |= TIM_DIER_UIE;

	NVIC_SetPriority(TIM5_IRQn, 4);
	NVIC_EnableIRQ(TIM5_IRQn);

	GameTimer_Enable();

}




void GameTimer_Init() {

	// had pin_init, keep function wrapper in case needed
	GameTimer_TimInit();

}

void TIM5_IRQHandler(void) {

	if ((TIM5->SR & TIM_SR_UIF) !=0) {
		TIM5->SR &= ~TIM_SR_UIF;
		//GPIOA->ODR ^= GPIO_ODR_OD5;
		buildFrame();

	}


}
