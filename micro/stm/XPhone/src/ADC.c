#include "ADC.h"

ADC_Type ADC_read(ADC_HandleTypeDef *han)
{
	HAL_ADC_Start(han);
	HAL_ADC_PollForConversion(han,ADC_POLL_TIMEOUT_MS);
	return (ADC_Type)HAL_ADC_GetValue(han);
}
