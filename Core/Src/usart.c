/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
uint8_t Serial_RxData;
uint8_t Serial_RxFlag;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
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

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void Serial_SendByte(uint8_t Byte) {
    HAL_UART_Transmit(&huart1, &Byte, 1, 0xffff);
}


void Serial_SendArray(uint8_t *Array, uint16_t Length) {
    uint16_t i;
    for (i = 0; i < Length; i++) {
        Serial_SendByte(Array[i]);
    }
}

void Serial_SendString(char *String) {
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++) {
        Serial_SendByte(String[i]);
    }
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y) {
    uint32_t Result = 1;
    while (Y--) {
        Result *= X;
    }
    return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length) {
    uint8_t i;
    for (i = 0; i < Length; i++) {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

void Serial_Printf(char *format, ...) {
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void) {
    if (Serial_RxFlag == 1) {
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}

uint8_t Serial_GetRxData(void) { return Serial_RxData; }

void Serial_CallBack(uint8_t Byte) {
    Serial_RxData = Byte;
    Serial_RxFlag = 1;
}

/*0xff 0xff [4*data] 0xfe*/
int Serial_SendPack(int value) {
    uint8_t *pack = (uint8_t *)&value;
    for (int i = 0; i < 7; i++) {
        if (i == 0 || i == 1) {
            if (pack[i] != 0xff) {
                return -1;
            }
        } else if (i == 6) {
            if (pack[i] != 0xfe) {
                return -1;
            }
        } else {
            Serial_SendByte(pack[i]);
        }
    }
    return 0;
}

int Serial_RecPack() {
    uint8_t pack[7];
    int i = 0;
    int ret = 0;

    /*�?查格�?*/
    while (i < 7) {
        if (Serial_GetRxFlag()) {
            pack[i] = Serial_GetRxData();
            if (i == 0 || i == 1) {
                if (pack[i] != 0xff) {
                    i = 0;
                }
            } else if (i == 6) {
                if (pack[i] != 0xfe) {
                    i = 0;
                }
            } else {
                i++;
            }
        }
    }
    ret = *(int *)&pack[2];
    return ret;
}

void Serial_BlueSend(uint16_t Data) {

    char Tx_Arry[4];
    Tx_Arry[0] = Data % 256;
    Tx_Arry[1] = (Data >> 8) % 256;
    Tx_Arry[2] = (Data >> 16) % 256;
    Tx_Arry[3] = 0x03;
    printf("%c", Tx_Arry[0]);
    printf("%c", Tx_Arry[1]);
    printf("%c", Tx_Arry[2]);
    printf("%c", Tx_Arry[3]);
}

void Serial_RedSend(uint16_t Data) {

    char Tx_Arry[4];
    Tx_Arry[0] = Data % 256;
    Tx_Arry[1] = (Data >> 8) % 256;
    Tx_Arry[2] = (Data >> 16) % 256;
    Tx_Arry[3] = 0x02;
    printf("%c", Tx_Arry[0]);
    printf("%c", Tx_Arry[1]);
    printf("%c", Tx_Arry[2]);
    printf("%c", Tx_Arry[3]);
}

void Serial_SendYes(void) {

    char Tx_Arry[4] = {0x01, 0x00, 0x00, 0x01};
    printf("%c", Tx_Arry[0]);
    printf("%c", Tx_Arry[1]);
    printf("%c", Tx_Arry[2]);
    printf("%c", Tx_Arry[3]);
}

void Serial_SendNo(void) {

    char Tx_Arry[4] = {0x00, 0x00, 0x00, 0x01};
    printf("%c", Tx_Arry[0]);
    printf("%c", Tx_Arry[1]);
    printf("%c", Tx_Arry[2]);
    printf("%c", Tx_Arry[3]);
}

//void Serial_SendWater(void) {

//    char Tx_Arry[4] = {0x02, 0x00, 0x00, 0x01};
//    printf("%c", Tx_Arry[0]);
//    printf("%c", Tx_Arry[1]);
//    printf("%c", Tx_Arry[2]);
//    printf("%c", Tx_Arry[3]);
//}







/* USER CODE END 1 */
