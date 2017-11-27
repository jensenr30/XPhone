// main.c
//
//

#include "globals.h"
#include "key.h"

int main(int argc, char* args[])
{
    // event variable
    SDL_Event event;

    // for loop vars
    int i;

    // mouse coordinates and structure
    int x, y, clicked = 0, justClicked = 0;
    SDL_Rect mousePos;

    // SDL key variables
    SDLKey keys[NUM_KEYS];
    int playKeys = 0, playSpeed = 0, currentlyPlayedKey = 0;

    // current song to be played
    Note *currentSong = malloc(sizeof(Note));
    // the track starts empty
    currentSong = init_note(KEY_TRACK_EMPTY, 0, 100);
    Note *noteToPlay = NULL;
    int currentTime = 0;
    int previousNotesPlayed[NUM_KEYS];

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
        previousNotesPlayed[i-1] = -1;
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
                        justClicked = 1;
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
                            case SDLK_r:
                                clear_song(currentSong);
                                //currentTime = 0;
                                break;
                            case SDLK_t:
                                save_song(currentSong, "songdata.txt");
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
                if(currentSong->key != KEY_TRACK_EMPTY) {
                    // check to see if we need to get the first element
                    if(noteToPlay == NULL) {
                        noteToPlay = currentSong;
                    }
                    // check if it is the right time to play the note
                    else if(noteToPlay->time == currentTime) {
                        // play the note
                        keys[noteToPlay->key].color = 0xFFFFFF;
                        // save note just played with its play time
                        previousNotesPlayed[noteToPlay->key] = (currentTime + noteToPlay->intensity) % SONG_LENGTH;
                        // get the next note to be played
                        noteToPlay = noteToPlay->next;
                    }
                }

                // iterate through previously played keys
                for(i = 0; i < NUM_KEYS; i++) {
                    // check if it is time to reset the key
                    if(previousNotesPlayed[i] == currentTime) {
                        // set key color back to normal
                        keys[i].color = 0xFF0000;
                        // reset previously played key time
                        previousNotesPlayed[i] = -1;
                    }
                }

                // render keys
                for(i = 0; i < NUM_KEYS; i++) {
                    SDL_FillRect(screen , &keys[i].rect , keys[i].color);
                }

                // render timeline
                for(i = 0; i < SONG_LENGTH; i += 2) {
                    SDL_Rect timeline;
                    timeline.y = 20;
                    timeline.x = i + 10;
                    timeline.w = 1;
                    timeline.h = 6;
                    SDL_FillRect(screen, &timeline, 0xffffff);
                }

                // render timeline arrow
                SDL_Rect timeline_arrow;
                timeline_arrow.y = 30;
                timeline_arrow.x = currentTime + 10;
                timeline_arrow.w = 1;
                timeline_arrow.h = 1;
                SDL_FillRect(screen, &timeline_arrow, 0xffffff);
                timeline_arrow.x = currentTime - 1 + 10;
                timeline_arrow.y = 31;
                timeline_arrow.w = 3;
                timeline_arrow.h = 1;
                SDL_FillRect(screen, &timeline_arrow, 0xffffff);
                timeline_arrow.x = currentTime - 2 + 10;
                timeline_arrow.y = 32;
                timeline_arrow.w = 5;
                timeline_arrow.h = 1;
                SDL_FillRect(screen, &timeline_arrow, 0xffffff);

                // update timeline from current song
                // if the song does not have any elements don't play it
                if(currentSong->key != KEY_TRACK_EMPTY) {
                    // iteration variable
                    Note *cur = currentSong;
                    // iterate through the list
                    while(cur != NULL) {
                        // create note based on current song
                        SDL_Rect timeline_note;
                        timeline_note.x = cur->time;
                        timeline_note.y = 16;
                        timeline_note.w = 1;
                        timeline_note.h = 3;
                        SDL_FillRect(screen, &timeline_note, 0xffffff);
                        // get next element
                        cur = cur->next;
                    }
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
                            SDL_FillRect(screen, &keys[i].rect, 0x0000FF);
                            // check if the key was just clicked
                            if(justClicked == 1) {
                                // create note
                                Note *n = init_note(i, currentTime, 100);
                                // add in the note that was hit to the track
                                insert_note(&currentSong, n);
                                // reset justclicked
                                justClicked = 0;
                            }
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

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}
