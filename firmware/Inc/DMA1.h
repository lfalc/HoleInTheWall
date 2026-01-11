#ifndef __STM32L476G_DMA1_H
#define __STM32L476G_DMA1_H

#include "stm32l476xx.h"

// Configuring DMA1 Channel 6 as the USART2 RX channel.
void DMA1_ch7_cfg(void);

void Enable_DMA(uint16_t lengthInBytes);

void Disable_DMA(void);


#endif /* __STM32L476G_DMA1_H */
