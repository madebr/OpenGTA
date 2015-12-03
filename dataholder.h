/************************************************************************
* Copyright (c) 2005-2006 tok@openlinux.org.uk                          *
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
#ifndef STYLE_HOLDER_H
#define STYLE_HOLDER_H
#include <string>
#include "opengta.h"
#include "m_exceptions.h"
#include "Singleton.h"

namespace OpenGTA {
/*
  class ActiveStyle {
    public:
      ActiveStyle();
      ~ActiveStyle();
      GraphicsBase & getStyle();
      void load(const std::string & file);
    private:
      void unload();
      GraphicsBase* m_style;
  };
*/
  template <class T>
    class ActiveData {
      public:
        ActiveData();
        ~ActiveData();
        T & get();
        void load(const std::string & file);
      private:
        void unload();
        T* m_data;
    };

  typedef ActiveData< GraphicsBase > ActiveStyle;
  typedef ActiveData< Map > ActiveMap;

  typedef Loki::SingletonHolder< ActiveStyle, Loki::CreateUsingNew, Loki::DefaultLifetime,
          Loki::SingleThreaded> StyleHolder;
  typedef Loki::SingletonHolder< ActiveMap, Loki::CreateUsingNew, Loki::DefaultLifetime,
          Loki::SingleThreaded> MapHolder;
}

#endif
