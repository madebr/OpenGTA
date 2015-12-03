#include <stdio.h>

  float slope_raw_data[45][5][4][3] = {
#include "slope1_data.h"
  };

int main(int argc, char* argv[]) {
  int slope_type, face_num, vertex;
  int tex_x, tex_y;

  for (slope_type = 0 ; slope_type < 45; slope_type++) {
    printf("{ // slope: %i\n", slope_type);
    for (face_num = 0; face_num < 5; face_num++) {
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
      int face_lower_left_found = 0;
      int face_lower_left_is = -1;
      for (vertex = 0; vertex < 3; vertex++) {
        if ((slope_raw_data[slope_type][face_num][vertex][tex_x] ==
          slope_raw_data[slope_type][face_num][vertex+1][tex_x]) &&
          (slope_raw_data[slope_type][face_num][vertex][tex_y] ==
            slope_raw_data[slope_type][face_num][vertex+1][tex_y]))
            printf("// degenerate face\n");
      }
      for (vertex = 0; vertex < 4; vertex++) {
        char* c;
        if (slope_raw_data[slope_type][face_num][vertex][tex_x] == 0.0f &&
          slope_raw_data[slope_type][face_num][vertex][tex_y] == 0.0f) {
          c = "// *";
          face_lower_left_found += 1;
          face_lower_left_is = vertex;
        }
        else
          c = "";
        printf("    { %.2f, %.2f %.2f %s %s\n", slope_raw_data[slope_type][face_num][vertex][0],
            slope_raw_data[slope_type][face_num][vertex][1],
            slope_raw_data[slope_type][face_num][vertex][2],
            (vertex == 3) ? "}" : "},", c);
      }
      printf("// %i lower faces; last: %i\n", face_lower_left_found, face_lower_left_is);
      printf("  %s\n", (face_num == 4) ? "}" : "},");
    }
    printf("%s\n", (slope_type == 44) ? "}" : "},");
  }
}
