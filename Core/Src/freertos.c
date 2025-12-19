/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "task3.h" // For task3_input_update
#include "task5.h" // For task5_poller
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
osSemaphoreId_t buttonSemaphore;

/* Definitions for Blink1Task */
osThreadId_t Blink1TaskHandle;
const osThreadAttr_t Blink1Task_attributes = {
  .name = "Blink1Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
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
  .priority = (osPriority_t) osPriorityHigh,
};

/* Definitions for InputTimer */
osTimerId_t inputTimerHandle;
const osTimerAttr_t inputTimer_attributes = {
  .name = "InputTimer"
};

/* Definitions for CommandTask */
osThreadId_t CommandTaskHandle;
const osThreadAttr_t CommandTask_attributes = {
  .name = "CommandTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Definitions for TrafficTask */
osThreadId_t TrafficTaskHandle;
const osThreadAttr_t TrafficTask_attributes = {
  .name = "TrafficTask",
  .stack_size = 256 * 4, // Larger stack for FSM?
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definition for InputTimer */

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Blink1(void *argument);
void Blink2(void *argument);
void Trigg(void *argument);
void InputTimerCallback(void *argument);
void Trigg(void *argument);
void InputTimerCallback(void *argument);
void CommandTask(void *argument);
void TrafficTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* Create binary semaphore, initial count 1 */
  buttonSemaphore = osSemaphoreNew(1, 1, NULL);
  /* USER CODE END Init */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* creation of Blink1Task */
  Blink1TaskHandle = osThreadNew(Blink1, NULL, &Blink1Task_attributes);

  /* creation of Blink2Task */
  Blink2TaskHandle = osThreadNew(Blink2, NULL, &Blink2Task_attributes);

  /* creation of TriggTask */
  TriggTaskHandle = osThreadNew(Trigg, NULL, &TriggTask_attributes);

  /* creation of CommandTask */
  CommandTaskHandle = osThreadNew(CommandTask, NULL, &CommandTask_attributes);

  /* creation of TrafficTask */
  TrafficTaskHandle = osThreadNew(TrafficTask, NULL, &TrafficTask_attributes);

  /* Create InputTimer (10ms periodic) */
  inputTimerHandle = osTimerNew(InputTimerCallback, osTimerPeriodic, NULL, &inputTimer_attributes);
  
  /* Start InputTimer */
  osTimerStart(inputTimerHandle, 10); // 10 ticks = 10ms (assuming 1kHz tick)

  /* USER CODE END RTOS_THREADS */

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
  LED_Driver_Init(); // Initialize hardware drivers (shift regs etc.)
  
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Blink1 */
/** 
* @brief Function implementing the Blink1Task thread.
* Controls LED based on Semaphore state.
*/
/* USER CODE END Header_Blink1 */
void Blink1(void *argument)
{
  /* USER CODE BEGIN Blink1 */
  TickType_t xLastWakeTime;
  const TickType_t xPeriod = pdMS_TO_TICKS(100); // 100ms period
  xLastWakeTime = xTaskGetTickCount();

  for(;;)
  {
    /* Check semaphore without blocking indefinitely (just peek) */
    /* If count > 0, button is RELEASED (Enable Blink) */
    if (osSemaphoreGetCount(buttonSemaphore) > 0) {
        // Toggle LD2 (Green LED)
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    } else {
        // Optional: Ensure LED is OFF if blocked? 
        // Strict toggle logic just stops toggling.
    }
    
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
  /* USER CODE END Blink1 */
}

/* USER CODE BEGIN Header_Blink2 */
/**
* @brief Function implementing the Blink2Task thread.
* Independent heartbeat.
*/
/* USER CODE END Header_Blink2 */
void Blink2(void *argument)
{
  /* USER CODE BEGIN Blink2 */
    for(;;)
    {
        // Toggle USR_LED1 (PB2) if available
        HAL_GPIO_TogglePin(USR_LED1_GPIO_Port, USR_LED1_Pin);
        osDelay(500); 
    }
  /* USER CODE END Blink2 */
}

/* USER CODE BEGIN Header_Trigg */
/**
* @brief Function implementing the TriggTask thread (User Button).
* Controls the semaphore.
*/
/* USER CODE END Header_Trigg */
void Trigg(void *argument)
{
  /* USER CODE BEGIN Trigg */
  for(;;)
  {
    // B1_Pin is User Button (PC13). Low = Pressed.
    if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
    {
        /* Button Pressed: Acquire sem (Block Blink1) */
        osSemaphoreAcquire(buttonSemaphore, 0);
    }
    else
    {
        /* Button Released: Release sem (Enable Blink1) */
        osSemaphoreRelease(buttonSemaphore);
    }
    osDelay(20); // Debounce / Polling rate
  }
  /* USER CODE END Trigg */
}

/* USER CODE BEGIN Header_InputTimerCallback */
/**
* @brief Function implementing the InputTimer callback.
* Called every 10ms to sample inputs.
*/
/* USER CODE END Header_InputTimerCallback */
void InputTimerCallback(void *argument)
{
    // Update Traffic Light Inputs
    task3_input_update();
}

/* USER CODE BEGIN Header_CommandTask */
/**
* @brief Function implementing the CommandTask thread.
* Polls UART for configuration commands.
*/
/* USER CODE END Header_CommandTask */
void CommandTask(void *argument)
{
  /* USER CODE BEGIN CommandTask */
  for(;;)
  {
      task5_poller(); 
      osDelay(50); // Yield to other tasks
  }
  /* USER CODE END CommandTask */
}

/* USER CODE BEGIN Header_TrafficTask */
/**
* @brief Function implementing the TrafficTask thread.
* Runs the Traffic Light State Machine.
*/
/* USER CODE END Header_TrafficTask */
void TrafficTask(void *argument)
{
  /* USER CODE BEGIN TrafficTask */
  for(;;)
  {
      task3(); // Run one cycle of FSM (contains delays)
      // Note: task3() internally uses osDelay now, so it yields.
      // If task3() returns immediately (e.g. no delay in some path), add safe-guard:
      osDelay(10); 
  }
  /* USER CODE END TrafficTask */
}
