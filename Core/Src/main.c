/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "icon.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "sim800.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

typedef enum {
  Lviv = 0,
  Kyiv = 1,
} City;

volatile City current_city = Lviv;
volatile uint8_t city_changed = 0;

int year, month, day_n, hour, minute;


float temperature = 0.0f;
float windspeed = 0.0f;
int weathercode = 0;
int is_day = 0;

uint16_t update_timer = 20000;

bool initialized = false;

volatile char rx_byte;
char rx_line[512];
uint16_t line_idx = 0;

uint32_t last_weather_update = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void updateDisplay();
void parseWeatherData(const char* json);

const char* getWeatherDesc(int code);
const unsigned char* getWeatherIcon(int code);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  sim800_init();
  HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_byte, 1);

  // temperature = 22.5f;
  // windspeed = 10.3f;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    if (!initialized) {
      ssd1306_SetCursor(0, 50);
      ssd1306_WriteString("Updating...", Font_7x10, White);
      ssd1306_UpdateScreen();
      sim800_get_weather("Lviv");
      initialized = true;
      last_weather_update = HAL_GetTick();
    }

    if (city_changed) 
    {
      city_changed = 0;
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 50);
      ssd1306_WriteString("Changing City...", Font_7x10, White);
      ssd1306_UpdateScreen();
      sim800_get_weather(current_city == Lviv ? "Lviv" : "Kyiv");
      last_weather_update = HAL_GetTick();
    }
    if (HAL_GetTick() - last_weather_update > update_timer) {
      sim800_get_weather(current_city == Lviv ? "Lviv" : "Kyiv");
      last_weather_update = HAL_GetTick();
    }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1) 
  {
    if (rx_byte == '\n' || rx_byte == '\r') {
      if (line_idx > 0) {
        rx_line[line_idx] = '\0';
        if (strstr(rx_line, "current_weather")) {
          parseWeatherData(rx_line);
          // city_changed = 1;
        }
                
        line_idx = 0;
      }
    } else {
      if (line_idx < 511) {
        rx_line[line_idx++] = rx_byte;
      } else {
        line_idx = 0;
      }
    }
    HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_byte, 1);
  }
}

void parseWeatherData(const char* json) {

  const char* cw = strstr(json, "\"current_weather\":{");
  if (!cw) {
    return;
  }
  
  char *p;
  p = strstr(cw, "\"time\":");
  if (p) {
    sscanf(p, "\"time\":\"%d-%d-%dT%d:%d\"", &year, &month, &day_n, &hour, &minute);
    hour+=2;
    if (hour >= 24) {
      hour -= 24;
      day_n += 1;
    }
  }
  p = strstr(cw, "\"temperature\":");
  if (p) {
    sscanf(p, "\"temperature\": %f", &temperature);
  }
  p = strstr(cw, "\"windspeed\":");
  if (p) {
    sscanf(p, "\"windspeed\": %f", &windspeed);
  }

  p = strstr(cw, "\"weathercode\":");
  if (p) {
    sscanf(p, "\"weathercode\": %d", &weathercode);
  }

  p = strstr(cw, "\"is_day\":");
  if (p) {
    sscanf(p, "\"is_day\": %d", &is_day);
  }

  updateDisplay();
  memset(rx_line, 0, sizeof(rx_line));
  line_idx = 0;
}

void updateDisplay() {
  char temp_str[35];


  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("City:", Font_7x10, White);
  
  ssd1306_SetCursor(35, 0);
  ssd1306_WriteString(current_city == Lviv ? "LVIV" : "KYIV", Font_7x10, White);

  ssd1306_DrawBitmap(104, 0, is_day ? day : night, 24, 24, White);
  ssd1306_DrawBitmap(104, 30, (uint8_t*)getWeatherIcon(weathercode), 24, 24, White);

  sprintf(temp_str, "%02d:%02d", hour, minute);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(temp_str, Font_11x18, White);
  ssd1306_SetCursor(60, 14);
  sprintf(temp_str, "%02d-%02d", day_n, month);
  ssd1306_WriteString(temp_str, Font_7x10, White);



  int t_int = (int)temperature;
  int t_dec = (int)((temperature - (float)t_int) * 10.0f);
  if (t_dec < 0) t_dec = -t_dec; 


  sprintf(temp_str, "Temp:%d.%d C", t_int, t_dec);
  ssd1306_SetCursor(0, 28);
  ssd1306_WriteString(temp_str, Font_7x10, White);

  int w_int = (int)windspeed;
  int w_dec = (int)((windspeed - (float)w_int) * 10.0f);
  if (w_dec < 0) w_dec = -w_dec;

  sprintf(temp_str, "Wind:%d.%d km/h", w_int, w_dec);
  ssd1306_SetCursor(0, 38);
  ssd1306_WriteString(temp_str, Font_7x10, White);

  ssd1306_SetCursor(0, 48);
  sprintf(temp_str, "%s", getWeatherDesc(weathercode));
  ssd1306_WriteString(temp_str, Font_7x10, White);

  ssd1306_UpdateScreen();
}

const char* getWeatherDesc(int code) {  
  if (code == 0) return "Clear";
  if (code >= 1 && code <= 3) return "Cloudy";
  if (code >= 45 && code <= 48) return "Foggy";
  if (code >= 51 && code <= 55) return "Drizzle";
  if (code >= 61 && code <= 65) return "Rainy";
  if (code >= 71 && code <= 77) return "Snowy";
  if (code >= 95) return "Thunderstorm";
  return "Unknown";
}

const unsigned char* getWeatherIcon(int code) {
  if (code == 0) return clearSky;
  if (code >= 1 && code <= 3) return cloud;
  if (code >= 45 && code <= 48) return fog;
  if (code >= 51 && code <= 55) return drizzle;
  if (code >= 61 && code <= 65) return rain;
  if (code >= 71 && code <= 77) return snow;
  if (code >= 95) return thunderstorm;
  return cloud;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_0)
  {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();

    if ((current_time - last_interrupt_time) > 200)
    {
      if (current_city == Lviv)
      {
        current_city = Kyiv;
      } else{
        current_city = Lviv;
      }
      city_changed = 1;

      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      last_interrupt_time = current_time;
    }
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
