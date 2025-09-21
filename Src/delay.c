#include "delay.h"

static TIM_HandleTypeDef *s_htim = NULL;
static uint32_t s_cnt_clk_hz = 0;   // timer counter clock after prescaler

/* APB x2 rule helper */
static uint32_t get_timer_clock_hz(TIM_TypeDef *instance)
{
    if (instance == TIM1 || instance == TIM8) {
        uint32_t pclk = HAL_RCC_GetPCLK2Freq();
        uint32_t ppre = RCC->CFGR & RCC_CFGR_PPRE2;
        return (ppre == RCC_CFGR_PPRE2_DIV1) ? pclk : (pclk * 2U);
    } else {
        uint32_t pclk = HAL_RCC_GetPCLK1Freq();
        uint32_t ppre = RCC->CFGR & RCC_CFGR_PPRE1;
        return (ppre == RCC_CFGR_PPRE1_DIV1) ? pclk : (pclk * 2U);
    }
}

void Delay_Init(TIM_HandleTypeDef *htim, uint8_t start_timer)
{
    s_htim = htim;

    /* Enable clk (usually Cube does this already) */
    if (s_htim->Instance == TIM1) __HAL_RCC_TIM1_CLK_ENABLE();
    if (s_htim->Instance == TIM2) __HAL_RCC_TIM2_CLK_ENABLE();
    if (s_htim->Instance == TIM3) __HAL_RCC_TIM3_CLK_ENABLE();
    if (s_htim->Instance == TIM4) __HAL_RCC_TIM4_CLK_ENABLE();
    if (s_htim->Instance == TIM5) __HAL_RCC_TIM5_CLK_ENABLE();
    if (s_htim->Instance == TIM6) __HAL_RCC_TIM6_CLK_ENABLE();
    if (s_htim->Instance == TIM7) __HAL_RCC_TIM7_CLK_ENABLE();
    if (s_htim->Instance == TIM8) __HAL_RCC_TIM8_CLK_ENABLE();

    /* counter clock = timer_clock / (PSC+1) */
    uint32_t tim_clk = get_timer_clock_hz(s_htim->Instance);
    s_cnt_clk_hz = tim_clk / (s_htim->Init.Prescaler + 1U);

    if (start_timer) {
        HAL_TIM_Base_Start(s_htim);
    }
}

void delay_us(uint32_t us)
{
    if (!s_htim || s_cnt_clk_hz == 0U) return;

    /* ticks = us * cnt_clk_hz / 1e6  (use 64-bit to avoid truncation to 0) */
    uint32_t ticks = (uint32_t)(((uint64_t)us * (uint64_t)s_cnt_clk_hz + 999999ULL) / 1000000ULL);
    if (us && ticks == 0U) ticks = 1U;  // guarantee at least 1 tick for nonzero delay

    __HAL_TIM_SET_COUNTER(s_htim, 0);
    while (__HAL_TIM_GET_COUNTER(s_htim) < ticks) { /* spin */ }
}
