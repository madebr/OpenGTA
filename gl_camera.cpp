#include "gl_camera.h"
#include "gl_screen.h"
#include "opengta.h"
#include "dataholder.h"
#include <iostream>
#include "log.h"
#include "blockdata.h"

using namespace OpenGTA;
float slope_height_offset(unsigned char slope_type, float dx, float dz);
namespace OpenGL {

/* implementation mainly taken from:
 *
 * "Talk to me like I'm a 3 year old!" Programming Lessons
 * by DigiBen  (digiben@gametutorials.com)
 *
 */
  
  Camera::Camera() : eye(), center(), up(), doRotate(false), 
    camGravity(false), gameCamMode(false), followTarget(&center) {
    interpolateStart = 0;
    interpolateEnd = 0;
  }

  void Camera::update_game(Uint32 ticks) {
    Vector3D delta(center - *followTarget);
    //INFO << delta.x << ", " << delta.y << ", " << delta.z << std::endl;
    float height_dist = fabs(delta.y);
    delta.y = 0;
    if (camGravity) {
      if (center.y - followTarget->y > 4.1) {
        delta.y = 0.001f * height_dist;
      }
      else if (center.y - followTarget->y < 3.9) {
        delta.y = -0.001f * height_dist; 
      }
    }
    //INFO << center.y << " " << followTarget->y<< " " << height_dist << std::endl;

    center += -delta;
    eye += -delta;
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z,
        up.x, up.y, up.z);
  }

  void Camera::setFollowMode(const Vector3D & target) {
    followTarget = &target;
    //INFO << "following " << target.x << ", " << target.y << ", " << target.z << std::endl;
    gameCamMode = 1;
  }

  void Camera::releaseFollowMode() {
    followTarget = &center;
    gameCamMode = 0;
  }

  void Camera::update(Uint32 ticks) {
    if (gameCamMode) {
      update_game(ticks);
      return;
    }
    moveByMouse();

    float x,y,z;
    x = floor(eye.x);
    y = floor(eye.y);
    z = floor(eye.z);
    int do_grav = 1;
    float delta_y = 0;
    if (camGravity && do_grav) {
      center.y -= 0.1f * ticks/40.0f;
      eye.y -= 0.1f * ticks/40.0f;
    }

    OpenGTA::Map & map = OpenGTA::ActiveMap::Instance().get();
    if (y < map.getNumBlocksAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z)) && y > 0.0f) {
      OpenGTA::Map::BlockInfo * block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z), PHYSFS_uint8(y));
      if (block->blockType() > 0 && block->blockType() <= 5) {
        float bz = slope_height_offset(block->slopeType(), eye.x - x, eye.z - z);
        if (block->slopeType() == 0 && (block->blockType() != 5 && block->blockType() != 6))
          bz -= 1;
        //INFO << int(block->blockType()) << ", " << eye.y << ", " << 
        //  eye.y - y << ", " << eye.y - y - bz << ", " << bz << std::endl;
        float react_delta = 0.3f;
        if (block->blockType() == 5 || block->blockType() == 6)
          react_delta = 0.0f;
        if (eye.y - y - bz < react_delta) {
          //do_grav = 0;
          Vector3D new_eye(eye);
          new_eye.y = y + bz + react_delta;
          delta_y = new_eye.y - eye.y;
          center.y = center.y - eye.y + new_eye.y;
          eye.y = new_eye.y;
        }
      }

#if 0
      if (y >= 1.0f) {
        block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z), PHYSFS_uint8(y-1));
        if (block->blockType() > 0) {
          INFO << "below is " << int(block->blockType()) << std::endl;
          center.y = center.y - eye.y + y + 0.3f;
          eye.y = y + 0.3f;
        }
      }
