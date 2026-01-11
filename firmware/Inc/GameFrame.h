
#ifndef __STM32L476G_GameFrame_H
#define __STM32L476G_GameFrame_H
#include "stm32l476xx.h"

#define wallWidth 80
#define terminalHeight 60
#define maxNumWalls 3
#define wallDistance 20
#define wallStartRow 2
#define minHoleSize 10
#define maxHoleSize 15
#define frameSize (maxNumWalls*(wallWidth+32))
#define MIN_TICK_BOUNDARY 15
#define MAX_TICK_BOUNDARY 185

void buildFrame();
void Game_Init();
extern char frameString[frameSize];
extern volatile uint8_t DMA_busy;


#endif
