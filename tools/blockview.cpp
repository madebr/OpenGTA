#include <iostream>
#include <SDL_opengl.h>
#include "common_sdl_gl.h"
#include "opengta.h"
#include "gl_texturecache.h"

namespace OpenGTA {
  class MapViewGL {
    private:
      Map* currentMap;
      Graphics8Bit* styleDB;
      OpenGL::TextureCache<uint8_t>* sideCache;
      OpenGL::TextureCache<uint8_t>* lidCache;
    public:
      MapViewGL();
      ~MapViewGL();
      int loadMap(const std::string &map, const std::string &style);
      void drawMap(int32_t x, int32_t y, int32_t dx, int32_t dy);
      void drawBlock(Map::BlockInfo* bi);
  };
}

namespace OpenGTA {
  MapViewGL::MapViewGL() {
    currentMap = NULL;
    styleDB = NULL;
    sideCache = NULL;
    lidCache = NULL;
  }
  MapViewGL::~MapViewGL() {
    if (currentMap)
      delete currentMap;
    if (styleDB)
      delete styleDB;
    if (sideCache)
      delete sideCache;
    if (lidCache)
      delete lidCache;
  }
  int MapViewGL::loadMap(const std::string &map, const std::string &style) {
    currentMap = new Map(map);
    styleDB = new Graphics8Bit(style);
    sideCache = new OpenGL::TextureCache<uint8_t>();
    lidCache = new OpenGL::TextureCache<uint8_t>();
    //currentMap->dump();
    return 0;
  }
}
extern SDL_Surface* screen;
GLfloat mapPos[3] = {12.0f, 12.0f, 20.0f};
GLfloat camVec[3] = {0.0f, 1.0f, 0.0f};

OpenGTA::MapViewGL *map = NULL;

void on_exit() {
  SDL_FreeSurface(screen);
  SDL_Quit();
  if (map)
    delete map;
  PHYSFS_deinit();
  if (global_EC)
    std::cerr << "Exiting after fatal problem - please see output above" << std::endl;
  else
    std::cout << "Goodbye" << std::endl;
}

void handleKeyPress( SDL_keysym *keysym ) {
  switch ( keysym->sym ) {
    case SDLK_ESCAPE:
      global_Done = 1;
      break;
    case SDLK_LEFT:
      mapPos[0] += 1.0f;
      break;
    case SDLK_RIGHT:
      mapPos[0] -= 1.0f;
      if (mapPos[0] < 0.0f)
        mapPos[0] = 0.0f;
      break;
    case SDLK_UP:
      mapPos[1] += 1.0f;
      break;
    case SDLK_DOWN:
      mapPos[1] -= 1.0f;
      if (mapPos[1] < 0.0f)
        mapPos[1] = 0.0f;
      break;
    case '+':
      mapPos[2] += 1.0f;
      break;
    case '-':
      mapPos[2] -= 1.0f;
      break;
    case 'x':
      camVec[0] = 1.0f; camVec[1] = 0.0f; camVec[2] = 0.0f;
      break;
    case 'y':
      camVec[0] = 0.0f; camVec[1] = 1.0f; camVec[2] = 0.0f;
      break;
    case 'z':
      camVec[0] = 0.0f; camVec[1] = 0.0f; camVec[2] = 1.0f;
      break;
    default:
      break;
  }
}

