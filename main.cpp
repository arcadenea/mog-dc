#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef DREAMCAST
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#else
#include "SDL.h"
#include "SDL_mixer.h"
#endif
#include "sound.h"

#ifdef DREAMCAST
#include <kos.h>
#endif

#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"

#include "mog.h"

extern void DebugReport(void);

int SCREEN_X = 640;
int SCREEN_Y = 400;
int COLOUR_DEPTH = 8;

#define TRANSPARANT_COLOR (0)

bool fullscreen = true;

// Redrawing constant
int REDRAWING_PERIOD = 40;

// Frames per second counter
float frames_per_sec = 0.0;
int frames_per_sec_tmp = 0;
int init_time = 0;

// Paths

#ifdef DREAMCAST
char *default_g_path = "/cd/graphics/original/";
char *default_s_path = "/cd/sound/original/";
#else
char *default_g_path = "graphics/original/";
char *default_s_path = "sound/original/";
#endif

char **g_paths = 0;
int n_g_paths = 0, act_g_path = 0;
char *g_path = 0;
char **s_paths = 0;
int n_s_paths = 0, act_s_path = 0;
char *s_path = 0;

extern int music_volume, sfx_volume;
extern int fighting_demon;

#ifndef _WIN32
struct timeval init_tick_count_value;

void setupTickCount()
{
	gettimeofday(&init_tick_count_value, NULL);
}

long GetTickCount()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval diff;
	diff.tv_sec = now.tv_sec - init_tick_count_value.tv_sec;
	diff.tv_usec = now.tv_usec - init_tick_count_value.tv_usec;
	if (diff.tv_usec < 0)
	{
		diff.tv_sec--;
		diff.tv_usec += 1000000;
	}
	return diff.tv_sec*1000 + diff.tv_usec/1000;
}
#endif

void pause(unsigned int time)
{
	unsigned int initt = GetTickCount();

	while((GetTickCount() - initt) < time);
} 


/* Pantalla: */ 

SDL_Surface *screen;

void Render(SDL_Surface *surface);
SDL_Surface* initializeSDL(int flags);
void finalizeSDL();

#ifdef _WIN32
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
#else
int main(int argc, char** argv)
{
	setupTickCount();
#endif

	int time, act_time;
	SDL_Event event;
    bool quit = false;

	time=init_time = GetTickCount();
	screen = initializeSDL((fullscreen ? SDL_FULLSCREEN : 0));

	if (screen == 0) {
		return 0;
	}

	GameInit(SCREEN_X, SCREEN_Y);

	SDL_FillRect(screen, NULL, 0);
	while (!quit) {
	
		while(SDL_PollEvent(&event)) {
            switch(event.type) {
			
                // grab keyboard events 
                case SDL_KEYDOWN:
#ifdef __APPLE__
                    // different quit shortcut on OSX: apple+Q
                    if (event.key.keysym.sym == SDLK_q) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_META) != 0) {
                            quit = true;
                        }
                    }
#endif
#ifdef _WIN32
                    // different quit shortcut on WIN32: ALT+F4
                    if (event.key.keysym.sym == SDLK_F4) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_ALT) != 0) {
                            quit = true;
                        }
                    }
#endif
                    // default quit: F12
                    if (event.key.keysym.sym == SDLK_F12) {
                        quit = true;
                    } /* if */
										

/*
FIXME: the code below is a big copy/paste; it should be in a separate function in stead
*/

#ifdef __APPLE__
                    if (event.key.keysym.sym == SDLK_f) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_META) != 0) {
							fullscreen = (fullscreen ? false : true);
							SDL_QuitSubSystem(SDL_INIT_VIDEO);
							SDL_InitSubSystem(SDL_INIT_VIDEO);
#ifdef DREAMCAST							
							SDL_DC_SetVideoDriver(SDL_DC_DMA_VIDEO);
#endif							
							if (SDL_WasInit(SDL_INIT_VIDEO)) {
								screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_HWPALETTE|(fullscreen ? SDL_FULLSCREEN : 0));
								
								if (screen == NULL) {
									fprintf(stderr, "Couldn't set %ix%ix%i", SCREEN_X, SCREEN_Y, COLOUR_DEPTH);
									if (fullscreen) {
										fprintf(stderr, ",fullscreen,");
									}
									fprintf(stderr," video mode: %s\n", SDL_GetError ());
									quit = true;
								} else {
									fprintf(stderr, "Set the video resolution to: %ix%ix%i",
										SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
										SDL_GetVideoSurface()->format->BitsPerPixel);
									if (fullscreen) {
										fprintf(stderr, ",fullscreen");
									}
									fprintf(stderr,"\n");
								}
												
								SDL_WM_SetCaption("Maze of Galious v0.63", 0);
								get_palette();
							} else {
								quit = true;
							}
                        } // if
                    } // if
#else
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
						if (IsAltPressed2()) {
							fullscreen = (fullscreen ? false : true);
							SDL_QuitSubSystem(SDL_INIT_VIDEO);
							SDL_InitSubSystem(SDL_INIT_VIDEO);
							
							if (SDL_WasInit(SDL_INIT_VIDEO)) {
								screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_HWPALETTE|(fullscreen ? SDL_FULLSCREEN : 0));
								
								if (screen == NULL) {
									fprintf(stderr, "Couldn't set %ix%ix%i", SCREEN_X, SCREEN_Y, COLOUR_DEPTH);
									if (fullscreen) {
										fprintf(stderr, ",fullscreen,");
									}
									fprintf(stderr," video mode: %s\n", SDL_GetError ());
									quit = true;
								} else {
									fprintf(stderr, "Set the video resolution to: %ix%ix%i",
										SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
										SDL_GetVideoSurface()->format->BitsPerPixel);
									if (fullscreen) {
										fprintf(stderr, ",fullscreen");
									}
									fprintf(stderr,"\n");
								}
												
								SDL_WM_SetCaption("Maze of Galious v0.63", 0);
								get_palette();
							} else {
								quit = true;
							}
                        }
                    }
