#include <cassert>
float slope_height_offset(unsigned char slope_type, float dx, float dz) {
  assert((dx >= 0.0f) && (dx <= 1.0f));
  assert((dz >= 0.0f) && (dz <= 1.0f));
  switch(slope_type) {
    case 0:
      return 1.0f;
    case 1:
      return 0.500000f - dz * 0.500000f;
    case 2:
      return 1.000000f - dz * 0.500000f;
    case 3:
      return 0.000000f + dz * 0.500000f;
    case 4:
      return 0.500000f + dz * 0.500000f;
    case 5:
      return 0.500000f - dx * 0.500000f;
    case 6:
      return 1.000000f - dx * 0.500000f;
    case 7:
      return 0.000000f + dx * 0.500000f;
    case 8:
      return 0.500000f + dx * 0.500000f;
    case 9:
      return 0.130000f - dz * 0.130000f;
    case 10:
      return 0.250000f - dz * 0.120000f;
    case 11:
      return 0.380000f - dz * 0.130000f;
    case 12:
      return 0.500000f - dz * 0.120000f;
    case 13:
      return 0.630000f - dz * 0.130000f;
    case 14:
      return 0.750000f - dz * 0.120000f;
    case 15:
      return 0.880000f - dz * 0.130000f;
    case 16:
      return 1.000000f - dz * 0.120000f;
    case 17:
      return 0.000000f + dz * 0.130000f;
    case 18:
      return 0.130000f + dz * 0.120000f;
    case 19:
      return 0.250000f + dz * 0.130000f;
    case 20:
      return 0.380000f + dz * 0.120000f;
    case 21:
      return 0.500000f + dz * 0.130000f;
    case 22:
      return 0.630000f + dz * 0.120000f;
    case 23:
      return 0.750000f + dz * 0.130000f;
    case 24:
      return 0.880000f + dz * 0.120000f;
    case 25:
      return 0.130000f - dx * 0.130000f;
    case 26:
      return 0.250000f - dx * 0.120000f;
    case 27:
      return 0.380000f - dx * 0.130000f;
    case 28:
      return 0.500000f - dx * 0.120000f;
    case 29:
      return 0.630000f - dx * 0.130000f;
    case 30:
      return 0.750000f - dx * 0.120000f;
    case 31:
      return 0.880000f - dx * 0.130000f;
    case 32:
      return 1.000000f - dx * 0.120000f;
    case 33:
      return 0.000000f + dx * 0.130000f;
    case 34:
      return 0.130000f + dx * 0.120000f;
    case 35:
      return 0.250000f + dx * 0.130000f;
    case 36:
      return 0.380000f + dx * 0.120000f;
    case 37:
      return 0.500000f + dx * 0.130000f;
    case 38:
      return 0.630000f + dx * 0.120000f;
    case 39:
      return 0.750000f + dx * 0.130000f;
    case 40:
      return 0.880000f + dx * 0.120000f;
    case 41:
      return 1.000000f - dz * 1.000000f;
    case 42:
      return 0.000000f + dz * 1.000000f;
    case 43:
      return 1.000000f - dx * 1.000000f;
    case 44:
      return 0.000000f + dx * 1.000000f;
    default:
      break;
  }
  assert(0);
  return 0.0f; // should never be reached
}
