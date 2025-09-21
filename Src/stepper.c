#include "stepper.h"
#include "delay.h"   // uses delay_us()

static inline void write_en(const StepperHandle *h, GPIO_PinState state)
{
    if (h->cfg.en_pin == 0) return; // no EN line wired
    HAL_GPIO_WritePin(h->cfg.port, h->cfg.en_pin, state);
}

void Stepper_Init(StepperHandle *h, const StepperConfig *cfg, uint8_t initial_dir)
{
    h->cfg = *cfg;
    h->dir = (initial_dir != 0);

    /* Caller is responsible for enabling GPIO clock and configuring pins
       (or do it here if you want the driver to own it).
       We’ll just drive them to safe defaults. */
    HAL_GPIO_WritePin(h->cfg.port, h->cfg.step_pin | h->cfg.dir_pin, GPIO_PIN_RESET);

    /* Disable driver by default */
    if (h->cfg.en_pin) {
        GPIO_PinState idle = h->cfg.en_active_low ? GPIO_PIN_SET : GPIO_PIN_RESET;
        write_en(h, idle);
    }
}

void Stepper_Enable(StepperHandle *h)
{
    if (!h->cfg.en_pin) return;
    GPIO_PinState on = h->cfg.en_active_low ? GPIO_PIN_RESET : GPIO_PIN_SET;
    write_en(h, on);
}

void Stepper_Disable(StepperHandle *h)
{
    if (!h->cfg.en_pin) return;
    GPIO_PinState off = h->cfg.en_active_low ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_en(h, off);
}

void Stepper_SetDir(StepperHandle *h, uint8_t dir)
{
    h->dir = (dir != 0);
    HAL_GPIO_WritePin(h->cfg.port, h->cfg.dir_pin, h->dir ? GPIO_PIN_SET : GPIO_PIN_RESET);
    /* DIR setup time is tiny on TMC2209; our delays far exceed it. */
}

void Stepper_StepN(StepperHandle *h, uint16_t steps)
{
    /* Ensure direction pin is already set */
    for (uint16_t i = 0; i < steps; i++)
    {
        HAL_GPIO_WritePin(h->cfg.port, h->cfg.step_pin, GPIO_PIN_SET);
        delay_us(h->cfg.step_pulse_us);
        HAL_GPIO_WritePin(h->cfg.port, h->cfg.step_pin, GPIO_PIN_RESET);
        delay_us(h->cfg.step_space_us);
    }
}

uint16_t Stepper_AngleToSteps(const StepperHandle *h, float angle_deg)
{
    float steps_f = angle_deg * (h->cfg.full_steps_per_rev * h->cfg.microsteps) / 360.0f;
    uint32_t total_steps = (uint32_t)(steps_f + 0.5f);
    if (total_steps > UINT16_MAX) total_steps = UINT16_MAX;
    return (uint16_t)total_steps;
}

void Stepper_RotateAngle(StepperHandle *h, float angle_deg, uint8_t dir)
{
    Stepper_SetDir(h, dir);
    Stepper_Enable(h);
    Stepper_StepN(h, Stepper_AngleToSteps(h, angle_deg));
    Stepper_Disable(h);
}
