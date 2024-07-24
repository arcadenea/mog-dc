#ifdef DREAMCAST
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#else
#include "SDL.h"
#include "SDL_mixer.h"
#endif
#include "sound.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "MOGtypes.h"
#include "filehandling.h"

#define AUDIO_BUFFER	1024


/* Paths: */ 
extern char *default_g_path;
extern char *default_s_path;
extern char **g_paths;
extern int n_g_paths, act_g_path;
extern char *g_path;
extern char **s_paths;
extern int n_s_paths, act_s_path;
extern char *s_path;
extern int music_volume;

bool sound_enabled = false;
bool music_played = false;

int music_position = 0;
bool playing_music = false;
bool music_loaded[8] = {false, false, false, false, false, false, false, false};			
Mix_Music *music_sound[8] = {0, 0, 0, 0, 0, 0, 0, 0};
char music_files[8][128];
char music_realfiles[8][128];

void myMusicPlayer(void *udata, Uint8 *stream, int len);

bool Sound_initialization(void)
{
    char SoundcardName[256];
	int audio_rate = 44100;
	int audio_channels = 1;
	int audio_bufsize = AUDIO_BUFFER;
	Uint16 audio_format = AUDIO_S16;
	SDL_version compile_version;

	sound_enabled = true;
	fprintf(stderr, "Initializing SDL_mixer.\n");
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_bufsize))  {
	  fprintf(stderr, "Unable to open audio: %s\n", Mix_GetError());
	  sound_enabled = false;
	  fprintf(stderr, "Running the game without audio.\n");
	  return false;	
	}
	SDL_AudioDriverName(SoundcardName, sizeof (SoundcardName));
	Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
	fprintf(stderr, "    opened %s at %d Hz %d bit %s, %d bytes audio buffer\n",
			 SoundcardName, audio_rate, audio_format & 0xFF,
			 audio_channels > 1 ? "stereo" : "mono", audio_bufsize);
	MIX_VERSION(&compile_version);
	fprintf(stderr, "    compiled with SDL_mixer version: %d.%d.%d\n",
			 compile_version.major,
			 compile_version.minor,
			 compile_version.patch);
	fprintf(stderr, "    running with SDL_mixer version: %d.%d.%d\n",
			 Mix_Linked_Version()->major,
			 Mix_Linked_Version()->minor,
			 Mix_Linked_Version()->patch);

	return true;
}

// Close down audio device and disable sound
void Sound_release(void)
{
	if (sound_enabled) {
		Mix_CloseAudio();
	} 
	sound_enabled = false;
}

// a check to see if file is readable and greater than zero
int file_check(char *fname)
{
	FILE *fp;

	if ((fp=f1open(fname,"r",GAMEDATA))!=NULL) {
		if (fseek(fp,0L, SEEK_END) == 0 && ftell(fp) > 0) {
  			fclose(fp);
			return true;
		} 
		 
		/* either the file could not be read (==-1) or size was zero (==0) */ 
		fprintf(stderr, "ERROR in file_check(): the file %s is corrupted.\n", fname);
		fclose(fp);
		exit(1);
	} 
	return false;
}

// load sound effects
SOUNDT Sound_create_sound(char *file,int flags)
{
	int n_ext = 6;
	char *ext[6] = {".WAV", ".OGG", ".MP3", ".wav", ".ogg", ".mp3"};
	char name[256], name2[256];
	int i;

	if (sound_enabled) {

		for(i = 0; i < n_ext; i++) {

			strcpy(name, file);
			strcat(name, ext[i]);
			sprintf(name2, "%s%s", s_path,name);

			if (file_check(name2)) {
				return Mix_LoadWAV(name2);
			}
		}

		for(i = 0; i < n_ext; i++) {
		
			strcpy(name, file);
			strcat(name, ext[i]);
			sprintf(name2, "%s%s", default_s_path, name);
			
			if (file_check(name2)) {
				return Mix_LoadWAV(name2);
			}
		}
		
		fprintf(stderr, "ERROR in Sound_create_sound(): Could not load sound file: %s%s.(wav|ogg|mp3)\n", s_path, file);
		exit(1);
	} else {
		return 0;
	}
}

// unload sound effects
void Delete_sound(SOUNDT s)
{
	if (sound_enabled) {
		Mix_FreeChunk(s);
	}
}

// play sound effect
void Sound_play(SOUNDT s)
{
	if (sound_enabled) {
		Mix_PlayChannel(-1, s, 0);
	}
}

// determine filename for a sound set
void Sound_obtain_file_name(char *file, char *fullname)
{
	int n_ext = 6;
	char *ext[6] = {".WAV", ".OGG", ".MP3", ".wav", ".ogg", ".mp3"};
	char name[256], name2[256];
	int i;

	for(i = 0; i < n_ext; i++) {
	
		strcpy(name, file);
		strcat(name, ext[i]);
		sprintf(name2, "%s%s", s_path,name);

		if (file_check(name2)) {
			strcpy(fullname, name2);
			return;
		} 
		
		strcpy(name, file);
		strcat(name, ext[i]);
		sprintf(name2, "%s%s", default_s_path, name);
		
		if (file_check(name2)) {
			strcpy(fullname, name2);
			return;
		}
	}

	fprintf(stderr, "ERROR in Sound_obtain_file_name(): Could not find sound file: %s%s.(wav|ogg|mp3)\n", s_path, file);
 	fullname[0] = 0;
	exit(1);
}

