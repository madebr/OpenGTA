#include <iostream>
#include <iomanip>
#include "sound_system.h"
#include "physfsrwops.h"
#include "m_exceptions.h"
#include "log.h"

namespace Audio {

#ifdef WITH_SOUND
  SoundSystem::SoundSystem() : device(), chunkCache() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    device.open();
    enabled = true;
    if (device.getStatus() == SoundDevice::OPEN)
      Sound_Init();
    else {
      WARN << "Could not open audio-device - disabling sound!" << std::endl;
      enabled = false;
    }
  }
  
  SoundSystem::~SoundSystem() {
    device.close();
    Sound_Quit();
  }

  void SoundSystem::playFx(std::string file, size_t idx) {
    if (!enabled)
      return;
    ChunkData & cd = chunkCache.getChunk(file, idx);
    Mix_PlayChannel(0, cd.chunk, 0);
  }

  void SoundSystem::playMusic(std::string file) {
    if (!enabled)
      return;
    Sound_AudioInfo inf;
    inf.format = device.getFormat();
    inf.channels = device.getNumChannels();
    inf.rate = device.getRate();

    MusicPlayerCtrl::clear();

    SDL_RWops * rw = PHYSFSRWOPS_openRead(file.c_str());
    if (!rw)
      throw E_FILENOTFOUND(file);

    size_t pos = file.rfind('.');
    std::string ext(file.substr(pos + 1, pos - 1));

    MusicPlayerCtrl::music = Sound_NewSample(rw, ext.c_str(), &inf, device.getBufferSize());

    if (!MusicPlayerCtrl::music)
      throw E_NOTSUPPORTED("file: " + file + " - " + Sound_GetError());

    Mix_HookMusic(musicPlayerFunc, 0);
    MusicPlayerCtrl::isPlaying = true;
  }

  void SoundSystem::listMusicDecoders() {
    std::cout << "* Supported music decoders *" << std::endl;
    const Sound_DecoderInfo **i;
    for (i = Sound_AvailableDecoders(); *i != NULL; i++) {
      std::cout << std::setfill(' ') << std::setw(5) <<
      (*i)->extensions[0] << " : " << (*i)->description  << std::endl;
    }
  }
#else
  SoundSystem::SoundSystem() : enabled(false) {
  }

  SoundSystem::~SoundSystem() {}

  void SoundSystem::playFx(std::string file, size_t idx) {}
  void SoundSystem::playMusic(std::string file) {}
  void SoundSystem::listMusicDecoders() {}
#endif

  void CB_MusicDone() {
    std::cout << "music finished" << std::endl;
  }
}

#ifdef SOUND_TEST
using namespace Audio;
int main(int argc, char* argv[]) {
  PHYSFS_init(argv[0]);
  PHYSFS_addToSearchPath("gtadata.zip", 1);
  PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);

  SoundSystem noisemaker;
  noisemaker.listMusicDecoders();
  noisemaker.playMusic(argv[1]);
  MusicPlayerCtrl::musicFinishedCB = CB_MusicDone;
  while (MusicPlayerCtrl::isPlaying) {
    SDL_Delay(1000);
  }
}
#endif
