#ifndef STEPPER_H
#define STEPPER_H

#include "stm32f7xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      step_pin;
    uint16_t      dir_pin;
    uint16_t      en_pin;           /* optional; set to 0 if unused */
    uint8_t       en_active_low;    /* 1 if EN low = enable (TMC2209), else 0 */
    uint16_t      full_steps_per_rev; /* e.g., 200 */
    uint16_t      microsteps;         /* e.g., 16 when MS1/MS2 high */
    uint32_t      step_pulse_us;      /* high time */
    uint32_t      step_space_us;      /* low time */
} StepperConfig;

typedef struct {
    StepperConfig cfg;
    uint8_t       dir;               /* 1 CW, 0 CCW */
} StepperHandle;

/* Basic driver API */
void Stepper_Init(StepperHandle *h, const StepperConfig *cfg, uint8_t initial_dir);
void Stepper_Enable(StepperHandle *h);
void Stepper_Disable(StepperHandle *h);
void Stepper_SetDir(StepperHandle *h, uint8_t dir);

/* Motion primitives */
void     Stepper_StepN(StepperHandle *h, uint16_t steps);
uint16_t Stepper_AngleToSteps(const StepperHandle *h, float angle_deg);
void     Stepper_RotateAngle(StepperHandle *h, float angle_deg, uint8_t dir);

#ifdef __cplusplus
}
#endif

#endif /* STEPPER_H */
