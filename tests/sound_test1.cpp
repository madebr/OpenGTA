#include <SDL.h>
#define USE_RWOPS
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

int music_volume=127;

bool sound_enabled=false;

bool playing_music=false;
Sound_Sample *music_sound=0;
int music_loops=1;
int current_music_loop=0;

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

#include "fx_sdt.h"
#include "sound_resample2.h"

class AudioChunkCache {
  public:
    struct ChunkId {
      std::string src_file;
      size_t      idx_in_file;
      ChunkId(const std::string & file, const size_t idx) :
        src_file(file), idx_in_file(idx) {}
      bool operator == (const ChunkId & o) const {
        return (idx_in_file == o.idx_in_file && src_file == o.src_file);
      }
      bool operator < (const ChunkId & o) const {
        if (idx_in_file < o.idx_in_file)
          return true;
        if (idx_in_file > o.idx_in_file)
          return false;
        return src_file < o.src_file;
      }
    };
    struct ChunkData {
      Uint8 * mem_buf;
      Mix_Chunk * chunk;
      size_t ref;
      ChunkData(Uint8 * m, Mix_Chunk * c, size_t r = 1) :
        mem_buf(m), chunk(c), ref(r) {}
      ChunkData(const ChunkData & o) :
        mem_buf(o.mem_buf), chunk(o.chunk), ref(o.ref) {}
    };
    typedef std::map< ChunkId, ChunkData > CacheType;
    CacheType cached;
    ChunkData & getChunk(std::string & file, size_t idx);
    void prepareDB(std::string db_file);
    OpenGTA::SoundsDB * getDB(std::string db_file);
  private:
    ChunkData loadChunk(std::string & file, size_t idx);
    typedef std::map< std::string, OpenGTA::SoundsDB* > LookupCache;
    LookupCache lookup;

};

AudioChunkCache::ChunkData & AudioChunkCache::getChunk(std::string & file, size_t idx) {
  ChunkId id(file, idx);
  CacheType::iterator i = cached.find(id);
  if (i == cached.end()) {
    ChunkData c = loadChunk(file, idx);
    cached.insert(std::make_pair<ChunkId, ChunkData>(id, c));
    i = cached.find(id);
  }
  return i->second;
}

AudioChunkCache::ChunkData AudioChunkCache::loadChunk(std::string & file, size_t idx) {
  LookupCache::iterator j = lookup.find(file);
  if (j == lookup.end()) {
    prepareDB(file);
    j = lookup.find(file);
  }
  OpenGTA::SoundsDB & db = *j->second;
  OpenGTA::SoundsDB::Entry & e = db.getEntry(idx);
  size_t si;
  unsigned char* mem = db.getBuffered(idx);
  Uint8 *mem2 = (Uint8*)Audio::resample_new(mem, e.rawSize, si, e.sampleRate, 44100);
  Mix_Chunk * music = Mix_QuickLoad_RAW(mem2, si);
  return ChunkData(mem2, music, 1);
}

void AudioChunkCache::prepareDB(std::string db) {
  lookup[db] = new OpenGTA::SoundsDB(db);
}

int main(int argc, char *argv[])
{
  PHYSFS_init(argv[0]);
  PHYSFS_mount("gtadata.zip", nullptr, 1);
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);

  Uint32 sdl_init_flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO;
  if (SDL_Init(sdl_init_flags) == -1)
  {
    fprintf(stderr, "SDL_Init() failed!\n"
        "  reason: [%s].\n", SDL_GetError());
    return(42);
  }
  SoundDevice dev;
  dev.open();

  if (argc == 3) {
  AudioChunkCache acc;
  std::string foo(argv[1]);
  AudioChunkCache::ChunkData & cd = acc.getChunk(foo, atoi(argv[2]));
  /*
  OpenGTA::SoundsDB db(argv[1]);
  OpenGTA::SoundsDB::Entry & e = db.getEntry(atoi(argv[2]));
  unsigned char* mem = db.getBuffered(atoi(argv[2]));
  size_t si;
  Uint8 *mem2 = (Uint8*)Audio::resample_new(mem, e.rawSize, si, e.sampleRate, 44100);

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
  */

  Mix_PlayChannel(0, cd.chunk, 0);
  }
  else {
    /*
    SDL_RWops * rw = PHYSFSRWOPS_openRead(argv[1]);
    assert(rw);
    Mix_Music * music = Mix_LoadMUS_RW(rw);
    assert(music);
    Mix_PlayMusic(music, 0);
    */
     Sound_Init();
     Sound_AudioInfo inf;
#define AUDIO_BUFFER   4096 
        inf.format=AUDIO_S16;
        inf.channels=2;
        inf.rate=44100;

      SDL_RWops * rw = PHYSFSRWOPS_openRead(argv[1]);

      assert(rw);
     music_sound = Sound_NewSample(rw, "mp3" ,&inf,AUDIO_BUFFER);
     assert(music_sound);
     Mix_HookMusic(myMusicPlayer, 0);
     playing_music = true;
  }
  //while (Mix_Playing(0)) {
  while (playing_music) {
    SDL_Delay(1000);
  }
  //delete [] mem2;
  Mix_HaltMusic();
  //Mix_FreeChunk(music);
  //music = NULL;
  SDL_Quit();
  return(0);
}
