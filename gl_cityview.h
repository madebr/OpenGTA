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
#ifndef GL_CITYVIEW_H
#define GL_CITYVIEW_H

#include "opengta.h"
#include "navdata.h"
#include "gl_texturecache.h"
#include "gl_frustum.h"
#include "gl_pagedtexture.h"

namespace OpenGTA {

  class BlockAnimCtrl;
  class CityView {
    public:
      CityView();
      ~CityView();
      void loadMap(const std::string &map, const std::string &style);
      void createLevelObject(OpenGTA::Map::ObjectPosition *obj);
      void setPosition(const GLfloat & x, const GLfloat & y, const GLfloat & z);
      void setTopDownView(const GLfloat & height);
      //void setCamVector(const GLfloat & x, const GLfloat & y, const GLfloat & z);
      void setZoom(const GLfloat zoom);
      void setViewMode(bool topDown);
      bool getViewMode() { return topDownView; }
      void setDrawHeadingArrows(bool yes) { drawHeadingMarkers = yes; }
      void setTexFlipTest(int v) { texFlipTest = v; }
      GLfloat* getCamPos() { return (GLfloat*)&camPos; }
      void setVisibleRange(int);
      int  getVisibleRange();
      void getTerrainHeight(GLfloat & x, GLfloat & y, GLfloat & z);
      void draw(Uint32 ticks);
      NavData::Sector* getCurrentSector();
      OpenGL::PagedTexture renderMap2Texture();

      bool CityView::getDrawTextured();
      bool CityView::getDrawLines();
      void CityView::setDrawTextured(bool v);
      void CityView::setDrawLines(bool v);

      void resetTextures();
      const SDL_Rect & getActiveRect() { return activeRect; }
      const SDL_Rect & getOnScreenRect() { return drawnRect; }
      BlockAnimCtrl* blockAnims;
      
    protected:
      void setNull();
      void cleanup();
      void drawBlock(OpenGTA::Map::BlockInfo* bi);
      void drawObject(OpenGTA::Map::ObjectPosition*);
      //OpenGL::PagedTexture createSprite(size_t sprNum, GraphicsBase::SpriteInfo* info);
      Util::CFrustum frustum;
      OpenGL::TextureCache<uint8_t>* sideCache;
      OpenGL::TextureCache<uint8_t>* lidCache;
      OpenGL::TextureCache<uint8_t>* auxCache;
      Map* loadedMap;
      OpenGTA::GraphicsBase* style;
      GLfloat zoomLevel;
      GLfloat camPos[3];
      GLfloat camVec[3];
      int visibleRange;
      bool topDownView;
      bool drawTextured;
      bool drawLines;

      SDL_Rect activeRect;
      SDL_Rect drawnRect;

      int  scene_rendered_vertices;
      int  scene_rendered_blocks;

      GLuint scene_display_list;
      bool scene_is_dirty;
      bool drawHeadingMarkers;
      int texFlipTest;

      Uint32 lastCacheEmptyTicks;

      NavData::Sector *current_sector;
  };
}

#endif
