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
#include <iostream>
#include "sound_resample2.h"
#include "interpolate.hpp"

namespace Audio {
  // resample uint8 mono to Sint16 stereo
  // assumption: src_rate < wanted_rate

  // derived from some sweet code found in the exult repository,
  // now rewritten to avoid license trouble...
  // yes, it is (still) similiar - who would have guessed?
  Sint16 *resample_new (Uint8 * src, size_t sourcelen, size_t & size,
      int rate, int wanted_rate) {
    int fp_pos = 0;
    int fp_speed = (1 << 16) * rate / wanted_rate;
    size = sourcelen;
    // adjust the magnitudes of size and rate to prevent division error
    while (size & 0xFFFF0000)
      size >>= 1, rate = (rate >> 1) + 1;
    // Compute the output size (times 4 since it is 16 stereo)
    size = (size * wanted_rate / rate) << 2;

    Sint16 *stereo_data = new Sint16[size];
    Sint16 *data = stereo_data;
    Uint8 *src_end = src + sourcelen;

    int result;

    // Compute the initial data feed for the interpolator. We don't simply
    // shift by 8, but rather duplicate the byte, this way we cover the full
    // range. Probably doesn't make a big difference, listening wise :-)
    int a = *(src + 0);
    a = (a | (a << 8)) - 32768;
    int b = *(src + 1);
    b = (a | (b << 8)) - 32768;
    int c = *(src + 2);
    c = (a | (c << 8)) - 32768;

    // scale down by 0.8333 to avoid oversampling
    Math::Interpolator::Cubic<float> inter(a * 0.8333f, b * 0.8333f, c * 0.8333f);
    do
    {
      do
      {
        result = int(inter.getAt(fp_pos / float(0x0000FFFF)));
        // limit the range
        if (result < -32768)
          result = -32768;
        else if (result > 32767)
          result = 32767;
        *data++ = result;
        *data++ = result;

        fp_pos += fp_speed;
      }
      while (!(fp_pos & 0xFFFF0000)); // need more input data
      src++;
      fp_pos &= 0x0000FFFF;
      if (src + 2 < src_end) // still data to fetch
      {
        c = *(src + 2);
        c = (c | (c << 8)) - 32768;
        inter.shiftAndFeed (c * 0.8333f);
      }
      else // guess & interpolate... this the end, my only friend the end
        inter.shift ();
    }
    while (src < src_end);
    return stereo_data;
  }

  Sint16 *resample16 (Uint8 * src, size_t sourcelen, size_t & size,
      int rate, int wanted_rate) {
    int fp_pos = 0;
    int fp_speed = (1 << 16) * rate / wanted_rate;
    size = sourcelen;
    while (size & 0xFFFF0000)
      size >>= 1, rate = (rate >> 1) + 1;
    size = (size * wanted_rate / rate) << 1;

    Sint16 *stereo_data = new Sint16[size];
    Sint16 *data = stereo_data;
    Uint8 *src_end = src + sourcelen;

    int result;
    int a = *(src + 0);
    a = a + (*(src + 1) << 8);
    int b = *(src + 2);
    b = b + (*(src + 3) << 8);
    int c = *(src + 4);
    c = c + (*(src + 5) << 8);


    std::cout << a << " " << b << " " << c << std::endl;
    Math::Interpolator::Cubic<float> inter(a * 0.8333f, b * 0.8333f, c * 0.8333f);
    do
    {
      do
      {
        result = int(inter.getAt(fp_pos / float(0x0000FFFF)));
        if (result < -32768)
          result = -32768;
        else if (result > 32767)
          result = 32767;
        *data++ = result;

        fp_pos += fp_speed;
      }
      while (!(fp_pos & 0xFFFF0000));
      src++;
      fp_pos &= 0x0000FFFF;
      if (src + 2 < src_end)
      {
        c = *(src + 2);
        c = c + (*(src + 3) << 8);
        inter.shiftAndFeed (c * 0.8333f);
      }
      else
        inter.shift ();
    }
    while (src < src_end);
    return stereo_data;
  }

}
