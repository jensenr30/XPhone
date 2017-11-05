/**
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    06-May-2016
  * @brief   This example describes how to configure and use GPIOs through
  *          the STM32F4xx HAL API.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_nucleo_144.h"
//#include "../../../../app/key.h"
#include <stdlib.h>

#include "../../../../app/key.c"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static GPIO_InitTypeDef  GPIO_Struct;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);


// shift register output pins
// port C pins
#define SRO_DATA  GPIO_PIN_1
#define SRO_LATCH GPIO_PIN_4
#define SRO_CLOCK GPIO_PIN_5

// shift register input pins
// port F pins
#define SRI_DATA  GPIO_PIN_2
#define SRI_LATCH GPIO_PIN_1
#define SRI_CLOCK GPIO_PIN_0

// input pins
// port C pins
#define IN_1 GPIO_PIN_8
#define IN_2 GPIO_PIN_9
#define IN_3 GPIO_PIN_10
#define IN_4 GPIO_PIN_11
#define IN_5 GPIO_PIN_12
#define IN_6 GPIO_PIN_2
#define IN_7 GPIO_PIN_2
#define IN_8 GPIO_PIN_3

//GPIO_PinState input_state[INPUT_PINS];
//GPIO_TypeDef input_pin_port[INPUT_PINS];

#define input_pin_port_1 GPIOC
#define input_pin_port_2 GPIOC
#define input_pin_port_3 GPIOC
#define input_pin_port_4 GPIOC
#define input_pin_port_5 GPIOC
#define input_pin_port_6 GPIOD
#define input_pin_port_7 GPIOG
#define input_pin_port_8 GPIOG

#define KEYS 8
#define KEY_BYTE_SIZE 1
#define KEY_COOLDOWN 10

uint8_t keyInput[KEY_BYTE_SIZE];
int8_t keyInputCoolDown[KEYS];
uint8_t keyOutput[KEY_BYTE_SIZE];

#define SONG_LENGTH 1000
int currentTime = 0;
int totalNotes = 0;

#define size 14
int i, j;
int song[size] = {
		  0,1,2,3,4,5,6,7,6,5,4,3,2,1
};


void clock_out(GPIO_TypeDef* GPIOx, uint16_t clockPin, uint16_t dataPin, uint16_t latchPin, uint8_t outputSize, uint8_t *data) {
	uint8_t i, j;

	for(i = 0; i < outputSize; i++) {
		// set mask for each iteration
		uint32_t mask = 1;

		for(j = 0; j < 8; j++) {
			// set data
			if((mask & data[i]) != 0) {
				GPIOx->BSRR = dataPin;
			} else {
				GPIOx->BSRR = (uint32_t)dataPin << 16U;
			}
			// clock data in
			GPIOx->BSRR = clockPin;
			GPIOx->BSRR = (uint32_t)clockPin << 16U;
			// get next bit
			mask <<= 1;
		}
	}

	// cycle the latch to clock all the bits in
	GPIOC->BSRR = latchPin;
	GPIOC->BSRR = (uint32_t)latchPin << 16U;
}

void clock_in(GPIO_TypeDef* GPIOx, uint16_t clockPin, uint16_t dataPin, uint16_t latchPin, uint8_t outputSize, uint8_t *data) {
	uint8_t i, j;
	uint8_t inData = 0;

	// cycle the latch to clock all the bits in
	GPIOx->BSRR = (uint32_t)latchPin << 16U;
	GPIOx->BSRR = latchPin;

	for(i = 0; i < outputSize; i++) {
		for(j = 0; j < 8; j++) {
			// read data from pin
			inData = HAL_GPIO_ReadPin(GPIOx, dataPin);

			// clock data in to get next bit
			GPIOx->BSRR = (uint32_t)clockPin << 16U;
			GPIOx->BSRR = clockPin;

			// insert bits into the output data
			if(inData != 0)
				data[i] |= (1 << j);
		}
	}
}

void setKeyOutput(uint8_t key, uint32_t *data) {
	// divide by 8 to get the base index
	uint8_t index = key / 8;
	// set the data to the proper index
	data[index] |= 1 << key;
}

void removeKeyOutput(uint8_t key, uint32_t *data) {
	// divide by 8 to get the base index
	uint8_t index = key / 8;
	// set the data to the proper index
	data[index] &= ~(1 << key);
}

int main(void)
{
  /* This sample code shows how to use GPIO HAL API to toggle LED1 and LED3 IOs
    in an infinite loop. */

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 100 MHz */
  SystemClock_Config();

  // enable port c
  __HAL_RCC_GPIOC_CLK_ENABLE();
  // enable port f
  __HAL_RCC_GPIOF_CLK_ENABLE();
  // enable port d
  __HAL_RCC_GPIOD_CLK_ENABLE();
  //enable port g
  __HAL_RCC_GPIOG_CLK_ENABLE();

  // setup struct
  GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Struct.Pull = GPIO_NOPULL;
  GPIO_Struct.Speed = GPIO_SPEED_HIGH;

  // enable output shift register
  GPIO_Struct.Pin = SRO_DATA;
  HAL_GPIO_Init(GPIOC, &GPIO_Struct);
  GPIO_Struct.Pin = SRO_LATCH;
  HAL_GPIO_Init(GPIOC, &GPIO_Struct);
  GPIO_Struct.Pin = SRO_CLOCK;
  HAL_GPIO_Init(GPIOC, &GPIO_Struct);

  // enable input shift register
  GPIO_Struct.Pin = SRI_LATCH;
  HAL_GPIO_Init(GPIOF, &GPIO_Struct);
  GPIO_Struct.Pin = SRI_CLOCK;
  HAL_GPIO_Init(GPIOF, &GPIO_Struct);
  GPIO_Struct.Mode = GPIO_MODE_INPUT;
  GPIO_Struct.Pin = SRI_DATA;
  HAL_GPIO_Init(GPIOF, &GPIO_Struct);
  // set latch pin to high
  GPIOF->BSRR = SRI_LATCH;

  // current song to be played
  Note *currentSong = init_note(KEY_TRACK_EMPTY, 0, 100);
  Note *noteToPlay = NULL;
  int previousNotesPlayed[KEYS];

  // reset key output
  for(i = 0; i < KEY_BYTE_SIZE; i++)
	  keyOutput[i] = 0;

  // set key cool down
    for(i = 0; i < KEY_BYTE_SIZE; i++)
  	  keyInputCoolDown[i] = -1;

  // clear the LEDs
  clock_out(GPIOC, SRO_CLOCK, SRO_DATA, SRO_LATCH, 8, keyOutput);

  // main loop
  while (1)
  {
	  // reset key input
	  for(i = 0; i < KEY_BYTE_SIZE; i++)
		  keyInput[i] = 0;

	  // get all of the key inputs
	  clock_in(GPIOF, SRI_CLOCK, SRI_DATA, SRI_LATCH, KEY_BYTE_SIZE, keyInput);

	  uint8_t curKey = 0;
	  for(i = 0; i < KEY_BYTE_SIZE; i++) {
		  uint8_t mask = 1;

		  for(j = 0; j < 8; j++) {
			  if((mask & keyInput[i]) != 0 && keyInputCoolDown[curKey] == -1) {
				  Note* n = init_note(curKey, currentTime, 10);
				  insert_note(&currentSong, n);
				  totalNotes++;
				  keyInputCoolDown[curKey] = (currentTime + KEY_COOLDOWN) % SONG_LENGTH;
			  }
			  // get next bit
			  mask <<= 1;
			  // increment current key
			  curKey++;
		  }
	  }

	  if(currentSong->key != KEY_TRACK_EMPTY) {
		  if(noteToPlay == NULL) {
			  noteToPlay = currentSong;
		  } else if(noteToPlay->time == currentTime) {
			  setKeyOutput(noteToPlay->key, keyOutput);
			  previousNotesPlayed[noteToPlay->key] = (currentTime + noteToPlay->intensity) % SONG_LENGTH;
			  noteToPlay = noteToPlay->next;
		  }
	  }

	  for(i = 0; i < KEYS; i++) {
		  if(previousNotesPlayed[i] == currentTime) {
			  removeKeyOutput(i, keyOutput);
			  previousNotesPlayed[i] = -1;
		  }
	  }

	  // increment the time
	  currentTime++;
	  if(currentTime > SONG_LENGTH)
		  currentTime = 0;

	  // reset key cool down if it is time too
	  for(i = 0; i < KEYS; i++) {
		  if(keyInputCoolDown[i] == currentTime)
			  keyInputCoolDown[i] = -1;
	  }

	  clock_out(GPIOC, SRO_CLOCK, SRO_DATA, SRO_LATCH, KEY_BYTE_SIZE, keyOutput);

	  // delay
	  HAL_Delay(1);

	  // clear the LEDs
	  //clock_out(GPIOC, CLOCK, DATA, LATCH, 8, 0);

  	  //HAL_RNG_GenerateRandomNumber(&RngHandle, &r);
	  //clock_bit(GPIOC, CLOCK, DATA, 8, r);
	  //GPIOC->BSRR = LATCH;
	  //GPIOC->BSRR = (uint32_t)LATCH << 16U;
	  //HAL_Delay(10);
  	  //clock++;

	  /*
	  //val = clock;
	  val = 128 + 127*sin(clock/Period);
	  clock_bit(GPIOC, CLOCK, DATA, 8, val);
	  GPIOC->BSRR = LATCH;
	  GPIOC->BSRR = (uint32_t)LATCH << 16U;
	  //HAL_Delay(10);
	  clock += 1;
	  if(clock >= SamplesPerSymbol)
		  clock = 0;
		  */
	  //for(i=0; i<T; i++)
	  //  asm("NOP");

	  /*
	  	  HAL_RNG_GenerateRandomNumber(&RngHandle, &r);
		  //r = rand(clock);
		  //r &= 8;
		  //r = (1 << r);
		  clock_bit(GPIOC, CLOCK, DATA, 8, r);
		  GPIOC->BSRR = LATCH;
		  GPIOC->BSRR = (uint32_t)LATCH << 16U;
		  //HAL_Delay(10);
	  	  clock++;
	  	  int i;
	  	  for(i = 0; i < 1000; i++)
	  		  asm("NOP");
*/
	  	  //if(clock > 10000)
	  	  	  //HAL_Delay(40);
	  /*
	  for(i = 0; i < size; i++) {
		  s = (1 << song[i]);
		  clock_bit(GPIOC, CLOCK, DATA, 8, s);
		  GPIOC->BSRR = LATCH;
		  GPIOC->BSRR = (uint32_t)LATCH << 16U;
		  HAL_Delay(100);
	  }
	  */
	  /*
	  clock_bit(GPIOC, CLOCK, DATA, 8, (64+16+4+2+1));
	  GPIOC->BSRR = LATCH;
	  GPIOC->BSRR = (uint32_t)LATCH << 16U;
	  */

	  /*
	  GPIOC->BSRR = GPIO_PIN_0;
	  HAL_Delay(10);
	  GPIOC->BSRR = (uint32_t)GPIO_PIN_0 << 16U;
	  HAL_Delay(1000);
	   */
	  /*
	  //HAL_GPIO_WritePin(GPIOC, DATA, 0);
	  GPIOC->BSRR = (uint32_t)DATA << 16U;
	  asm("NOP; NOP; NOP");
	  //HAL_GPIO_WritePin(GPIOC, CLOCK, 1);
	  GPIOC->BSRR = CLOCK;
	  asm("NOP");
	  asm("NOP");
	  asm("NOP");
	  //HAL_GPIO_WritePin(GPIOC, CLOCK, 0);
	  GPIOC->BSRR = (uint32_t)CLOCK << 16U;
	  asm("NOP");
	  asm("NOP");
	  asm("NOP");
	  //HAL_GPIO_WritePin(GPIOC, LATCH, 1);
	  GPIOC->BSRR = LATCH;
	  asm("NOP");
	  asm("NOP");
	  asm("NOP");
	  //HAL_GPIO_WritePin(GPIOC, LATCH, 0);
	  GPIOC->BSRR = (uint32_t)LATCH << 16U;
	  asm("NOP");
	  asm("NOP");
	  asm("NOP");
	*/
	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
	//HAL_Delay(1);
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
	//HAL_Delay(1);
    //HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
    /* Insert delay 100 ms */
    //HAL_Delay(100);
    //HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
    /* Insert delay 100 ms */
    //HAL_Delay(100);
    //HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
    /* Insert delay 100 ms */
    //HAL_Delay(100);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 200
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  if(ret != HAL_OK)
  {
    while(1) { ; } 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
  if(ret != HAL_OK)
  {
    while(1) { ; }  
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
