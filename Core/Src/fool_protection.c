#include "fool_protection.h"

volatile uint8_t request_in_flight = 0;
uint32_t request_started_at = 0;

void button_irq_enable(void)
{
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
  NVIC_ClearPendingIRQ(EXTI0_IRQn);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void button_irq_disable(void)
{
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
}

void weather_request_begin(void)
{
  request_in_flight = 1;
  request_started_at = HAL_GetTick();
  button_irq_disable();
}

void weather_request_end(void)
{
  request_in_flight = 0;
  button_irq_enable();
}