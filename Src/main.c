#include "main.h"
#include "delay.h"
#include "stepper.h"

/* --- CubeMX-generated externs (keep your existing SystemClock_Config) --- */
TIM_HandleTypeDef htim1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
void Error_Handler(void);

/* --- Your wiring (BTT TMC2209 v1.3 on GPIOA 5/6/7) --- */
#define STEP_PORT   GPIOA
#define STEP_PIN    GPIO_PIN_5
#define DIR_PIN     GPIO_PIN_6
#define EN_PIN      GPIO_PIN_7

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM1_Init();

    /* Initialize delay module on TIM1 (honors TIM1 PSC from your IOC) */
    Delay_Init(&htim1, 1 /* start timer */);

    /* Create stepper instance */
    StepperHandle motor;
    StepperConfig cfg = {
        .port               = STEP_PORT,
        .step_pin           = STEP_PIN,
        .dir_pin            = DIR_PIN,
        .en_pin             = EN_PIN,
        .en_active_low      = 1,         // TMC2209 ENN: low = enable
        .full_steps_per_rev = 200,
        .microsteps         = 16,        // MS1/MS2 high
        .step_pulse_us      = 500,       // high time
        .step_space_us      = 500        // low time -> 1 kHz total
    };

    Stepper_Init(&motor, &cfg, /*initial_dir=*/1);

    while (1)
    {
        Stepper_RotateAngle(&motor, 30.0f, 1);
        delay_ms(1000);

        Stepper_RotateAngle(&motor, 45.0f, 1);
        delay_ms(1000);

        Stepper_RotateAngle(&motor, 90.0f, 1);
        delay_ms(1000);

        Stepper_RotateAngle(&motor, 180.0f, 1);
        delay_ms(1000);
    }
}

/* -------------------- CubeMX areas (unchanged logic) -------------------- */

static void MX_TIM1_Init(void)
{
    /* Keep your existing generated code or the version we used earlier.
       Key point: Delay_Init() will read PSC and compute µs correctly. */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __HAL_RCC_TIM1_CLK_ENABLE();

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 215; /* <- if this is what your IOC set; any value is OK */
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 0xFFFF;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK) Error_Handler();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) Error_Handler();

    sMasterConfig.MasterOutputTrigger  = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode      = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) Error_Handler();
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef io = {0};
    HAL_GPIO_WritePin(STEP_PORT, STEP_PIN | DIR_PIN | EN_PIN, GPIO_PIN_RESET);

    io.Pin   = STEP_PIN | DIR_PIN | EN_PIN;
    io.Mode  = GPIO_MODE_OUTPUT_PP;
    io.Pull  = GPIO_NOPULL;
    io.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STEP_PORT, &io);
}

/* Keep your SystemClock_Config() and Error_Handler() from before */


/* -------------------- Clock (HSI @ 16 MHz) -------------------- */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                                  RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) Error_Handler();
}

/* -------------------- Error handler -------------------- */

void Error_Handler(void)
{
    __disable_irq();
    while (1) { }
}
