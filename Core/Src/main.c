/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "motor.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_Buffer_size 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern uint8_t uartRxReceived;
extern uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
extern uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];
uint16_t ADC_buffer[ADC_Buffer_size];
uint16_t current_value_buffer[100];
uint16_t ohmega_buffer[100];


uint8_t flag=0;
uint8_t ADC_flag =0;
uint8_t TIMER_4_flag=0;
uint16_t cnt_tim_3_value=32767;
float ohmega=0;

int raw_value=0;
float current_value;

//Variables pour la boucle de courant
float Ireq=0.0;
float Kp=0.00001;					//gain de proportionnel
float Ki=400;					//gain de l'itégrateur
float Te=1/16000.0;				//période de 16KHz
float CL_buffer[3]={0.0,0.0,50.0};	//le tableau contenant les valeurs presentes et précédantes des valeurs d'alpha et d'epsilon = [epsilon[n-1],epsilon[n],alpha2[n-1])
uint16_t alpha1=0;
float alpha2=50.0;
uint16_t alpha3=0;
float epsilon=0;
uint8_t CL_flag=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_Delay(1);
	shellInit();
	htim3.Instance->CNT=32767;

	//On effectue une calibration
	if (HAL_OK!= HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED))
		Error_Handler();

	//On start le timer qui débute les convertions par interruption
	if (HAL_OK!=HAL_TIM_Base_Start(&htim2))
		Error_Handler();
	//on start le DMA
	if (HAL_OK!= HAL_ADC_Start_DMA(&hadc1,ADC_buffer,ADC_Buffer_size))
		Error_Handler();
	//On start les timers 4
	if (HAL_OK!=HAL_TIM_Base_Start_IT(&htim4))
		Error_Handler();
	if (HAL_OK!=HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL))
			Error_Handler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		// SuperLoop inside the while(1), only flag changed from interrupt could launch functions
		if(uartRxReceived){
			if(shellGetChar()){
				shellExec();
				shellPrompt();
			}
			uartRxReceived = 0;
		}
		if (flag){
			motorPowerOn();
			flag=0;
		}

		if(ADC_flag==1){
			//Additonner les 10 valeurs acquises dans raw_value
			raw_value=0;
			for (int i=0;i<ADC_Buffer_size;i++){
				raw_value+=ADC_buffer[i];
			}

			//convertion en la valeur moyenne de courant réel
			current_value=((raw_value/ADC_Buffer_size)*3.3/4095.0-2.5)*12.0;

			//affichage dans l'uart de cette valeur

			//sprintf(current_value_buffer,"{ADC Value : %1.2f}\r\n",current_value);
			//HAL_UART_Transmit(&huart2, current_value_buffer,strlen((char*) current_value_buffer)*sizeof(char), HAL_MAX_DELAY);


			//ASSERVISSEMENT EN COURANT

			if (CL_flag){
			epsilon=Ireq-current_value;

			CL_buffer[1]=epsilon;		//écriture de la nouvelle valeur

			alpha1=(uint16_t) (Kp*epsilon);
			alpha2=CL_buffer[2]+(Ki*Te/2.0)*(CL_buffer[1]+CL_buffer[0]);
			alpha2=verif_alpha_float(alpha2);		//vérifi que la valeur de alpha soit compris enre 0 et 100
			alpha3=alpha2+alpha1;
			alpha3=verif_alpha(alpha3);

			CL_buffer[0]=CL_buffer[1]; 	//epsilon[n-1]=epsilon[n]
			CL_buffer[2]=alpha2; 		//epsilon[n-1]=epsilon[n]
			motorSetAlpha(alpha3);
			}

			//réinitialisation du flag
			ADC_flag=0;
		}

		if (TIMER_4_flag){
			//valeur de vitesse en tour/min
			ohmega=(cnt_tim_3_value-32767.0)/(0.1*4096.0)*60;

			//ecrire dans l'UART la valeur retournée
			/**sprintf(ohmega_buffer,"{Vitesse : %1.2f}\r\n",ohmega);
			HAL_UART_Transmit(&huart2, ohmega_buffer,strlen((char*) ohmega_buffer)*sizeof(char), HAL_MAX_DELAY);**/
			TIMER_4_flag=0;
		}




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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//la fonction de callback qui met un flag a 1 pour dire qu'il y a eu un appuie sur le Blue Button
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	flag=1;
}

//une fois que mon buffer est plein, on appel cette fonction qui set le ADC_flag a 1
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	ADC_flag=1;
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
  if (htim->Instance == TIM4) {
     cnt_tim_3_value=htim3.Instance->CNT;
     htim3.Instance->CNT=32767;
     TIMER_4_flag=1;
   }

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
