// globals.h
//
//
#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED
	
	#if !__arm__
		#define SDL_PROGRAM 1		// defines SDL_PROGRAM if it should compile for SDL on a PC.
	#endif
	
	
	#if SDL_PROGRAM
		#define SCREEN_WIDTH 1024	// initial width  of the program window.
		#define SCREEN_HEIGHT 768	// initial height of the program window.
		#define NUM_KEYS 8			//
		#define PLAY_SPEED 200		//
		#define SONG_LENGTH 1000	//
		
		#include <stdio.h>			//
		#include <SDL2/SDL.h>		// used for graphics, user interface, and sound.
	#endif
	
	#include "key.h"			//
	
#endif // GLOBALS_H_INCLUDED
