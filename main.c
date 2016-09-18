#include "globals.h"

int main( int argc, char* args[] )
{
    // event variable
    SDL_Event event;

    // for loop vars
    int i;

    // mouse coordinates and structure
    int x, y, clicked = 0;
    SDL_Rect mousePos;

    // key variables
    SDLKey keys[NUM_KEYS];
    int playKeys = 0, playSpeed = 0, currentlyPlayedKey = 0;

    // song variables
    Note *currentSong = malloc(sizeof(Note));
    // creating a note to insert
    Note *n = malloc(sizeof(Note));
    n->key = 1;
    n->time = 0;
    n->intensity = 0;
    insert_note(currentSong, n);
    // free dat memory
    free(n);

    // clock speed
    int clockspeed = 0;

    // create keys
    for(i = 1; i < NUM_KEYS + 1; i++) {
        SDLKey curKey;
        curKey.rect.x = i * 100;
        curKey.rect.y = 100;
        curKey.rect.w = 50;
        curKey.rect.h = 600 - (i * 30);
        curKey.color = 0xFF0000;
        keys[i-1] = curKey;
    }

	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screen = NULL;

	//Initialize SDL
	if(SDL_Init( SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		window = SDL_CreateWindow( "X Phone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if(window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
		    while(1) {
                while(SDL_PollEvent(&event)){
                    if(event.type == SDL_QUIT) {
                        SDL_Quit();
                        return 0;
                    }
                    // check mouse state
                    else if(event.type == SDL_MOUSEBUTTONDOWN) {
                        clicked = 1;
                    } else if(event.type == SDL_MOUSEBUTTONUP) {
                        clicked = 0;
                    // check key presses
                    } else if(event.type == SDL_KEYDOWN) {
                        switch(event.key.keysym.sym) {
                            case SDLK_a:
                                if(clicked == 0)
                                    clicked = 1;
                                else
                                    clicked = 0;
                                break;
                            case SDLK_s:
                                playKeys = 1;
                                break;
                            case SDLK_d:
                                play_track(currentSong, keys);
                                break;
                            default:
                                break;
                        }
                    }
                }

                //Get window surface
                screen = SDL_GetWindowSurface(window);

                //Fill background with black
                SDL_FillRect(screen, NULL, 0x000000);

                // play all keys sequentially
                if(playKeys == 1) {
                    // play the next key only if the timer has elapsed
                    if(playSpeed > PLAY_SPEED) {
                        // set current key back to starting value
                        keys[currentlyPlayedKey].color = 0xFF0000;
                        // get the next speed
                        currentlyPlayedKey++;
                        // reset timer
                        playSpeed = 0;
                        // check to see if we played all the keys
                        if(currentlyPlayedKey > NUM_KEYS) {
                            // reset
                            playKeys = 0;
                            currentlyPlayedKey = 0;
                        }
                    } else {
                        // increase timer
                        playSpeed++;
                        // keep key set to blue
                        keys[currentlyPlayedKey].color = 0x0000FF;
                    }
                }

                // render keys as red
                for(i = 0; i < NUM_KEYS; i++) {
                    SDL_FillRect(screen , &keys[i].rect , keys[i].color);
                }

                // check if mouse was clicked
                if(clicked == 1) {
                    // get current location
                    SDL_GetMouseState(&x, &y);
                    // fill in current location
                    /*
                    int size = 20;
                    mousePos.x = x - size / 2;
                    mousePos.y = y - size / 2;
                    mousePos.w = size;
                    mousePos.h = size;
                    // display mouse click
                    SDL_FillRect(screen , &mousePos , 0x00FF00);
                    */
                    // check if key was hit
                    for(i = 0; i < NUM_KEYS; i++) {
                        if(x > keys[i].rect.x && x < keys[i].rect.x + keys[i].rect.w && y > keys[i].rect.y && y < keys[i].rect.y + keys[i].rect.h) {
                            // change color of key to blue
                            SDL_FillRect(screen , &keys[i].rect , 0x0000FF);
                        }
                    }
                }

                //Update the surface
                SDL_UpdateWindowSurface(window);
                // update clock
                clockspeed++;
                if(clockspeed > 10000) {
                    clockspeed = 0;
                }
		    }
		}
	}

    // free dat memory
    free(currentSong);

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}
