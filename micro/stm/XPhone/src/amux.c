#include "amux.h"
#include "debug.h"
#include "GPIO.h"
#include "inttypes.h"

// This function will read the analog voltage of one of the inputs of the analog multiplexer.
// This is handy when you want to know how loud the user hit a particular key.
// this function will set the amux shift register automatically (configure which analog multiplexer input to read from) which should be obvious.
ADC_Type amux_read(KeyType k)
{
	ADC_Type value = 0;
	if(k >= KEYS)
	{
		warning("amux_read got a k >= KEYS");
		return ADC_Type_MAX;
	}
	
	
	uint8_t key_select[2*AMUX_CTRL_BITS];
	
	uint8_t b;
	for(b=0; b<AMUX_CTRL_BITS; b++)
	{
		key_select[b] = ( (k&(1<<b)) == 1);
	}
	
	
	//shift_out();
	return value;
}