GLuint createGLTexture(GLsizei w, GLsizei h, const void* pixels) {
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //      GL_LINEAR_MIPMAP_LINEAR);
  /*gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w,
      h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  return tex;
}
  
void OpenGTA::MapViewGL::drawBlock(OpenGTA::Map::BlockInfo* bi) {
  float size = 0.5f;
  int jj = 0;
  if (bi == NULL)
    return;
  glEnable(GL_TEXTURE_2D);
  if (bi->left) {
    // Left Face
    if (!sideCache->hasTexture(bi->left)) {
      styleDB->getSide(static_cast<unsigned int>(bi->left-1), 0, true);
      sideCache->addTexture(bi->left, createGLTexture(64, 64, &styleDB->tileTmpRGBA));
    }
    glBindTexture(GL_TEXTURE_2D, sideCache->getTextureWithId(bi->left));
  //}
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); 
    //glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-size, -size, -size);// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-size, -size,  size);// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-size,  size,  size);// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-size,  size, -size);// Top Left Of The Texture and Quad
    glEnd();
  }
  if (bi->right) {
    // Right face
    if (!sideCache->hasTexture(bi->right)) {
      styleDB->getSide(static_cast<unsigned int>(bi->right-1), 0, true);
      sideCache->addTexture(bi->right, createGLTexture(64, 64, &styleDB->tileTmpRGBA));
    }
    glBindTexture(GL_TEXTURE_2D, sideCache->getTextureWithId(bi->right));
  //}
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 0.0f); 
    //glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f( size, -size, -size);// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f( size,  size, -size);// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( size,  size,  size);// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( size, -size,  size);// Bottom Left Of The Texture and Quad
    glEnd();
  }
  if (bi->top) {
    // Back Face
    if (!sideCache->hasTexture(bi->top)) {
      styleDB->getSide(static_cast<unsigned int>(bi->top-1), 0, true);
      sideCache->addTexture(bi->top, createGLTexture(64, 64, &styleDB->tileTmpRGBA));
    }
    glBindTexture(GL_TEXTURE_2D, sideCache->getTextureWithId(bi->top));
  //}
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); 
    //glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-size,  size, -size);// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-size,  size,  size);// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f( size,  size,  size);// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( size,  size, -size);// Top Right Of The Texture and Quad
    glEnd();
  }
  if (bi->bottom) {
    // Front Face
    if (!sideCache->hasTexture(bi->bottom)) {
      styleDB->getSide(static_cast<unsigned int>(bi->bottom-1), 0, true);
      sideCache->addTexture(bi->bottom, createGLTexture(64, 64, &styleDB->tileTmpRGBA));
    }
    glBindTexture(GL_TEXTURE_2D, sideCache->getTextureWithId(bi->bottom));
  //}
    glBegin(GL_QUADS);
    //glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-size, -size, -size);// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f( size, -size, -size);// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f( size, -size,  size);// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(-size, -size,  size);// Bottom Right Of The Texture and Quad
    glEnd();
  }
  if (bi->lid) {
    // Top Face
     if (!lidCache->hasTexture(bi->lid)) {
      styleDB->getLid(static_cast<unsigned int>(bi->lid-1), 0, true);
      lidCache->addTexture(bi->lid, createGLTexture(64, 64, &styleDB->tileTmpRGBA));
    }
    if (bi->typeMapExt & 128) {
      //std::cout << "blending!" << std::endl;
    }
    jj = 0;
    if (bi->typeMap & 16384) {
      jj += 2;
    }
    if (bi->typeMap & 32768) {
      jj += 4;
    }
    GLfloat lidTex[8] = {0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f};
#ifdef GLTEX_HELPER
#undef GLTEX_HELPER
#endif
#define GLTEX_HELPER \
 glTexCoord2f(lidTex[jj], lidTex[jj+1]); jj += 2; if (jj > 6) { jj = 0; }

    glBindTexture(GL_TEXTURE_2D, lidCache->getTextureWithId(bi->lid));
  //}
    glBegin(GL_QUADS);
    GLTEX_HELPER;
    //glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-size, -size,  size);// Bottom Left Of The Texture and Quad
    GLTEX_HELPER;
    glVertex3f( size, -size,  size);// Bottom Right Of The Texture and Quad
    GLTEX_HELPER; 
    glVertex3f( size,  size,  size);// Top Right Of The Texture and Quad
    GLTEX_HELPER;
    glVertex3f(-size,  size,  size);// Top Left Of The Texture and Quad
    glEnd();
#undef GLTEX_HELPER
  }
}
void OpenGTA::MapViewGL::drawMap(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;
  
  // FIXME: can't access 255 either x or y... WHY?
  if (x2 >= 255)
    x2 = 255;
  if (y2 >= 255)
    y2 = 255;

  //std::cout << "draw: " << x1 << " " << y1 << " - " << x2 << " " << y2 << std::endl;
  sideCache->sink();
  lidCache->sink();
  for (int i = y1; i <= y2; i++) {
    glPushMatrix();
    glTranslatef(0.0f, -1.0f*i, 0.0f);
    for (int j= x1; j <= x2; j++) {
      glPushMatrix();
      glTranslatef(1.0f*j, 0.0f, 0.0f);
      PHYSFS_uint16 emptycount = currentMap->getNumBlocksAt(j,i);
      /*glPushMatrix();
       * for(int c=0; c < 6 - emptycount-1; c++) */
      for (int c=6-emptycount-0; c >= 1; c--) {
        drawBlock(currentMap->getBlockAt(j,i, c));
        glTranslatef(0.0f, 0.0f, 1.0f);
      }
      glPopMatrix();
    }
    glPopMatrix();
  }
  sideCache->clear();
  lidCache->clear();
}

void drawScene() {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(mapPos[0], -mapPos[1], mapPos[2], mapPos[0], -mapPos[1], 0.0f, camVec[0], camVec[1], camVec[2]);
  //map->drawMap(int(mapPos[0]), int(mapPos[1]));  
  map->drawMap(int32_t(mapPos[0])-10, int32_t(mapPos[1])-10,int32_t(mapPos[0])+10, int32_t(mapPos[1])+10);

  SDL_GL_SwapBuffers();
}


void run_main() {
  SDL_Event event;
  int paused = 0;

  PHYSFS_init("mapview");
  PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);

  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glEnable(GL_ALPHA_TEST);
  //glDisable(GL_DEPTH_TEST);
  
  map = new OpenGTA::MapViewGL();
  map->loadMap("NYC.CMP", "STYLE001.GRY");
  
  while(!global_Done && !global_EC) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_ACTIVEEVENT:
          if (event.active.gain == 0)
            paused = 1;
          else
            paused = 0;
          break;
        case SDL_KEYDOWN:
          handleKeyPress(&event.key.keysym);
          break;
        case SDL_VIDEORESIZE:
          screen = SDL_SetVideoMode( event.resize.w,
              event.resize.h, 32, videoFlags );
          if (!screen)
            ERROR("Failed to set video mode after resize event");
          resize(event.resize.w, event.resize.h);
          break;
        case SDL_QUIT:
          global_Done = 1;
          break;
        case SDL_MOUSEMOTION:
          std::cout << "Mouse move: x " << float(event.motion.x)/screen->w << " y " << float(event.motion.y)/screen->h << std::endl;
          break;
        default:
          break;
      }
    }
    if (!paused)
      drawScene();
  }
}
