#include <stdio.h>

  float slope_raw_data[45][5][4][3] = {
#include "slope1_data.h"
  };

int main(int argc, char* argv[]) {
  int slope_type, face_num, vertex;
  int tex_x, tex_y;

  for (slope_type = 0 ; slope_type < 45; slope_type++) {
    printf("{ // slope: %i\n", slope_type);
    for (face_num = 1; face_num < 5; face_num++) {
      switch (face_num) {
        case 0:
          printf("  { // lid\n");
          tex_x = 0;
          tex_y = 2;
          break;
        case 1:
          printf("  { // north\n");
          tex_x = 0;
          tex_y = 1;
          break;
        case 2:
          printf("  { // south\n");
          tex_x = 0;
          tex_y = 1;
          break;
        case 3:
          printf("  { // west\n");
          tex_x = 2;
          tex_y = 1;
          break;
        case 4:
          printf("  { // east\n");
          tex_x = 2;
          tex_y = 1;
          break;
      }
      for (vertex = 0; vertex < 4; vertex++) {
        float x_tmp = ((face_num == 1) || (face_num == 3)) ? slope_raw_data[slope_type][face_num][vertex][tex_x] :
          1.0f - slope_raw_data[slope_type][face_num][vertex][tex_x];
        printf("    { %.2f, %.2f %s\n", x_tmp, 
            1.0f-slope_raw_data[slope_type][face_num][vertex][tex_y],
            (vertex == 3) ? "}" : "},");
      }
      printf("  %s\n", (face_num == 4) ? "}" : "},");
    }
    printf("%s\n", (slope_type == 44) ? "}" : "},");
  }
}
