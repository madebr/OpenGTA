#ifndef GL_CAMERA_H
#define GL_CAMERA_H
#include <SDL.h>
#include <SDL_opengl.h>
#include "math3d.h"

namespace OpenGL {

  class Camera {
    private:
      Camera();
      ~Camera() = default;
    public:
      Camera(const Camera& copy) = delete;
      Camera& operator=(const Camera& copy) = delete;

      static Camera& Instance()
      {
        static Camera instance;
        return instance;
      }

      void setSpeed(float forward_is_positive);
      void setRotating(bool demo);
      void setCamGravity(bool demo);
      void rotateView(float x, float y, float z);
      void rotateAround(const Vector3D & c, float x, float y, float z);
      void translateBy(const Vector3D & t);
      void translateTo(const Vector3D & e);
      void setVectors(const Vector3D & e, const Vector3D & c, const Vector3D & u);
      void moveByMouse();
      void interpolate(const Vector3D & to, const Uint32 & start, const Uint32 & end);
      void setFollowMode(const Vector3D & target);
      void releaseFollowMode();

      void update(Uint32 dt);
      Vector3D & getEye() { return eye; }
      Vector3D & getCenter() { return center; }
      Vector3D & getUp() { return up; }
    private:
      void update_game(Uint32 dt);
      Vector3D eye;
      Vector3D center;
      Vector3D up;
      float  speed;
      bool   doRotate;
      bool   camGravity;
      bool   gameCamMode;
      Vector3D const * followTarget;

      Vector3D interpolateFrom;
      Vector3D interpolateTo;
      Uint32 interpolateStart;
      Uint32 interpolateEnd;
  };
}
#endif
