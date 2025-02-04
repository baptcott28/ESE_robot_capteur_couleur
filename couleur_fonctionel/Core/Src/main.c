/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "color_sensor.h"
#include <stdio.h>
#include <stdlib.h>
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

/* USER CODE BEGIN PV */
h_color_sensor_t color_sensor1;
h_green_transformation_t color_sensor1_green_transformation;
h_red_transformation_t color_sensor1_transformation;

SemaphoreHandle_t colorMeasureSemaphore;
TaskHandle_t h_colorMeasureTask;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==button_Pin){

		/*//interruption sans RTOS
		printf("----- appui bouton -----\r\n");
		colorSetPhotodiodeType(&color_sensor1, GREEN);
		colorEnable(&color_sensor1);*/

		/*BaseType_t higher_priority_task_woken = pdFALSE;
		colorSetPhotodiodeType(&color_sensor1, GREEN);
		printf("semaphore colormeasureSemaphore donné\r\n");
		xSemaphoreGiveFromISR(colorMeasureSemaphore,&higher_priority_task_woken);*/
		//colorEnable(&color_sensor1);
	}
}

TaskHandle_t h_bidonTask;
TaskHandle_t h_processWU;
SemaphoreHandle_t sem_bidon;

void bidonTask(void * pvParameters){
	for(;;){
		xSemaphoreTake(sem_bidon,portMAX_DELAY);
	}
}

void task_process_Wake_Up(void * pvParameters){
	sem_bidon=xSemaphoreCreateBinary();
	uint32_t counter=0;
	for(;;){
		/*if((counter%100)==0){
			printf("\ncounter=0mod(1000)\r\n");
			xSemaphoreGive(sem_bidon);
			printf("semaphore sem_bidon donné\r\n");
		}*/
		if (counter ==300){
			xSemaphoreGive(colorMeasureSemaphore);
			counter=0;
		}
		counter++;
		vTaskDelay(10); //crée le delay de 1ms
	}
}

void creationTaskProcessing(void){
	// creation tache process

	if(pdTRUE==xTaskCreate(task_process_Wake_Up,"Taskprocess",COLOR_STACK_DEPTH,NULL,10,&h_processWU)){
		printf("process wake up created successfully\r\n");
	}
	else{
		printf("process wake up creation failed !\r\n");
	}

	// creation tache bidon
	if(pdTRUE==xTaskCreate(bidonTask,"bidonTask",COLOR_STACK_DEPTH,NULL,2,&h_bidonTask)){
		printf("bidonTask created successfully\r\n");
	}
	else{
		printf("bidonTask creation failed !\r\n");
	}

	// creation semaphore bidon
	sem_bidon=xSemaphoreCreateBinary();
	if(sem_bidon==NULL){
		printf("sem bidon creation failed ! \r\n");
	}
	printf("sem bidon created sucessfully\r\n");
}
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  //desactive the internal buffering for scanf, that is set by default in syscalls.c
  setvbuf(stdin, NULL, _IONBF, 0);

	printf("\n\n---- color sensor ----\r\n");


	// vrai code
  	//color_disable(&color_sensor1);

	/*// ----- test 1 : verification des valeur renvoyées -----

	color_sensor_init(&color_sensor1,RED,CENT_POUR_CENT,SENSOR_DISABLE);
	color_enable(&color_sensor1);*/

	//Code de test

	/*printf("---------red---------\r\n");
  color_sensor_init(&color_sensor1,RED,CENT_POUR_CENT,SENSOR_ENABLE);
  color_enable(&color_sensor1);
  HAL_Delay(20);
  color_disable(&color_sensor1);*/

	/*// ----- test 2 : appui sur bouton pour declencher une mesure -----

  printf("---- color sensor ----\r\n");

  color_disable(&color_sensor1);

  color_sensor_init(&color_sensor1,GREEN,CENT_POUR_CENT,SENSOR_DISABLE);
  printf("color sensor initialized\r\nwaiting for button press : \r\n");*/


  	/*// ----- test 3 : Fonction de calibration -----
  	printf("--- calibration lancée ---\r\n");
  	colorSensorInit(&color_sensor1, GREEN,CENT_POUR_CENT,SENSOR_DISABLE);
  	colorHandleCalibrationSensor(&color_sensor1);
  	printf("waiting for button press to measure : \r\n");*/

	// ----- test 4 : FreeRTOS -----

	colorSensorInit(&color_sensor1, GREEN,CENT_POUR_CENT,SENSOR_DISABLE);
	colorHandleCalibrationSensor(&color_sensor1);

	colorTaskCreation(&color_sensor1);
	creationTaskProcessing();


	printf("scheduler started\r\n");
  	vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//--_ Redirection du printf
int __io_putchar(int ch){
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}


// --- redirection du scanf
int __io_getchar(void){
  uint8_t ch = 0;

  /* Clear the Overrun flag just before receiving the first character */
  __HAL_UART_CLEAR_OREFLAG(&huart1);

  /* Wait for reception of a character on the USART RX line and echo this
   * character on console */
  HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
