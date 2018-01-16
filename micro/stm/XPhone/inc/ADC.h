#ifndef ADC_H_DEF
	#define ADC_H_DEF
	
	#include "main.h"
	/* Definition for ADCx clock resources */
	#define ADCx                            ADC1
	#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()
	#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA2_CLK_ENABLE()
	#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
	
	#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
	#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()
	
	/* Definition for ADCx's Channel */
	#define ADCx_CHANNEL                    ADC_CHANNEL_10
	
	/* Definition for ADCx's DMA */
	#define ADCx_DMA_CHANNEL                DMA_CHANNEL_0
	#define ADCx_DMA_STREAM                 DMA2_Stream0
	
	/* Definition for ADCx's NVIC */
	#define ADCx_DMA_IRQn                   DMA2_Stream0_IRQn
	#define ADCx_DMA_IRQHandler             DMA2_Stream0_IRQHandler
	
	#define ADCx_RESOLUTION					ADC_RESOLUTION_12B
	#define ADCx_REF_VOLTAGE				((float)3.3)
	#define ADCx_MAX_CONV					((uint16_t)(1<<12)-1)	
	#define ADC_POLL_TIMEOUT_MS				(5)
	#define ADC_Type						uint16_t
	#define ADC_Type_MAX					((ADC_Type)-1)
	
	// the variable that stores all of the ADCx configuration
	ADC_HandleTypeDef    AdcHandle;
	
	#define ADC_volt(read) (ADCx_REF_VOLTAGE*read/ADCx_MAX_CONV)
	
	// function prototypes (see ADC.c for function definitions).
	void ADC_init();
	ADC_Type ADC_read(ADC_HandleTypeDef *);
	
#endif