// load sound effect
Mix_Chunk *Sound_create_stream(char *file)
{
	int n_ext = 6;
	char *ext[6] = {".WAV", ".OGG", ".MP3", ".wav", ".ogg", ".mp3"};
	char name[256], name2[256];
	int i;

	if (sound_enabled) {

		for(i = 0; i < n_ext; i++) {

			strcpy(name, file);
			strcat(name, ext[i]);
			sprintf(name2, "%s%s", s_path, name);

			if (file_check(name2)) {
				return Mix_LoadWAV(name2);
			}
		}
		
		for(i = 0; i < n_ext; i++) {
		
			strcpy(name, file);
			strcat(name, ext[i]);
			sprintf(name2, "%s%s", default_s_path, name);
		
			if (file_check(name2)) {
				return Mix_LoadWAV(name2);
			}
		}
		
		fprintf(stderr, "ERROR in Sound_create_stream(): Could not load sound file: %s%s.(wav|ogg|mp3)\n", s_path, file);
		exit(1);
	} else {
		return 0;
	} 
}

// load musics
// queue up to 3 music files that will be played in sequence
void Sound_create_music(char *f1, char *f2, char *f3)
{
	char tmp[128];
	int seq_len = 0;

	if (sound_enabled) {
	
		if (f1 != 0) {
			Sound_obtain_file_name(f1, tmp);
			strcpy(music_files[0], f1);
			strcpy(music_realfiles[0], tmp);
			music_loaded[0] = true;
			music_sound[0] = Mix_LoadMUS(tmp);
			seq_len = 1;
		} else {
			music_files[0][0] = 0;
			music_loaded[0] = false;
		} 
		 
		if (f2 != 0) {
			Sound_obtain_file_name(f2, tmp);
			strcpy(music_files[1], f2);
			strcpy(music_realfiles[1], tmp);
			music_loaded[1] = true;
			music_sound[1] = Mix_LoadMUS(tmp);
			seq_len = 2;
		} else {
			music_files[1][0] = 0;
			music_loaded[1] = false;
		}
		
		if (f3 != 0) {
			Sound_obtain_file_name(f3, tmp);
			strcpy(music_files[2], f3);
			strcpy(music_realfiles[2], tmp);
			music_loaded[2] = true;
			music_sound[2] = Mix_LoadMUS(tmp);
			seq_len=3;
		} else {
			music_files[2][0] = 0;
			music_loaded[2] = false;
		}
		
		music_position = 0;
		playing_music = true;
		music_played = false;
	}
}

void Sound_subst_music(char *f)
{
	char tmp[128];

	Sound_obtain_file_name(f, tmp);
}

void Sound_subst_music_now(char *f)
{
	Sound_release_music();
	Sound_create_music(f, 0, 0);
} 

void Sound_release_music(void)
{
	int i;

	if (sound_enabled) {
		playing_music = false;
		for(i = 0; i < 3; i++) {
			if (music_loaded[i]) {
				Mix_FreeMusic(music_sound[i]);
			}
			music_loaded[i] = false;
		}
	}
}

void Sound_temporary_release_music(void)
{
	int i;

	if (sound_enabled) {
		playing_music = false;
		for(i = 0; i < 3 ; i++) {
			if (music_loaded[i]) {
				Mix_FreeMusic(music_sound[i]);
			}
		} 
	}
}

void Sound_pause_music(void)
{
	playing_music = false;
}

void Sound_unpause_music(void)
{
	music_played = false;
	playing_music = true;
}

void music_recovery(void)
{	
	char tmp[128];
	int seq_len = 0;

	if (sound_enabled) {

		if (music_loaded[0]) { 
			music_loaded[0] = true;
			Sound_obtain_file_name(music_files[0], tmp);
			strcpy(music_realfiles[0], tmp);
			music_sound[0] = Mix_LoadMUS(music_files[0]);
			seq_len = 1;
		}
		
		if (music_loaded[1]) { 
			music_loaded[1] = true;
			Sound_obtain_file_name(music_files[1], tmp);
			strcpy(music_realfiles[1], tmp);
			music_sound[1] = Mix_LoadMUS(music_files[1]);
			seq_len = 2;
		}

		if (music_loaded[2]) { 
			music_loaded[2] = true;
			Sound_obtain_file_name(music_files[2], tmp);
			strcpy(music_realfiles[2], tmp);
			music_sound[2] = Mix_LoadMUS(music_files[2]);
			seq_len = 3;
		}
		
		playing_music = true;
		music_played = false;
	}
}

void PlayAudioQueue() 
{
	if (playing_music) {

		// if we have a next song in the queue we don't loop the current song
		int loop = 0; 

		if (music_loaded[music_position]) {
			if (!Mix_PlayingMusic()) {
						
				// new call from Sound_create_music(); handle playback differently
				if (music_played == false) {
					loop = (music_loaded[music_position + 1]) ? 0 : -1;
					Mix_PlayMusic(music_sound[music_position], loop);
					music_played = true;
				// start next song
				} else {
					music_position++;
					loop = (music_loaded[music_position + 1]) ? 0 : -1;
					Mix_PlayMusic(music_sound[music_position], loop);
					printf("playing 2 %s\n", music_realfiles[music_position]);
				}
			}
		}
	}
}
