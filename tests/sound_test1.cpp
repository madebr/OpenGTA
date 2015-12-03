#include <SDL.h>
#include <SDL_sound.h>
#include <SDL_mixer.h>
#include <string>

#include <physfs.h>
#include "physfsrwops.h"
#include <assert.h>
//#include "wavestream2.h"

class SoundDevice {
  public:
    SoundDevice();
    ~SoundDevice();
    void close();
    void open();
    void open(int r, Uint16 f, int c, int bs);
    const char* getCardName() { return cardName; }
  private:
    char   cardName[50];
    int    rate;
    int    channels;
    int    bufSize;
    Uint16 format;
    int    _status;
};


SoundDevice::SoundDevice() {
  memset(cardName, 0, sizeof(cardName));
  rate = 44100;
  channels = 2;
  bufSize = 4096;
  format = AUDIO_S16;
  _status = 0;
}

SoundDevice::~SoundDevice() {
  Mix_HaltGroup(-1);
  Mix_HaltMusic();
  close();
}

void SoundDevice::close() {
  if (!_status)
    return;
  Mix_CloseAudio();
}

void SoundDevice::open() {
  if (_status)
    close();
  if (Mix_OpenAudio(rate, format, channels, bufSize))
    throw std::string(SDL_GetError());
  SDL_AudioDriverName (cardName, sizeof (cardName));
  Mix_QuerySpec(&rate, &format, &channels);
  fprintf (stderr, "opened %s at %d Hz %d bit %s (%i), %d bytes audio buffer\n",
			 cardName, rate, format & 0xFF,
			 channels > 1 ? "stereo" : "mono", channels, bufSize);
  
  _status = 1;
}

void SoundDevice::open(int r, Uint16 f, int c, int bs) {
  rate = r;
  format = f;
  channels = c;
  bufSize = bs;
  open();
}

#if 0
void myMusicPlayer(void *udata, Uint8 *stream, int len) {
  int i,act=0;
  Sint16 *ptr2;

  if (stream == 0)
    throw std::string("Zero music stream :-(");

  ptr2=(Sint16 *)stream;
  if (playing_music) {
    while(act<len) {
      if (music_sound!=0) {
        /* Play a music file: */ 

        if ((music_sound->flags&SOUND_SAMPLEFLAG_EOF)) {
          /* End of file: */ 
          if (music_loops!=-1) {
            current_music_loop++;
            if (current_music_loop>music_loops) {
              playing_music=false;
              if (music_sound!=0) Sound_FreeSample(music_sound);                                                      

              music_sound=0;
            } else {
              Sound_Rewind(music_sound);
            } /* if */ 
          } else {
            Sound_Rewind(music_sound);
          } /* if */ 
        } else {
          /* In the middle of the file: */ 
          int decoded=0;
          Sint16 *ptr;

          Sound_SetBufferSize(music_sound, len-act);

          decoded=Sound_Decode(music_sound);
          ptr=(Sint16 *)music_sound->buffer;
          for(i=0;i<decoded;i+=2,ptr++,ptr2++) {
            *ptr2=((Sint32(*ptr)*Sint32(music_volume))/127);
          } /* for */ 
          act+=decoded;
        } /* if */ 
      } else {
        /* No music file loaded: */ 
        for(i=act;i<len;i++) stream[i]=0;
        act=len;
      } /* if */ 
    } /* while */ 
  } else {
    /* No music to play: */ 
    for(i=0;i<len;i++) stream[i]=0;
  } /* if */ 
} /* myMusicPlayer */
#endif

#include "fx_sdt.h"
#include "sound_resample.h"

int main(int argc, char *argv[])
{
  PHYSFS_init(argv[0]);
  PHYSFS_addToSearchPath("gtadata.zip", 1);

  Uint32 sdl_init_flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO;
  if (SDL_Init(sdl_init_flags) == -1)
  {
    fprintf(stderr, "SDL_Init() failed!\n"
        "  reason: [%s].\n", SDL_GetError());
    return(42);
  }
  SoundDevice dev;
  dev.open();

  OpenGTA::SoundsDB db(argv[1]);
  OpenGTA::SoundsDB::Entry & e = db.getEntry(atoi(argv[2]));
  unsigned char* mem = db.getBuffered(atoi(argv[2]));
  size_t si;
  Uint8 *mem2 = (Uint8*)resample_new(mem, e.rawSize, si, e.sampleRate, 44100);

  //write(1, mem, e.rawSize+ 36 + 8);

  //SDL_RWops *handle = PHYSFSRWOPS_openRead(argv[1]);
  //SDL_RWops * handle = SDL_RWFromMem(mem, e.rawSize + 36 + 8);
  //assert(handle);
  //Mix_Chunk * music = Mix_LoadWAV_RW(handle, 1); 
  Mix_Chunk * music = Mix_QuickLoad_RAW(mem2, si); 
  if (!music) {
    fprintf(stderr, "Error loading music: %s\n", SDL_GetError());
  }
  delete [] mem;
  Mix_PlayChannel(0,music,0);

  while (Mix_Playing(0)) {
    SDL_Delay(1000);
  }
  delete [] mem2;
  Mix_HaltMusic();
  Mix_FreeChunk(music);
  //music = NULL;
  SDL_Quit();
  return(0);
}