#endif
				
					// Change graphic set with either F10 or 9 (F10 is already used in OSX)
					if (event.key.keysym.sym == SDLK_F10 || event.key.keysym.sym == SDLK_9) {

						act_g_path++;
						if (act_g_path >= n_g_paths) {
							act_g_path = 0;
						}
						
						g_path = g_paths[act_g_path];
						ReleaseGraphics();
						ReloadGraphics(SCREEN_X, SCREEN_Y);
						guardar_configuracion("MoG.cfg");
						
						if (fighting_demon != 0) {
							redo_demonintro(fighting_demon, 0, SCREEN_X, SCREEN_Y);
						}
					} 

					// Change sound set with either F11 or 0 (F10 is already used in OSX)
					if (event.key.keysym.sym == SDLK_F11 || event.key.keysym.sym == SDLK_0) {

						act_s_path++;
						if (act_s_path >= n_s_paths) {
							act_s_path = 0;
						}
						
						s_path = s_paths[act_s_path];
						ReleaseSound(false);
						ReloadSound();
						music_recovery();
						guardar_configuracion("MoG.cfg");
						Mix_VolumeMusic(music_volume);
						SetSFXVolume(sfx_volume);
					}

					if (event.key.keysym.sym == SDLK_d) {
						write_debug_report("debug-report.txt");
					} 
                    break;

                // quit game and close window 
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }
		
		// game main loop
		act_time = SDL_GetTicks();
        if (!quit && act_time - time >= REDRAWING_PERIOD) {
            int max_frame_step = 10;
            do {
                time += REDRAWING_PERIOD;
                if ((act_time - time) > 10 * REDRAWING_PERIOD) {
                    time = act_time;
				}

	
				// render graphics
				Render(screen);
				SDL_Flip(screen);
				
				// handle music
				// we don't use the SDL_mixer hook anymore because this is easier
				PlayAudioQueue();

                act_time = SDL_GetTicks();
                max_frame_step--;
            } while (act_time - time >= REDRAWING_PERIOD && max_frame_step > 0);

        }
        if ((act_time - init_time) >= 1000) {
            frames_per_sec = frames_per_sec_tmp;
            frames_per_sec_tmp = 0;
            init_time = act_time;
        }
		
		SDL_Delay(1);		
	}

	finalizeSDL();

	return 0;
}

SDL_Surface* initializeSDL(int moreflags)
{
	char VideoName[256];
	SDL_Surface *screen;

	int flags = SDL_HWPALETTE|moreflags;

	
#ifdef DREAMCAST   
       if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK) < 0) {
		return 0;
	} 

    SDL_JoystickEventState(SDL_ENABLE);
	SDL_JoystickOpen(0);
	SDL_JoystickOpen(1);
	
#else
     if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
		return 0;
	}
#endif
	fprintf(stderr, "Initializing SDL video subsystem.\n");

	if ((SDL_Init(SDL_INIT_VIDEO)) == -1) {
		fprintf(stderr, "Couldn't initialize video subsystem: %s\n", SDL_GetError());
		exit(-1);
	}

	SDL_VideoDriverName(VideoName, sizeof (VideoName));

    fprintf (stderr, "SDL driver used: %s\n", VideoName);
    // Set the environment variable SDL_VIDEODRIVER to override
    // For Linux: x11 (default), dga, fbcon, directfb, svgalib,
    //            ggi, aalib
    // For Windows: directx (default), windib
	fprintf(stderr, "SDL video subsystem initialized.\n");

	fprintf(stderr, "Initializing SDL audio subsystem.\n");

	if ((SDL_InitSubSystem(SDL_INIT_AUDIO)) == -1) {
	  fprintf(stderr, "Couldn't initialize audio subsystem: %s\n", SDL_GetError());
	  exit(-1);
	}
	fprintf (stderr, "SDL audio subsystem initialized.\n");


	atexit(SDL_Quit);
	SDL_WM_SetCaption("Maze of Galious v0.63", 0);
	
	if (fullscreen) {
		SDL_ShowCursor(SDL_DISABLE);
	}
	
	Sound_initialization();

	pause(1000);

	screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, flags);

	if (screen == NULL) {
		fprintf(stderr, "Couldn't set %ix%ix%i", SCREEN_X, SCREEN_Y, COLOUR_DEPTH);
	    if (fullscreen) {
			fprintf(stderr, ",fullscreen,");
	    }
		fprintf(stderr," video mode: %s\n", SDL_GetError ());
	    exit(-1);
	} else {
	    fprintf(stderr, "Set the video resolution to: %ix%ix%i",
				 SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
				 SDL_GetVideoSurface()->format->BitsPerPixel);
	    if (fullscreen) {
			fprintf(stderr, ",fullscreen");
		}
	    fprintf(stderr,"\n");
    }
	
	SDL_EnableUNICODE(1);
	
	return screen;
}

void finalizeSDL()
{
	ReleaseSound(true);
	ReleaseGraphics();
	Sound_release();
	SDL_Quit();
}
