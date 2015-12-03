#include <stdio.h>

  float slope_raw_data[45][5][4][3] = {
#include "slope1_data.h"
  };

int main(int argc, char* argv[]) {
  int slope_type, vertex;
  int equal_y1, equal_y2, other_y;
  int marked_axis;
#define STORE(a, b, c) { equal_y1 = a; equal_y2 = b; other_y = c;}

  printf("#include <cassert>\n");
  printf("float slope_height_offset(unsigned char slope_type, float dx, float dz) {\n");
  printf("  assert((dx >= 0.0f) && (dx <= 1.0f));\n");
  printf("  assert((dz >= 0.0f) && (dz <= 1.0f));\n");
  printf("  switch(slope_type) {\n");

  for (slope_type = 0 ; slope_type < 45; slope_type++) {
    //printf("# slope %i\n", slope_type);
    printf("    case %i:\n", slope_type);
    if (slope_raw_data[slope_type][0][0][1] == slope_raw_data[slope_type][0][1][1]) {
      //printf("0, 1\n");
      STORE(0, 1, 2);
    }
    else if (slope_raw_data[slope_type][0][1][1] == slope_raw_data[slope_type][0][2][1]) {
      //printf("1, 2\n");
      STORE(1, 2, 0);
    }
    else if (slope_raw_data[slope_type][0][2][1] == slope_raw_data[slope_type][0][3][1]) {
      //printf("2, 3\n");
      STORE(2, 3, 0);
    }
    else if (slope_raw_data[slope_type][0][0][1] == slope_raw_data[slope_type][0][2][1]) {
      //printf("0, 2\n");
      STORE(0, 2, 1);
    }
    else if (slope_raw_data[slope_type][0][1][1] == slope_raw_data[slope_type][0][3][1]) {
      //printf("1, 3\n");
      STORE(1, 3, 0);
    }
    else {
      printf("argh?\n");
      return(1);
    }
    if (slope_raw_data[slope_type][0][equal_y1][0] == slope_raw_data[slope_type][0][equal_y2][0]) {
      //printf("# x-axis\n");
      marked_axis = 0;
    }
    else if (slope_raw_data[slope_type][0][equal_y1][2] == slope_raw_data[slope_type][0][equal_y2][2]) {
      //printf("# z-axis\n");
      marked_axis = 2;
    }
    else {
      printf("not good\n");
      return(1);
    }

    float low = 1.0f;
    float high = 0.0f;
    int low_idx = -1;
    int high_idx = -1;
    for (vertex = 0; vertex < 4; vertex++) {
      if (slope_raw_data[slope_type][0][vertex][1] < low) {
        low = slope_raw_data[slope_type][0][vertex][1];
        low_idx = vertex;
      }
      if (slope_raw_data[slope_type][0][vertex][1] > high) {
        high = slope_raw_data[slope_type][0][vertex][1];
        high_idx = vertex;
      }
    }
    //printf("# low: %f high: %f\n", low, high);
    int increasing;

    if (slope_raw_data[slope_type][0][low_idx][marked_axis] < 
      slope_raw_data[slope_type][0][high_idx][marked_axis])
      increasing = 1;
    else
      increasing = 0; 

    if (high - low == 0.0f)
      printf("      return 0.0f;\n");
    else
      printf("      return %ff %c d%c * %ff;\n", (increasing ? low : high),
      (increasing ? '+' : '-'),
      ((marked_axis == 0) ? 'x' : 'z'), (high - low));
    
    /*
    for (vertex = 0; vertex < 4; vertex++) {
      printf("%f %f %f\n", 
        slope_raw_data[slope_type][0][vertex][0],
        slope_raw_data[slope_type][0][vertex][1],
        slope_raw_data[slope_type][0][vertex][2]);
    }
    */
  }
  printf("    default:\n");
  printf("      break;\n");
  printf("  }\n");
  printf("  assert(0);\n");
  printf("  return 0.0f; // should never be reached\n");
  printf("}\n");
}
