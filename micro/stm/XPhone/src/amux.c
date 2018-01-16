#include "amux.h"

// This function will read the analog voltage of one of the inputs of the analog multiplexer.
// This is handy when you want to know how loud the user hit a particular key.
// this function will set the amux shift register automatically (configure which analog multiplexer input to read from) which should be obvious.
// TODO: figure out how long this function typically takes (in total microseconds)
ADC_Type amux_read(KeyType k)
{
	// make sure that this function was not called by a dumbass.
	if(k >= KEYS)
	{
		warning("amux_read got a k >= KEYS");
		return ADC_Type_MAX;
	}
	
	// shift (speed limited) using the out using the 
	uint8_t b;
	for(b=0; b<AMUX_CTRL_BITS; b++)
	{
		pin_set(AMUX_SR_DATA_GPIO,AMUX_SR_DATA, (k&(1<<b)) != 0);
		pin_off(AMUX_SR_CLOCK_GPIO,AMUX_SR_CLOCK);
		wait_100ns();
		wait_100ns();
		pin_on(AMUX_SR_CLOCK_GPIO,AMUX_SR_CLOCK);
		wait_100ns();
		wait_100ns();
	}
	
	// TODO: figure out how long I need to pause before sampling the analog voltage.
	// TODO: I would think that 30 us would be sufficient, but I should try to measure how long it takes to switch between keys... 
	pause_us(30);
	
	//shift_out();
	return ADC_read(&AdcHandle);
}

