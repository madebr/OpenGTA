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
#include <cassert>
#include "gl_font.h"
#include "log.h"
#include "buffercache.h"
#include "m_exceptions.h"

namespace OpenGL {
  DrawableFont::DrawableFont() {
    fontSource = NULL;
    texCache = NULL;
    scale = 1;
  }
  DrawableFont::~DrawableFont() {
    cleanup();
  }
  void DrawableFont::setScale(unsigned int newScale) {
    scale = newScale;
    clearCached();
  }
  void DrawableFont::clearCached() {
    std::map<char, FontQuad*>::const_iterator j = drawables.begin();
    while (j != drawables.end()) {
      delete j->second;
      ++j;
    }
    drawables.clear();
  }
  void DrawableFont::resetTextures() {
    clearCached();
    texCache->clearAll();
  }
  void DrawableFont::loadFont(const std::string & filename) {
    cleanup();
    fontSource = new OpenGTA::Font(filename);
    texCache = new TextureCache<char>(("FontTextures: " + filename).c_str());
    srcName.clear();
    srcName = filename;
  }
  void DrawableFont::cleanup() {
    clearCached();
    if (fontSource != NULL)
      delete fontSource;
    if (texCache != NULL)
      delete texCache;
    fontSource = NULL;
    texCache = NULL;
  }
  GLfloat DrawableFont::drawString(const std::string & text) {
    assert(texCache != NULL);
    assert(fontSource != NULL);
    std::string::const_iterator i = text.begin();
    std::string::const_iterator e = text.end();
    GLfloat move = 0.0f;
    while (i != e) {

      if (*i != ' ') {
        FontQuad* character = NULL;
        std::map<char, FontQuad*>::const_iterator j = drawables.find(*i);
        if (j == drawables.end()) {
          character = createDrawableCharacter(*i);
          drawables[*i] = character;
        }
        else
          character = j->second;
        Renderer<FontQuad>::draw(*character);
      }
      GLfloat mm = float(fontSource->getMoveWidth(*i)) * 1.1f * scale;
      glTranslatef(mm, 0.0f, 0.0f);
      move += mm; 
      i++;
    }
    return move;
  }

  GLfloat DrawableFont::drawString_r2l(const std::string & text) {
    assert(texCache != NULL);
    assert(fontSource != NULL);
    std::string::const_reverse_iterator i = text.rbegin();
    std::string::const_reverse_iterator e = text.rend();
    GLfloat move = 0.0f;
    while (i != e) {

      if (*i != ' ') {
        FontQuad* character = NULL;
        std::map<char, FontQuad*>::const_iterator j = drawables.find(*i);
        if (j == drawables.end()) {
          character = createDrawableCharacter(*i);
          drawables[*i] = character;
        }
        else {
          GLfloat mm = float(fontSource->getMoveWidth(*i)) * 1.1f * scale;
          glTranslatef(-mm, 0.0f, 0.0f);
          character = j->second;
          move += mm; 
        }
        Renderer<FontQuad>::draw(*character);
      }
      i++;
    }
    return move;
  }

  uint16_t DrawableFont::getHeight() {
    return scale * fontSource->getCharHeight();
  }

  FontQuad* DrawableFont::createDrawableCharacter(const char & c) {
    GLuint texid;
    unsigned int w;
    unsigned int h;
    unsigned char * src = fontSource->getCharacterBitmap(
        fontSource->getIdByChar(c), &w, &h);
    if (src == NULL) {
      throw E_UNKNOWNKEY("Failed to load bitmap for: " + std::to_string(c));
      //throw std::string("Failed to load bitmap for character: " + c);
    }
    unsigned int glwidth = 1;
    unsigned int glheight = 1;

    while(glwidth < w)
      glwidth <<= 1;

    while(glheight < h)
      glheight <<= 1;

    Util::BufferCache & bc = Util::BufferCache::Instance();
    
    unsigned char* dst = bc.requestBuffer(glwidth * glheight * 4);
    assert(dst != NULL);
    unsigned char * t = dst;
    unsigned char * r = src;
    for (unsigned int i = 0; i < h; i++) {
      memcpy(t, r, w * 4);
      t += glwidth * 4;
      r += w * 4;
    }
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glwidth, glheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, dst);
    texCache->addTexture(c, texid);

    FontQuad* res = new FontQuad();
    res->vertices[0][0] = res->vertices[0][1] = 0;
    res->vertices[1][0] = w * scale;
    res->vertices[1][1] = 0;
    res->vertices[2][0] = w * scale;
    res->vertices[2][1] = h * scale;
    res->vertices[3][0] = 0;
    res->vertices[3][1] = h * scale;

    float glw = float(w) / float(glwidth);
    float glh = float(h) / float(glheight);
    res->texCoords[0][0] = 0.0f;
    res->texCoords[0][1] = glh;
    res->texCoords[1][0] = glw;
    res->texCoords[1][1] = glh;
    res->texCoords[2][0] = glw;
    res->texCoords[2][1] = 0.0f;
    res->texCoords[3][0] = 0.0f;
    res->texCoords[3][1] = 0.0f;
    
    res->texId = texid;
    
    return res;
  }
}

#if 0
int main() {
  OpenGL::DrawableFont *f = new OpenGL::DrawableFont();
  f->drawString("hello world");
  delete f;
}
#endif
