#ifndef OPENGTA_SCRIPT_CV_H
#define OPENGTA_SCRIPT_CV_H

#include "gl_cityview.h"
#include "lunar.h"

namespace OpenGTA {
  namespace Script {
    class CityView : public OpenGTA::CityView {
      public:
        int setCamPosition(lua_State *L);
        int getCamPosition(lua_State *L);

//        int setCamVector(lua_State *L);
//        int getCamVector(lua_State *L);
        int setTopDownView(lua_State *L);

        int setZoom(lua_State *L);
        int getZoom(lua_State *L);
        
        int setVisibleRange(lua_State *L);
        int getVisibleRange(lua_State *L);
        int setDrawHeadingArrows(lua_State *L);

        // --
        
        static const char className[];
        static Lunar<CityView>::RegType methods[];
    };
  }
}
#endif
