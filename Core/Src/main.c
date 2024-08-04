/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ADP.h"
#include "OLED.h"
#include "data_proc.h"
#include <stdbool.h>
#include <stdio.h>
//#include "hdlc.h"
#include <math.h>
#include "queue.h"
#include "smoke_cls.h"
//extern int Simple_test(uint32_t a, uint32_t b, double c);

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
#define RXBUFFERSIZE 256
char RxBuffer[RXBUFFERSIZE];

#include <string.h>


#define AVER_CNT 4
#define RXBUFFERSIZ0E  256     //???????
#define RxPacketSIZE 4
#define Aver_time 20
#define Delay_time 2000
#define Refresh_Sens 0.01
char RxBuffer[RXBUFFERSIZE]; //????		//??????
uint8_t Uart1_Rx_Cnt = 0;    //??????
uint8_t Res = 0;
uint16_t USART1_RX_STA;

uint16_t last;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t LED_R = 0;
uint32_t LED_B = 0;
double LED_R_B = 0;

uint32_t delta_LED_R = 0;
uint32_t delta_LED_B = 0;
double R_B_ratio = 1;

uint8_t yes_cnt = 0;
uint8_t no_to_yes = 0;
uint8_t data_cnt = 0;
int a ;
int b ;


uint16_t cnt_test = 0,out = 0;

void Get_Aver(union FifoData* Aver_Data){

	struct CsPin ADPD_Gpio;

	ADPD_Gpio.gpio = GPIOB;
	ADPD_Gpio.gpioPinMask = GPIO_PIN_10;  //main

	union FifoData Fun_Data;
	uint8_t Fun_Cnt = 0;
	uint8_t Fun_Cnt_Real =0;
	Aver_Data ->bytes .SLOTA =0;
	Aver_Data ->bytes .SLOTB =0;	
	
	while(1){
	
		HAL_Delay (Aver_time );
		Fun_Data = readData(hspi1,ADPD_Gpio);

		if(Fun_Data .raw ){
		
			if(Fun_Data .bytes .SLOTB <32000){
		
				Fun_Cnt ++;

				Aver_Data ->bytes .SLOTA +=Fun_Data .bytes .SLOTA ;
				Aver_Data ->bytes .SLOTB +=Fun_Data .bytes .SLOTB ;
				Fun_Cnt_Real ++;
			}
			if(Fun_Cnt >=AVER_CNT ||(Fun_Data .bytes .SLOTB >=32000&&Fun_Cnt_Real !=0)){
			
				Aver_Data ->bytes .SLOTA /=Fun_Cnt_Real ;
				Aver_Data ->bytes .SLOTB /=Fun_Cnt_Real ;
				break;
			}
		}
			
	}

}

int max(int a, int b)
{
	if (a>b)
		return a;
	else return b;
}


