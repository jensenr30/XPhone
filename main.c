#include "globals.h"

int main(int argc, char* args[])
{
    // event variable
    SDL_Event event;

    // for loop vars
    int i;

    // mouse coordinates and structure
    int x, y, clicked = 0;
    SDL_Rect mousePos;

    // SDL key variables
    SDLKey keys[NUM_KEYS];
    int playKeys = 0, playSpeed = 0, currentlyPlayedKey = 0;

    // current song to be played
    Note *currentSong = malloc(sizeof(Note));
    // the 255 key indicates that the track is empty
    currentSong->key = 255;
    Note *noteToPlay = malloc(sizeof(Note));
    Note *previousNotePlayed = malloc(sizeof(Note));
    previousNotePlayed->key = 255;
    int intensityCounter = 0;
    int currentTime = 0;

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
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
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
                                play_song(currentSong, keys);
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
                        // get the next key
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

                // check if the song is empty
                if(currentSong->key != 255) {
                    // check to see if we need to get the first element
                    if(currentSong->next == NULL) {
                        noteToPlay = currentSong;
                    }
                    // check if it is the right time to play the note
                    if(noteToPlay->time == currentTime) {
                        // play the note
                        keys[noteToPlay->key].color = 0xFFFFFF;
                        // save note just played
                        if(previousNotePlayed->key == 255) {
                            previousNotePlayed = noteToPlay;
                        } else {
                            previousNotePlayed->next = noteToPlay;
                        }
                        // get the next note to be played
                        noteToPlay = currentSong->next;
                    }
                }

                // check if the list is empty
                if(previousNotePlayed->key != 255) {
                    // check to see how long the key should be played
                    if(previousNotePlayed->intensity > intensityCounter) {
                        // reset the timer
                        intensityCounter = 0;
                        // set the key back to original color
                        keys[previousNotePlayed->key].color = 0xFF0000;
                        // get the next element
                        previousNotePlayed = previousNotePlayed->next;
                        // check if the list is now empty
                        if(previousNotePlayed == NULL) {
                            // reset the list to have the empty list key
                            previousNotePlayed = malloc(sizeof(Note));
                            previousNotePlayed->key = 255;
                        }
                    } else {
                        intensityCounter++;
                    }
                }

                // render keys
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
                            // add in the note that was hit to the track
                            Note *n = malloc(sizeof(Note));
                            n->key = i;
                            n->time = currentTime;
                            n->intensity = 100;
                            n->next = NULL;
                            insert_note(currentSong, n);
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
                // update song time
                currentTime++;
                if(currentTime > SONG_LENGTH) {
                    currentTime = 0;
                }
		    }
		}
	}

    // free dat memory
    free(currentSong);
    free(noteToPlay);
    free(previousNotePlayed);

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}
