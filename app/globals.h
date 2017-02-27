// globals.h
//
//

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#define SCREEN_WIDTH 1024	// initial width  of the program window.
#define SCREEN_HEIGHT 768	// initial height of the program window.
#define NUM_KEYS 8			//
#define PLAY_SPEED 200		//
#define SONG_LENGTH 1000	//

#define SDL_PROGRAM 1		// this should be 1 if you are compiling for the SDL program on a PC.
#define MICRO_PROGRAM 0		// this should be 1 if you are compiling for a microcontroller.

#if SDL_PROGRAM
#include <stdio.h>			//
#include <SDL2/SDL.h>		// used for graphics, user interface, and sound.
#endif

#include "key.h"			//

#endif // GLOBALS_H_INCLUDED
