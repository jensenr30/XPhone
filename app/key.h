// key.h
//
// keys are buttons that the user presses.
// keys 0,1,2, ... correspond to physical buttons/interfaces that the player interacts with.
// In the case of Jensen and JP's xylophone, key 0 happens to be Middle C.
// To conform to the MIDI standard, we would say that the MIDI note is given as:
//		MIDI = 60 + XYLOPHONE_KEY;
// where 60 corresponds to Middle C in the MIDI standard.

#ifndef KEY_H_INCLUDED
	#define KEY_H_INCLUDED
	
	#define KEY_TRACK_EMPTY 255
	
	/// Structures
	
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
	#endif
	
	
	/// Note structure
	// the notes to be played
	typedef struct Note {
		// the key to be played
		uint8_t key;
		// the time between the start of the track and
		// when the key will be played (in milliseconds)
		uint32_t time;
		// how hard the key is going to be hit
		uint8_t intensity;
		// pointer to the next element in the list
		struct Note *next;
	} Note;
	
	
	/// Functions
	Note* init_note(uint8_t key, uint32_t time, uint8_t intensity);
	void insert_note(Note **song, Note *note);
	void clear_song(Note *song);
	
	#if SDL_PROGRAM
	
	void play_song(Note *song, SDLKey *keys);
	
	#endif // SDL_PROGRAM
	
#endif // KEY_H_INCLUDED
