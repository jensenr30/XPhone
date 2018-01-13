#include "ADC.h"

ADC_Type ADC_read(ADC_HandleTypeDef *han)
{
	HAL_ADC_Start(han);
	HAL_ADC_PollForConversion(han,ADC_POLL_TIMEOUT_MS);
	return (ADC_Type)HAL_ADC_GetValue(han);
}

//-------------------------------------------------------------------------
// code to initialize the ADC (eventually, put this in ADC.c in ADC_init())
//-------------------------------------------------------------------------
void ADC_init()
{
	ADC_ChannelConfTypeDef sConfig;
	/*##-1- Configure the ADC peripheral #######################################*/
	AdcHandle.Instance          = ADCx;
	
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV8;
	AdcHandle.Init.Resolution            = ADCx_RESOLUTION;
	AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.NbrOfDiscConversion   = 0;
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start trigged at each external event */
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion       = 1;
	AdcHandle.Init.DMAContinuousRequests = ENABLE;
	AdcHandle.Init.EOCSelection          = DISABLE;
	
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		/* ADC initialization Error */
		error("Could not initialize ADC!");
	}
	
	/*##-2- Configure ADC regular channel ######################################*/
	sConfig.Channel      = ADC_CHANNEL_10;
	sConfig.Rank         = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset       = 0;
	
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		/* Channel Configuration Error */
		error("Could not configure ADC!");
	}
}
