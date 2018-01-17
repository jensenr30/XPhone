#ifndef AMUX_H_INCLUDED
	#define AMUX_H_INCLUDED
	
	#include "main.h"
	#include "key.h"
	#include "ADC.h"
	#include "debug.h"
	#include "GPIO.h"
	#include "inttypes.h"
	#include "pause.h"

	// this is how many bits it takes to control each analog mux
	#define AMUX_CTRL_BITS	(8)
	
	// this is how many milliseconds we need to wait between getting a key stroke and measuring its voltage.
	// if you measure too early, you will run the risk sampling on either side of edges created by the peak-detection circuit.
	// waiting for this period of time (an no shorter) helps make the analog measurements repeatable.
	#define AMUX_SAMPLE_HOLDOFF (40)
	
	// this is how many pending key strikes we can support.
	// These are the keys that have been hit, but cannot sample YET because AMUX_SAMPLE_HOLDOFF has not yet elapsed.
	// so we keep them in an array until the proper time to sample their voltage has come.
	#define AMUX_PENDING_KEY_READS_MAX	(10)
	
	// this is the array that keeps track of all the keys that have been hit, but cannot sample YET because AMUX_SAMPLE_HOLDOFF has not yet elapsed.
	Note *amux_pending_key_reads[AMUX_PENDING_KEY_READS_MAX];
	
	void amux_init();
	ADC_Type amux_read(KeyType k);
	
	
#endif

// 
