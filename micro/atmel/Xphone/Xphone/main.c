#define F_CPU (20000000)		// define the clock speed to be 20 MHz.
#include <avr/io.h>				// standard input/output functions
#include <util/delay.h>			// include the use of the delay functions _delay_ms() and _delay_us()
#include <avr/interrupt.h>		// this allows me to use interrupt functions
#include "shift_out_328.h" 

// 74hc595 serial->parallel output shift register control
#define p_SR_data			PORTC4		// this pin holds the data that must be clocked into the shift register
#define p_SR_RCK			PORTC3		// this pin updates the outputs of the shift register
#define p_SR_SCK			PORTC2		// this pin clocks data into the shift register
// trash up the ass
#define p_input				PORTC5
#define p_key				PORTC1
// at25sf161 Flash (2 MB, 16 Mbit) Chip.
#define p_FLASH_CS			PORTD0
#define p_FLASH_SO			PORTD1
#define p_FLASH_SCK			PORTD2		// flash serial clock. when writing data in, rising-edge triggered. When reading data out, falling-edge triggered.
#define p_FLASH_SI			PORTD3

//=================================================================
// function definitions
//=================================================================

// input/output functions are defined here
#define low(port, pin)		port &=	~(1<<pin)				// this sets a certain pin low.
#define high(port, pin)		port |=	 (1<<pin)				// this sets a certain pin high.
#define toggle(port, pin)	port ^=  (1<<pin)				// this toggles a certain pin.
#define set_input(portdir, pin) portdir &= ~(1<<pin)		// this sets a certain pin as an input.
#define set_output(portdir, pin) portdir |= (1<<pin)		// this sets a certain pin as an output.
#define get_bit(data, bit_numb) ((data >> bit_numb) & 1)	// this gets a specific bit from the input

//=================================================================
// Shift register output pins guide
//=================================================================
// The shift register is serial-in parallel-out.
// The shift register is 24 bits long.
// Q0 is the first output of the shift register
// Q23 is the last output of the shift register
/*
	Q0		seven-segment A  anode for all digits
	Q1		seven-segment B  anode for all digits
	Q2		seven-segment C  anode for all digits
	Q3		seven-segment D  anode for all digits
	Q4		seven-segment E  anode for all digits
	Q5		seven-segment F  anode for all digits
	Q6		seven-segment G  anode for all digits
	Q7		seven-segment DP anode for all digits
	
	Q8		unit indicator	MegaHertz
	Q9		unit indicator	Hertz
	Q10		unit indicator	Seconds
	Q11		unit indicator	Minutes
	Q12		frequency division selection 0
	Q13		frequency division selection 1
	Q14		frequency division selection 2
	Q15		
	
	Q16		digit 0	-	transistor pulling digit 0's common cathode low (most  significant digit) (left-most  digit)
	Q17		digit 1	-	transistor pulling digit 1's common cathode low
	Q18		digit 2	-	transistor pulling digit 2's common cathode low
	Q19		digit 3	-	transistor pulling digit 3's common cathode low
	Q20		digit 4	-	transistor pulling digit 4's common cathode low
	Q21		digit 5	-	transistor pulling digit 5's common cathode low (least significant digit) (right-most digit)
	Q22		
	Q23		
*/

#define WRITE_CMD 2
#define READ_CMD 11
#define WRITE_ENABLE 6

void send_command_PORTD(uint8_t sck_pin, uint8_t si_pin, uint8_t command) {
	uint8_t i;
	
	for(i = 0; i < 8; i++) {
		// get last bit and set port bit
		if(command & 128)
			high(PORTD, si_pin);
		else
			low(PORTD, si_pin);
		
		// clock it in
		low(PORTD, sck_pin);
		high(PORTD, sck_pin);
		// get the next bit
		command = command << 1;
	}
}

#define size 14
int song[size] = {
	0,1,2,3,4,5,6,7,6,5,4,3,2,1
};

