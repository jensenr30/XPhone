#ifndef AMUX_H_INCLUDED
	#define AMUX_H_INCLUDED
	
	#include "key.h"
	#include "ADC.h"
	#include "debug.h"
	#include "GPIO.h"
	#include "inttypes.h"
	#include "pause.h"

	#define AMUX_CTRL_BITS	(4)			// this is how many bits it takes to control each analog mux
	#define AMUX_SIZE		(1<<AMUX_CTRL_BITS)		// this is how many inputs each MUX has
	
	ADC_Type amux_read(KeyType k);

#endif

// 
