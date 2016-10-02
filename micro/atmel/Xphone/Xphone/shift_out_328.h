
// this function shifts out data to a serial-in-parallel-out shift register on some port.
// This function works on ports B, C, and D.
// This function was designed to be used on the ATmega328P microcontroller,
// but it will likely work on many other Ateml microcontrollers will similar port configurations.

// "port" is a character 'b', 'c', 'd', or 'B', 'C', or 'D'. This tells the function where to shift out.
// "clockPin" is the pin in "port" that will be used as the clock of the shift register. values 0-7 are acceptable.
// "dataPin"  is the pin in "port" that will be used as the data  of the shift register. values 0-7 are acceptable.
	// NOTE: This function will not set the DDRx registers.
	// The clockPin and dataPin of the port need to be configured as outputs before attempting to shift out data.
// bits is the number of bits that need to be shifted out.
// data is the data to be shifted out.
// bitDirection is 'm' or 'M' if you want to shift out the most significant bit first (MSBF). Any other character will be interpreted as LSBF.
void shift_out_328(char port, uint8_t clockPin, uint8_t dataPin, uint16_t bits, uint64_t data, char bitDirection){
	
	unsigned int i;					// this is used to count through the bits
	long unsigned int mask = 1;		// this is used to selectively pick bits from "data" to shift out one at a time.
	
	// set up the initial state of the bit mask
	if(bitDirection == 'm' || bitDirection == 'M'){
		for(i=1; i<bits; i++)	// start the mask at 2^(bits - 1)
			mask <<= 1;
	}
	
	for(i=0; i<bits; i++){
			
		//set data bit high if needed
		if(data&mask){
			switch(port){
			case 'b':
			case 'B':
				PORTB |=  (1 << dataPin);
				break;
			case 'c':
			case 'C':
				PORTC |=  (1 << dataPin);
				break;
			case 'd':
			case 'D':
				PORTD |=  (1 << dataPin);
				break;
			default:
				break;
			}
		}
		// otherwise, set the data bit low
		else{
			switch(port){
				case 'b':
				case 'B':
					PORTB &= ~(1 << dataPin);
					break;
				case 'c':
				case 'C':
					PORTC &= ~(1 << dataPin);
					break;
				case 'd':
				case 'D':
					PORTD &= ~(1 << dataPin);
					break;
				default:
					break;
			}
		}
			
		//clock data into the register.
		switch(port){
			case 'b':
			case 'B':
				PORTB &= ~(1 << clockPin);
				PORTB |=  (1 << clockPin);
				break;
			case 'c':
			case 'C':
				PORTC &= ~(1 << clockPin);
				PORTC |=  (1 << clockPin);
				break;
			case 'd':
			case 'D':
				PORTD &= ~(1 << clockPin);
				PORTD |=  (1 << clockPin);
				break;
			default:
				break;
		}
			
		// modify mask for the next loop iteration.
		if(bitDirection == 'm' || bitDirection == 'M')		// if the bit direction if MSB (Most Significant Bit first),
			mask >>= 1;										// shift the mask right (decrease the bit mask).
		else												// otherwise,
			mask <<= 1;										// shift the mask left  (increase the bit mask).
	}
}
