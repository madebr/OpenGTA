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
#ifdef WITH_SOUND
#include <string>
#include "m_exceptions.h"
#include "sound_music_player.h"

namespace Audio {
  bool MusicPlayerCtrl::isPlaying = false;
  Sound_Sample * MusicPlayerCtrl::music = 0;
  MusicFinishedCallbackType MusicPlayerCtrl::musicFinishedCB;

  void MusicPlayerCtrl::clear() {
    if (MusicPlayerCtrl::isPlaying)
      MusicPlayerCtrl::isPlaying = false;
    if (MusicPlayerCtrl::music) {
      Sound_FreeSample(MusicPlayerCtrl::music);
        MusicPlayerCtrl::music = 0;
    }
    if (MusicPlayerCtrl::musicFinishedCB)
      MusicPlayerCtrl::musicFinishedCB();
  }

  // taken from an example on the web, can't remember where...
  // slightly simplified: removed looping
  void musicPlayerFunc(void *udata, Uint8 *stream, int len) {
    int i,act=0;
    Sint16 *ptr2;

    if (stream == 0)
      throw E_INVALIDFORMAT("Encountered a zero music stream while playing :-(");

    ptr2=(Sint16 *)stream;
    if (MusicPlayerCtrl::isPlaying) {
      while (act < len) {
        if (MusicPlayerCtrl::music != 0) {
          if ((MusicPlayerCtrl::music->flags&SOUND_SAMPLEFLAG_EOF)) {
            MusicPlayerCtrl::clear();
          }
          else {
            /* In the middle of the file: */ 
            int decoded=0;
            Sint16 *ptr;

            Sound_SetBufferSize(MusicPlayerCtrl::music, len-act);

            decoded=Sound_Decode(MusicPlayerCtrl::music);
            ptr=(Sint16 *)MusicPlayerCtrl::music->buffer;
            for(i=0;i<decoded;i+=2,ptr++,ptr2++) {
              *ptr2=((Sint32(*ptr)*Sint32(music_volume))/127);
            } /* for */ 
            act+=decoded;

          }
        }
        else {
          for(i=act;i<len;i++) stream[i]=0;
          act=len;
        }
      }
    }
    else { // not playing; fill with zero
      for(i=0;i<len;i++) stream[i]=0;
    }
  }
}

#endif