int state = 1;
//'LED_R','LED_B','LED_R_B'
int smoke_cls(float inputs[3]) {
    float output[3];
    float input[3];
    memcpy(input, inputs, 3 * sizeof(float));
    int max_index = forward(&nn, input, output);
    // printf("Predicted class: %d\n", max_index);
    enqueue(max_index);
    // printf("count of 0: %d\n", countelement(0));
    // printf("count of 1: %d\n", countelement(1));
    // printf("count of 2: %d\n", countelement(2));
    int arr[3] = {countelement(0), countelement(1), countelement(2)};
    // printf("max index: %d\n",find_max_index_int(arr,3));
    if (state == 1) {
        if (find_max_index_int(arr, 3) == 0) {
            state = 0;
        } else if (find_max_index_int(arr, 3) == 2) {
            state = 2;
        }
    } else if (state == 0) {
        if (countelement(0) == 0)
            state = 1;
    } else if (state == 2) {
        if (countelement(2) == 0)
            state = 1;
    }
    dequeue();
    //printf("state: %d\n", state);
    return  state;
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    uint8_t RxData; // main
    struct CsPin ADPD_Gpio;

    union FifoData rawdata;

    ADPD_Gpio.gpio = GPIOB;
    ADPD_Gpio.gpioPinMask = GPIO_PIN_10; // main

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
    uint8_t cnt = 0;
    double aver_Red = 0;
    double aver_Blue = 0;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    smokeSensorADPD188Detect(hspi1, ADPD_Gpio);
    smokeSensorADPD188Init(hspi1, ADPD_Gpio);
    //	OLED_Init();
    HAL_UART_Receive_IT(&huart1, (uint8_t *)RxBuffer, 1);
    //
    //	OLED_ShowString(1, 1, "RxData:");

    //    test_hdlc();
    int t = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1) {
        // double a = sin(0.1*t);
        // double b = cos(0.1*t);
        
        // printf("hello world!\n");
        // HAL_GPIO_TogglePin(LED_ON_Board_GPIO_Port, LED_ON_Board_Pin);

			//Simple_test(1, 2, 3);
       HAL_Delay (Delay_time);//Æô¶¯ÑÓÊ±
	  
	  union FifoData Last_Data,New_Data,Base_Data;
		Get_Aver (&Last_Data );
		Base_Data .raw =0;
		
	  while(1){
	  
//		  Get_Aver (&New_Data );
//			if (!Base_Data .raw) Base_Data .raw = New_Data.raw;
//			
//		  if((New_Data .bytes .SLOTB <(1+Refresh_Sens )*Last_Data .bytes .SLOTB )&&
//			  (New_Data .bytes.SLOTB >(1-Refresh_Sens )*Last_Data .bytes .SLOTB) )
//				data_cnt ++;
//			else data_cnt = 0;
//			
//		  Last_Data =New_Data ;
//			
//			if (data_cnt >= 20){Base_Data =New_Data ;data_cnt =0;}
//		  
//			last = Base_Data .bytes .SLOTB ;
//		  
//			LED_B = New_Data .bytes.SLOTA;
//			LED_R = New_Data .bytes.SLOTB;
//			LED_R_B = LED_R /(LED_B +1e-7);
//			
////			delta_LED_R = (uint32_t)max((int)((int)LED_R -(int)Base_Data .bytes .SLOTB),0 );
////			delta_LED_B = (uint32_t)max((int)((int)LED_B -(int)Base_Data .bytes .SLOTA) ,0);
//			
//			a = LED_R -Base_Data .bytes .SLOTB;
//			b = LED_B -Base_Data .bytes .SLOTA;
//			
//			delta_LED_R =max(a,0);
//			delta_LED_B = max(b,0);
//			if (delta_LED_B <=5) R_B_ratio = 0;
//			else R_B_ratio = delta_LED_R/(double)(delta_LED_B+1e-6);
//			
//			//Simple_test(delta_LED_R,delta_LED_B,R_B_ratio);
//			float inputs[3] = {delta_LED_R,delta_LED_B,R_B_ratio};
//			int smoke = smoke_cls(inputs);
//			if (smoke == 0 )
//				Serial_SendYes();
//			else if(smoke == 2){
//				Serial_SendWater();
//			}
//			else if(smoke == 1){
//				Serial_SendNo();
//			}
//			
////		  if((LED_R >=2*Base_Data .bytes .SLOTB) &&(LED_R_B <3)&&(LED_R_B >1)){
////		  
////			  no_to_yes ++;
////			  if(no_to_yes >=3){
////				  
////				  yes_cnt =5;
////			  }
////		  }
////		  else {
////		  
////			  no_to_yes =0;
////			  if (yes_cnt >0)
////				  yes_cnt --; 
////		  }
////			if (yes_cnt)   Serial_SendYes ();
////			else Serial_SendNo ();
//		  Serial_BlueSend (LED_B);
//		  Serial_RedSend (LED_R);
	  }
       

        //		Serial_SendByte(LED_B);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}

//uint8_t hdlc_buf[MAX_HDLC_LEN];
//uint8_t hdlc_buf_ptr;
//uint8_t hdlc_buf_sta;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
//        if (hdlc_buf_sta == 0) {
//            if (Res == HDLC_FLAG) {
//                hdlc_buf_ptr = 0;
//                hdlc_buf_sta = 1;
//                hdlc_buf[hdlc_buf_ptr++] = Res;
//            }
//        } else if (hdlc_buf_sta == 1) {
//            hdlc_buf[hdlc_buf_ptr++] = Res;
//            if (Res == HDLC_FLAG) {
//                hdlc_buf_ptr = 0;
//                hdlc_buf_sta = 0;
//                hdlc_decode_and_save(hdlc_buf, hdlc_buf_ptr);
//            }
//        }
        if (USART1_RX_STA == 0) {
            if (Res == 0x00 || Res == 0x01) {
                Uart1_Rx_Cnt = 0;
                USART1_RX_STA = 1;
                RxBuffer[Uart1_Rx_Cnt] = Res;
            }
        } else if (USART1_RX_STA == 1) {
            Uart1_Rx_Cnt++;
            RxBuffer[Uart1_Rx_Cnt] = Res;

            if (Uart1_Rx_Cnt >= RxPacketSIZE - 1) {
                USART1_RX_STA = 0;
                //                printf("Receive success Data:0x%02x 0x%02x
                //                0x%02x 0x%02x \r\n",
                //				RxBuffer[0],RxBuffer[1],RxBuffer[2],RxBuffer[3]);
                if ((RxBuffer[1] == 0x00) && (RxBuffer[2] == 0x00) &&
                    (RxBuffer[3] == 0x00)) {

                    if (RxBuffer[0] == 0x00) {

                        Serial_SendNo(); //ï¿½ï¿½ï¿½ï¿½
                                         //ï¿½Ø±Õ¾ï¿½ï¿½ï¿½
                    } else if (RxBuffer[0] == 0x01) {

                        Serial_SendYes(); //ï¿½ï¿½ï¿½ï¿½
                                          //ï¿½ò¿ª¾ï¿½ï¿½ï¿½
                    }
                }
            }
        }
    }
    HAL_UART_Receive_IT(&huart1, &Res, 1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1) {
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
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
