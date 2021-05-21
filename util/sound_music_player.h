/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
*                                                                       *
* This software is provided as-is, without any express or implied       *
* warranty. In no event will the authors be held liable for any         *
* damages arising from the use of this software.                        *
*                                                                       *
* Permission is granted to anyone to use this software for any purpose, *
* including commercial applications, and to alter it and redistribute   *
* it freely, subject to the following restrictions:                     *
*                                                                       *
* 1. The origin of this software must not be misrepresented; you must   *
* not claim that you wrote the original software. If you use this       *
* software in a product, an acknowledgment in the product documentation *
* would be appreciated but is not required.                             *
*                                                                       *
* 2. Altered source versions must be plainly marked as such, and must   *
* not be misrepresented as being the original software.                 *
*                                                                       *
* 3. This notice may not be removed or altered from any source          *
* distribution.                                                         *
************************************************************************/
#ifndef SOUND_MUSIC_PLAYER_H
#define SOUND_MUSIC_PLAYER_H
#ifdef WITH_SOUND

#include <SDL_sound.h>

#include <functional>

namespace Audio {
  static int music_volume = 127;
  using MusicFinishedCallbackType = std::function<void()>;

  struct MusicPlayerCtrl {
    static bool         isPlaying;
    static Sound_Sample *music;
    static void clear();
    static MusicFinishedCallbackType musicFinishedCB;
  };
  void musicPlayerFunc(void *udata, Uint8 *stream, int len);
}
#endif

#endif
