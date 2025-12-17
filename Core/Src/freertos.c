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
uint8_t txbuffer[3] = {0b10010,0b10,0b10};
uint8_t RED[3] = {0b1001,0b1001,0b1001};
uint8_t YELLOW[3] = {0b10010,0b10,0b10};
uint8_t GREEN[3] = {0b100100,0b10100,0b10100};
//Note: MSB first bit on bus. [0] = U3
uint32_t TL1_Red 	= 0b0001; 			//U1 Q0
uint32_t TL1_Yellow = 0b0010; 			//U1 Q1
uint32_t TL1_Green 	= 0b0100; 			//U1 Q2
uint32_t PL1_Red 	= 0b1000; 			//U1 Q3
uint32_t PL1_Green 	= 0b0001 <<  4; 	//U1 Q4
uint32_t PL1_Blue 	= 0b0010 <<  4; 	//U1 Q5

uint32_t TL2_Red 	= 0b0001 <<  8; 	//U2 Q0
uint32_t TL2_Yellow = 0b0010 <<  8; 	//U2 Q1
uint32_t TL2_Green 	= 0b0100 <<  8; 	//U2 Q2
uint32_t PL2_Red 	= 0b1000 <<  8; 	//U2 Q3
uint32_t PL2_Green 	= 0b0001 << 12; 	//U2 Q4
uint32_t PL2_Blue 	= 0b0010 << 12; 	//U2 Q5

uint32_t TL3_Red 	= 0b0001 << 16; 	//U3 Q0
uint32_t TL3_Yellow = 0b0010 << 16; 	//U3 Q1
uint32_t TL3_Green 	= 0b0100 << 16; 	//U3 Q2
uint32_t TL4_Red 	= 0b1000 << 16; 	//U3 Q3
uint32_t TL4_Yellow = 0b0001 << 20; 	//U3 Q4
uint32_t TL4_Green 	= 0b0010 << 20; 	//U3 Q5


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
void wait_cycles( uint32_t n );
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
  //HAL_GPIO_WritePin(USR_LED2_GPIO_Port,USR_LED2_Pin,GPIO_PIN_SET);
  //HAL_GPIO_WritePin(S595_Enable_GPIO_Port,S595_Enable_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S595_Reset_GPIO_Port,S595_Reset_Pin,GPIO_PIN_SET);
  int8_t state = 4;
  /* Infinite loop */
  for(;;)
  {
  	uint32_t led_buffer = 	0x000000FF;/* |
  							TL4_Yellow |
							TL3_Yellow |
							TL1_Yellow; */
    //HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
    HAL_GPIO_TogglePin(USR_LED1_GPIO_Port,USR_LED1_Pin);
    if (HAL_SPI_Transmit(&hspi3, (uint8_t *) &led_buffer +3, 3, 1000) == HAL_ERROR){
      HAL_GPIO_TogglePin(USR_LED2_GPIO_Port,USR_LED2_Pin);
      
    } ; //Sending in Blocking mode
    
    HAL_GPIO_WritePin(S595_STCP_GPIO_Port,S595_STCP_Pin,GPIO_PIN_SET);
    osDelay(50);
    HAL_GPIO_WritePin(S595_STCP_GPIO_Port,S595_STCP_Pin,GPIO_PIN_RESET);
    HAL_Delay(1000);
    switch (state)
    {
    case 0:
      txbuffer[0] = RED[0];
      txbuffer[1] = RED[1];
      txbuffer[2] = RED[2];
      break;
    case 1:
      /* code */
      txbuffer[0] = YELLOW[0];
      txbuffer[1] = YELLOW[1];
      txbuffer[2] = YELLOW[2];
      break;
    case 2:
      txbuffer[0] = GREEN[0];
      txbuffer[1] = GREEN[1];
      txbuffer[2] = GREEN[2];
      /* code */
      break;
    case 3:
    	uint32_t leds = PL1_Red |
						PL2_Red |
						PL1_Green |
						PL2_Green  |
						PL1_Blue |
						PL2_Blue |
						TL3_Yellow |
						TL1_Yellow;
												;
		txbuffer[0] = leds >> 16;
    	txbuffer[1] = leds >> 8;
    	txbuffer[2] = leds ;//>> 0;
    	break;

    default:
      //state = 0;
      break;
    }
    //state = (state + 1) % 4;
  }
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
  for(;;)
  {
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
  for(;;)
  {
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
  for(;;)
  {
    TickType_t xLastWakeTime;
    const TickType_t xPeriod = pdMS_TO_TICKS(200) ; // ms to ticks
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    /* Infinite loop */
    for(;;)
      {
        vTaskDelayUntil( &xLastWakeTime, xPeriod );
        wait_cycles(10); //add a breakpoint in this line
      }
  }
  /* USER CODE END Trigg */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void wait_cycles( uint32_t n ) {
  uint32_t l = n/3; //cycles per loop is 3
  asm volatile( "0:" "SUBS %[count], 1;" "BNE 0b;" :[count]"+r"(l) );
}
/* USER CODE END Application */