int main(void)
{
	
	//=================================================================
	// setup pins
	//=================================================================
	
	// setup output ports
	set_output(DDRC, p_SR_data);
	set_output(DDRC, p_SR_RCK);
	set_output(DDRC, p_SR_SCK);
	set_input(DDRC, p_input);
	set_output(DDRC, p_key);
	
	// flash memory pins
	set_output(DDRD, p_FLASH_CS);
	set_input(DDRD, p_FLASH_SO);
	set_output(DDRD, p_FLASH_SCK);
	set_output(DDRD, p_FLASH_SI);
	
	//=================================================================
	// main loop
	//=================================================================
	/*
	while(1)
	{
		high(PORTD, p_FLASH_CS);
		low(PORTD, p_FLASH_CS);
	}
	*/
	
	int s = 1, i;
	uint8_t myInput = 0;
	uint32_t address = 0;
	//while(1){
	
	shift_out_328('C', p_SR_SCK, p_SR_data, 8, (64+16+4+1), 'M');
	high(PORTC, p_SR_RCK);
	low(PORTC, p_SR_RCK);
		
	// set cs to inactive state
	high(PORTD, p_FLASH_CS);
	_delay_ms(1000);
	
	
	
	//while(1){
		// enable write
		low(PORTD, p_FLASH_CS);
		send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, WRITE_ENABLE);
		high(PORTD, p_FLASH_CS);
		_delay_ms(1);
	//}
	// write data
	// start transmition
	low(PORTD, p_FLASH_CS);
	// write command
	send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, WRITE_CMD);
	// 3 bytes for address
	send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 0);
	send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 0);
	send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 0);
	// data
	send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 255);
	// end transmition
	high(PORTD, p_FLASH_CS);
	
	//_delay_ms(10);
	//address++;
	//}
    while (1)
    {
		// read data
		// start transmition
		low(PORTD, p_FLASH_CS);
		// write command
		send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, READ_CMD);
		// 3 bytes for address
		send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 0);
		send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 0);
		send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 0);
		// dummy byte
		send_command_PORTD(p_FLASH_SCK, p_FLASH_SI, 0);
		
		myInput = 0;
		for(i = 0; i < 8; i++) {
			myInput = (myInput << 1) | get_bit(PIND, p_FLASH_SO);
			high(PORTD, p_FLASH_SCK);
			low(PORTD, p_FLASH_SCK);
		}
		
		// end transmition
		high(PORTD, p_FLASH_CS);
		
		shift_out_328('C', p_SR_SCK, p_SR_data, 8, myInput, 'M');
		high(PORTC, p_SR_RCK);
		low(PORTC, p_SR_RCK);
		
		/*
		code for solinoid test
		high(PORTC, p_key);
		_delay_ms(5);
		low(PORTC, p_key);
		_delay_ms(500);
		*/
		/*
		//code for simple song
		for(i = 0; i < size; i++) {
			s = (1 << song[i]);
			shift_out_328('C', p_SR_SCK, p_SR_data, 8, s, 'M');
			high(PORTC, p_SR_RCK);
			low(PORTC, p_SR_RCK);
			_delay_ms(100);
		}
		*/
		/*
		// input
		uint8_t portCdata = PINC;
		myInput = get_bit(portCdata, p_input);
		
		if(myInput == 1)
			s = s << 1;
			//s += 1;
		
		if(s > 255)
			s = 1;
		
		shift_out_328('C', p_SR_SCK, p_SR_data, 8, s, 'M');
		_delay_ms(100);
		high(PORTC, p_SR_RCK);
		low(PORTC, p_SR_RCK);
		*/
		//s = s << 1;
		//s = 1 << 1;
		//s |= (1<<2);
		//s |= (1<<5);
		//s += 1;
		
		//if((s & ((1<<7) - 1) ) == 0)
		//	s = 1;
		
		/*
		high(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		low(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		low(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		low(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		low(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		low(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		low(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		high(PORTC, p_SR_data);
		low(PORTC, p_SR_SCK);
		high(PORTC, p_SR_SCK);
		
		high(PORTC, p_SR_RCK);
		low(PORTC, p_SR_RCK);
		_delay_us(10000);
		*/
    }
	
}