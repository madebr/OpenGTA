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
#include "dataholder.h"
#include "log.h"
#include "cistring.h"

namespace OpenGTA {
  template<> ActiveStyle::ActiveData() {
    m_data = 0;
  }

  template<> ActiveStyle::~ActiveData() {
    unload();
  }

  template<> GraphicsBase & ActiveStyle::get() {
    assert(m_data);
    return *m_data;
  }

  template<class T> void ActiveData<T>::unload() {
    if (m_data)
      delete m_data;
    m_data = 0;
  }
  
  template<> void ActiveStyle::load(const std::string & file) {
    unload();
    Util::ci_string tmpname(file.c_str());
    if (tmpname.find(".g24") != std::string::npos) {
      m_data = new Graphics24Bit(file);
    }
    else if (tmpname.find(".gry") != std::string::npos) {
      m_data = new Graphics8Bit(file);
    }
    else {
      try {
        m_data = new Graphics8Bit(file);
      }
      catch (const Exception & e) {
        INFO << "loading 8 bit failed: " << e.what() << std::endl;
        m_data = 0;
        try {
          m_data = new Graphics24Bit(file);
        }
        catch (const Exception & e) {
          ERROR << "loading 24 bit failed " << e.what() << std::endl;
          m_data = 0;
        }
      }
    }
    assert(m_data);
  }

  template<> ActiveMap::ActiveData() {
    m_data = 0;
  }

  template<> ActiveMap::~ActiveData() {
    unload();
  }
  
  template<> Map & ActiveMap::get() {
    assert(m_data);
    return *m_data;
  }
  
  template<> void ActiveMap::load(const std::string & file) {
    unload();
    try {
      m_data = new Map(file); 
    }
    catch (const Exception & e) {
      ERROR << "loading map failed: " << e.what();
      m_data = 0;
    }
    assert(m_data);
  }

}
