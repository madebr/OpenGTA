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
#ifndef GL_BASE_H
#define GL_BASE_H
#include <SDL_opengl.h>

namespace OpenGL {
  /*
  template<GLenum target>
    struct TexOps
    {
      static GLuint create();
      static void   bind(GLuint id);
      static void   destropy(GLuint id);
    };
  */
  
  template<typename vertex_type, int entries_per_vertex>
    struct Quad 
    {
      vertex_type vertices[4][entries_per_vertex];
      static const GLenum primitiveType = GL_QUADS;
    };
  template<typename normal_type, int entries_per_vertex>
    struct QuadNormals
    {
      normal_type normals[4][entries_per_vertex];
    };
  template<typename color_type, int num_colors>
    struct QuadColors
    {
      color_type colors[4][num_colors];
    };
  template<typename texcoord_type, int num_coords>
    struct QuadTexCoords
    {
      texcoord_type texCoords[4][num_coords];
      GLuint texId;
    };

  template<typename vertex_type, int entries_per_vertex, typename color_type, int num_colors>
    struct ColoredQuad : public Quad<vertex_type, entries_per_vertex>,
                         public QuadColors<color_type, num_colors>
    {
    };
      
  template<typename vertex_type, int entries_per_vertex, typename texcoord_type, int num_coords>
    struct TexturedQuad : public Quad<vertex_type, entries_per_vertex>,
                          public QuadTexCoords<texcoord_type, num_coords>
    {
    };
  
  typedef Quad<GLint, 2> Quad2Int;
  typedef ColoredQuad<GLint, 2, GLfloat, 3> ColoredQuad2Int;
  typedef TexturedQuad<GLint, 2, GLfloat, 2> FontQuad;
  

  template<class T> class ImmediateRenderer
  {
    public:
      static void draw(const T & t);
      static void begin(GLenum type);
      static void end();
    protected:
      static GLenum currentPrimitiveType;
      static bool   insideBegin;
      static void   assertCorrectPrimitive(GLenum newType);
  };

  // ugly as hell, but seems to work
#define Renderer ImmediateRenderer

}
#endif