#endif
    }
    y -= 1;
    if (y < map.getNumBlocksAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z)) && y > 0.0f) {
      OpenGTA::Map::BlockInfo * block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z), PHYSFS_uint8(y));
      if (block->blockType() == 5 || block->blockType() == 6) {
        float bz = slope_height_offset(block->slopeType(), eye.x - x, eye.z - z);
        //INFO << eye.y << ", " << y << " bz " << bz << std::endl;
        if (eye.y - y - bz < 0.4f) {
          Vector3D new_eye(eye);
          new_eye.y = y + bz + 0.4;
          delta_y = new_eye.y - eye.y;
          //INFO << "setting " << new_eye.y << std::endl;
          center.y = center.y - eye.y + new_eye.y;
          eye.y = new_eye.y;
          do_grav = 0;
        }
      }
    }

    /*
    if (camGravity && do_grav) {
      Vector dn(0, -0.1f*do_grav, 0);
      eye += dn;
      center += dn;
    }*/
    /*
    if (camGravity && do_grav) {
      center.y -= 0.1f * ticks/40.0f;
      eye.y -= 0.1f * ticks/40.0f;
    }*/

    if (interpolateStart) {
      float as_one = float(interpolateStart - 1) / interpolateEnd;
      Vector3D now = (1 - as_one) * interpolateFrom + as_one * interpolateTo;
      center = center - eye + now;
      eye = now;

      interpolateStart += ticks;
      if (interpolateStart > interpolateEnd)
        interpolateStart = 0;
    }
    else {

      if (speed > 0.01f || speed < -0.01f) {

        Vector3D v = center - eye;
        if (camGravity)
          v.y = delta_y;
        v = v.Normalized();
        center += speed * ticks/40.0f * v;
        eye += speed * ticks/40.0f * v;
        //INFO << v.y << std::endl;
      }

      if (doRotate)
        rotateAround(Vector3D(center.x, 0, center.z), 0, 0.01f, 0);
    }
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z,
        up.x, up.y, up.z);
  }

  void Camera::setRotating(bool demo) {
    doRotate = demo;
  }

  void Camera::setCamGravity(bool demo) {
    camGravity = demo;
  }

  void Camera::setVectors(const Vector3D & e, const Vector3D & c, const Vector3D & u) {
    eye = e;
    center = c;
    up = u;
  }

  void Camera::setSpeed(float new_speed) {
    speed = new_speed;
  }

  void Camera::translateBy(const Vector3D & t) {
    eye += t;
    center += t;
  }

  void Camera::translateTo(const Vector3D & e) {
    Vector3D rel = eye - e;
    translateBy(rel);
  }

  void Camera::rotateView(float x, float y, float z) {
    Vector3D v = center - eye;
    if (x) {
      center.z = eye.z + sin(x) * v.y + cos(x) * v.z;
      center.y = eye.y + cos(x) * v.y - sin(x) * v.z;
    }
    if (y) {
      center.z = eye.z + sin(y) * v.x + cos(y) * v.z;
      center.x = eye.x + cos(y) * v.x - sin(y) * v.z;
    }
    if (z) {
      center.x = eye.x + sin(z) * v.y + cos(z) * v.x;
      center.y = eye.y + cos(z) * v.y - sin(z) * v.x;
    }
  }

  void Camera::rotateAround(const Vector3D & lookAt, float x, float y, float z) {
    Vector3D v = eye - lookAt;
    if (x) {
      eye.z = lookAt.z + sin(x) * v.y + cos(x) * v.z;
      eye.y = lookAt.y + cos(x) * v.y - sin(x) * v.z;
    }
    if (y) {
      eye.z = lookAt.z + sin(y) * v.x + cos(y) * v.z;
      eye.x = lookAt.x + cos(y) * v.x - sin(y) * v.z;
    }
    if (z) {
      eye.x = lookAt.x + sin(z) * v.y + cos(z) * v.x;
      eye.y = lookAt.y + cos(z) * v.y - sin(z) * v.x;
    }
  }

  void Camera::moveByMouse() {
    Screen & screen = Screen::Instance();
    int w, h;
    w = screen.getWidth() / 2;
    h = screen.getHeight() / 2;
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_WarpMouse(w, h);
    if ((mx == w) && (my == h))
      return;
    float rot_x = (float(w) - mx) / 100;
    float rot_y = (float(h) - my) / 100;
    center.y += rot_y * 8;
    if (center.y - eye.y > 15)
      center.y = eye.y + 15;
    else if (center.y - eye.y < -15)
      center.y = eye.y - 15;
    rotateView(0, -rot_x, 0);
  }

  void Camera::interpolate(const Vector3D & to, const Uint32 & start, const Uint32 & end) {
    interpolateFrom = eye;
    interpolateTo = to;
    interpolateStart = start;
    interpolateEnd = end;
  }

#if 0
  void QuaternionCamera::update(Uint32 ticks) {
    float cur_ratio = 0;
    quat step = exp(sampleLinear(ln_start, ln_end, cur_ratio));
    mtx44 m = quatToRotationMatrix(step);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf((float*)m.x);
  }
#endif
}
