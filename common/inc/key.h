#ifndef KEY_H_INCLUDED
	#define KEY_H_INCLUDED
	
	#include "main.h"
	#include <inttypes.h>
	#include <stdlib.h>
	#define KeyType uint8_t					// this is the data type used to index into the keys (for XPhone, this goes from 0 to 36).
	#define KeyIntensityType uint8_t		// this is the data type used to store the intensity of the key hit. 
	#define KeyStateType uint8_t			// this is the data type used to record the state of a key (boolean: 0 or 1)
	#define KeyTimeType uint32_t			// this is the data type used to record the time at which the key was played (ms)
	#define KeyTimeMax UINT32_MAX			// the is the max key time.

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
	
	
	/// Functions
	Note* init_note(KeyType key, KeyTimeType time, KeyIntensityType intensity);
	void insert_note(Note **song, Note *note);
	void clear_song(Note *song);
	
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
