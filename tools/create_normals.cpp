#include <iostream>
#include <cmath>

float slope_raw_data[45][5][4][3] = {
#include "slope1_data.h"
};

int main(int argc, char* argv[]) {
  for (int which=0; which <= 44; which++) {
    std::cout << "  // slope " << which << std::endl;
    int side = 0;

      float x1,x2,y1,y2,z1,z2 = 0.0f;
      x1 = slope_raw_data[which][side][0][0] - slope_raw_data[which][side][1][0];
      y1 = slope_raw_data[which][side][0][1] - slope_raw_data[which][side][1][1];
      z1 = slope_raw_data[which][side][0][2] - slope_raw_data[which][side][1][2];

      x2 = slope_raw_data[which][side][1][0] - slope_raw_data[which][side][2][0];
      y2 = slope_raw_data[which][side][1][1] - slope_raw_data[which][side][2][1];
      z2 = slope_raw_data[which][side][1][2] - slope_raw_data[which][side][2][2];

      float nx,ny,nz,vLen = 0.0f;
      nx = (y1 * z2) - (z1 * y2);
      ny = (z1 * x2) - (x1 * z2);
      nz = (x1 * y2) - (y1 * x2);
      vLen = sqrt( (nx * nx) + (ny * ny) + (nz * nz) );
      if (vLen == 0.0f)
        std::printf("%f %f %f\n", nx, ny, nz);
      nx /= vLen;
      ny /= vLen;
      nz /= vLen;

      if (which < 44)
        std::printf("  { %f, %f, %f },\n", nx, ny, nz);
      else
        std::printf("  { %f, %f, %f }\n", nx, ny, nz);
  }
}
