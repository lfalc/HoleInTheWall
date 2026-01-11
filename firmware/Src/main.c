#include "stm32l476xx.h"
#include "USART2.h"
#include "sensor.h"
#include "GPIO.h"
#include "GameFrame.h"
#include "DMA1.h"
#include "GameTimer.h"

void FPU_Enable(){
	SCB->CPACR |= ((3UL << 20U) | (3UL << 22U));
}

int main(void){

	FPU_Enable();
	configure_GPIO_pin();
	USART2_Init();
	DMA1_ch7_cfg();
	init_sensor();

	Game_Init();
	GameTimer_Init();

while (1) {}

}


// TODO
// PC change color on hit
// wall change color on hit or pass
// change addwall logic to only add new walls at the top
// implement digital filter on sensor
