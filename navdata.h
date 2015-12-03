/************************************************************************
* Copyright (c) 2005-2006 tok@openlinux.org.uk                          *
*                                                                       *
* This file contains code derived from information copyrighted by       *
* DMA Design. It may not be used in a commercial product.               *
*                                                                       *
* See license.txt for details.                                          *
*                                                                       *
* This notice may not be removed or altered.                            *
************************************************************************/
#ifndef NAVDATA_H
#define NAVDATA_H
#include <map>
#include <physfs.h>

namespace OpenGTA {

  /** Helper class for area names.
   *
   * A simple box; beware uint8 overflow!
   *
   * @see NavData
   */
  struct Rect2D {
    public:
      /** Zero-constructor.
       * Everything is 0
       */
      Rect2D();
      /** Constructor with full params.
       * @param x
       * @param y
       * @param w
       * @param h
       */
      Rect2D(PHYSFS_uint8, PHYSFS_uint8, PHYSFS_uint8, PHYSFS_uint8);
      /** Test: point-in-box.
       * @param x
       * @param y
       * @note If the point is inside 'lastSubLocation' is updated before returning.
       */
      bool isInside(PHYSFS_uint8, PHYSFS_uint8);
      /** Calculate north/south/east/west/central of point (which has to be inside).
       * @param x
       * @param y
       * @return uint8 bitfield
       */
      PHYSFS_uint8 subLocation(PHYSFS_uint8, PHYSFS_uint8);
      PHYSFS_uint16 getSize();
      PHYSFS_uint8 x, y;
      PHYSFS_uint8 w, h;
      /** Last sub-area location.
       * 0 = central
       * 1 = north
       * 2 = south
       * 4 = east
       * 8 = west
       * ... valid combinations of the last four
       */
      PHYSFS_uint8  lastSubLocation;
  };

  /** Container of all named sectors.
   * @see Sector
   */
  class NavData {
    public:
      /** A named sector of the map.
       */
      struct Sector : public Rect2D {
        /** Constructor from valid PHYSFS handle.
         */
        Sector(PHYSFS_file*);
        Sector();
        /** Sample number.
         * 1) see $LANGUAGE.FXT file for actual name
         * 2) probably sound?
         */
        PHYSFS_uint8 sam; // sample number
        char         name[30]; // FIXME: should not be used
        /** Returns the name prefixed with sub-area location.
         */
        const char*  getFullName();
        private:
          bool isADummy;
      };
      NavData(PHYSFS_uint32 size, PHYSFS_file *fd);
      ~NavData();
      Sector* getSectorAt(PHYSFS_uint8, PHYSFS_uint8);
    private:
      void clear();
      typedef std::multimap<PHYSFS_uint16, Sector*> SectorMapType;
      SectorMapType areas;
  };
}

#endif
