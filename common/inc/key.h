#ifndef KEY_H_INCLUDED
	#define KEY_H_INCLUDED
	
	#include "GPIO.h"
	#include <inttypes.h>
	#include <stdlib.h>
	#define KeyType uint8_t					// this is the data type used to index into the keys (for XPhone, this goes from 0 to 36).
	#define KeyIntensityType uint8_t		// this is the data type used to store the intensity of the key hit. 
	#define KeyStateType uint8_t			// this is the data type used to record the state of a key (boolean: 0 or 1)
	#define KeyTimeType uint32_t			// this is the data type used to record the time at which the key was played (ms)
	#define KeyTimeMax UINT32_MAX			// the is the max key time. 1.6 months.  :O
	#define KEYS 			((KeyType)37)	// this is the total number of keys on the XPhone hardware operated by the stm32
	#define MIDI_OFFSET 	((KeyType)60)	// the xylophone starts on middle C, and goes up from there.
	
	// key input states
	volatile KeyStateType key_input_states[KEYS];	// the key input states
	// key input cooldown periods
	#define KEY_COOLDOWN ((KeyTimeType)20)	// Once a key is hit, it takes this many milliseconds before the XPhone will recognize another key hit. If you hit they key a bunch of times within this window of time, it will only register the first key hit. This is a way of debouncing the input.
	KeyTimeType KeyCooldownTimes[KEYS];		// this tells you how many milliseconds you have to wait until you can input on this key again. this is usually set to 50 right away, and it gets decremented.
	uint8_t KeyCooldownActive[KEYS];		// this array indicates (for each key) whether or not it is in its cooldown period.
	
	#define KEY_TRACK_EMPTY 255	
	/// Note structure
	// the notes to be played
	typedef struct Note {
		// the key to be played
		KeyType key;
		// the time between the start of the track and
		// when the key will be played (in milliseconds)
		KeyTimeType time;
		// how hard the key is going to be hit
		KeyIntensityType intensity;
		// pointer to the next element in the list
		struct Note *next;
	} Note;
	
	
	
	// TODO figure out a way to change uint32_t to SolTimType without having build issues. It is probably due to my shitty .h and .c structure...
	uint32_t keyIntensityMin[KEYS];
	uint32_t keyIntensityMax[KEYS];
	#define KEY_CAL_START	((uint32_t)1500)	// initial solenoid intensity
	#define KEY_CAL_STEP	((uint32_t)100)		// solenoid intensity step size.
	#define KEY_CAL_STEP_TIME ((200))
	
	/// Functions
	Note* init_note(KeyType key, KeyTimeType time, KeyIntensityType intensity);
	void insert_note(Note **song, Note *note);
	void clear_song(Note *song);
	void note_clear_from_memory(Note *note);
	void key_init();
	void key_cal();
	void keys_read();
	void key_make_track_empty(Note *note);
	
	#if SDL_PROGRAM
		/// SDLKey structure
		// the key to be rendered
		typedef struct {
			// holds a rectangle for the rendering
			// a rectangle or "a key"
			SDL_Rect rect;
			// the color of the rect or "the key"
			int color;
		} SDLKey;
		
		void play_song(Note *song, SDLKey *keys);
		
	#endif // SDL_PROGRAM

#endif // KEY_H_INCLUDED
