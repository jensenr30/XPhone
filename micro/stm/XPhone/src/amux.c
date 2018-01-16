#include "amux.h"

// This function will read the analog voltage of one of the inputs of the analog multiplexer.
// This is handy when you want to know how loud the user hit a particular key.
// this function will set the amux shift register automatically (configure which analog multiplexer input to read from) which should be obvious.
ADC_Type amux_read(KeyType k)
{
	// make sure that this function was not called by a dumbass.
	if(k >= KEYS)
	{
		warning("amux_read got a k >= KEYS");
		return ADC_Type_MAX;
	}
	
	// calculate the bits we need to shift out in order to properly control the shift register.
	uint8_t key_select[AMUX_CTRL_BITS];
	uint8_t b;
	for(b=0; b<AMUX_CTRL_BITS; b++) { key_select[b] = ( (k&(1<<b)) == 1); }
	
	// output the data to the [shift register] that controls the [analog multiplexer].
	// this will make the analog multiplexer feed the STM32 the voltage from the desired key, (i.e. the argument passed to this function: k).
	// NOTE: I am sending this the data pin for the "latch" pin. This is because the shift register for the amux is a 74HC164. this is an unbuffered shift register, so the bits are shifted out (and available) in real time. The data is kind of a dummy pin. data pin will get jerked around at the end of the shift_out() function, but it doesn't matter. It will just look kind of funny if you ever happen to be debugging it.
	shift_out(AMUX_SR_CLOCK_GPIO,AMUX_SR_CLOCK,AMUX_SR_DATA_GPIO,AMUX_SR_DATA,AMUX_SR_DATA_GPIO,AMUX_SR_DATA,AMUX_CTRL_BITS,key_select,0);
	
	// TODO: figure out how long I need to pause before sampling the analog voltage.
	// TODO: I would think that 30 us would be sufficient, but I should try to measure how long it takes to switch between keys... 
	pause_us(30);
	
	//shift_out();
	return ADC_read(&AdcHandle);
}

