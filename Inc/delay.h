#ifndef DELAY_H
#define DELAY_H

#include "stm32f7xx_hal.h"
#include <stdint.h>

/* Initialize delay module with a HAL timer already created by CubeMX.
   - htim       : pointer to a *base* timer handle (e.g., &htim1)
   - start_timer: if true, calls HAL_TIM_Base_Start(htim).
   Works with any prescaler value from the IOC. */
void Delay_Init(TIM_HandleTypeDef *htim, uint8_t start_timer);

/* Busy-wait delays built on the timer passed to Delay_Init(). */
void delay_us(uint32_t us);

/* Convenience wrapper around HAL_Delay (ms). */
static inline void delay_ms(uint32_t ms) { HAL_Delay(ms); }

#endif /* DELAY_H */
