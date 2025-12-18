/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led_driver.h"
#include "spi.h"

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
/* USER CODE BEGIN Variables */
uint8_t varBlink1 = 0;
uint8_t varBlink2 = 0;
// Variables removed as they are now handled by led_driver:
// txbuffer, RED, YELLOW, GREEN, and all bit definitions
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Blink1Task */
osThreadId_t Blink1TaskHandle;
const osThreadAttr_t Blink1Task_attributes = {
  .name = "Blink1Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Blink2Task */
osThreadId_t Blink2TaskHandle;
const osThreadAttr_t Blink2Task_attributes = {
  .name = "Blink2Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TriggTask */
osThreadId_t TriggTaskHandle;
const osThreadAttr_t TriggTask_attributes = {
  .name = "TriggTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void wait_cycles(uint32_t n);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Blink1(void *argument);
void Blink2(void *argument);
void Trigg(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Blink1Task */
  Blink1TaskHandle = osThreadNew(Blink1, NULL, &Blink1Task_attributes);

  /* creation of Blink2Task */
  Blink2TaskHandle = osThreadNew(Blink2, NULL, &Blink2Task_attributes);

  /* creation of TriggTask */
  TriggTaskHandle = osThreadNew(Trigg, NULL, &TriggTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  LED_Driver_Init();
  uint8_t state = 0;

  /* Infinite loop */
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Blink1 */
/**
 * @brief Function implementing the Blink1Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Blink1 */
void Blink1(void *argument)
{
  /* USER CODE BEGIN Blink1 */
  /* Infinite loop */
  for (;;) {
    osDelay(1);
  }
  /* USER CODE END Blink1 */
}

/* USER CODE BEGIN Header_Blink2 */
/**
 * @brief Function implementing the Blink2Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Blink2 */
void Blink2(void *argument)
{
  /* USER CODE BEGIN Blink2 */
  /* Infinite loop */
  for (;;) {
    varBlink2 = 1;
    wait_cycles(400000);
    varBlink2 = 0;
    vTaskDelay(20);
  }
  /* USER CODE END Blink2 */
}

/* USER CODE BEGIN Header_Trigg */
/**
 * @brief Function implementing the TriggTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Trigg */
void Trigg(void *argument)
{
  /* USER CODE BEGIN Trigg */
  /* Infinite loop */
  for (;;) {
    TickType_t xLastWakeTime;
    const TickType_t xPeriod = pdMS_TO_TICKS(200); // ms to ticks
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    /* Infinite loop */
    for (;;) {
      vTaskDelayUntil(&xLastWakeTime, xPeriod);
      wait_cycles(10); // add a breakpoint in this line
    }
  }
  /* USER CODE END Trigg */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void wait_cycles(uint32_t n) {
  uint32_t l = n / 3; // cycles per loop is 3
  asm volatile("0:"
               "SUBS %[count], 1;"
               "BNE 0b;"
               : [count] "+r"(l));
}
/* USER CODE END Application */

