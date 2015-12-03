#include <iostream>
#include <getopt.h>
#include "interpolate.hpp"

using namespace std;

void run_linear(int numSteps) {
  float y1, y2;
  cin >> y1;
  cin >> y2;
  Math::Interpolator::Linear<float> li(y1, y2);
  int counter = 0;
  for (int i = 0; i < numSteps; ++i) {
    cout << counter << " " << li.getAt(float(i) / numSteps) << endl;
    ++counter;
  }
  while (cin >> y2) {
    li.shiftAndFeed(y2);
    for (int i = 0; i < numSteps; ++i) {
      cout << counter << " " << li.getAt(float(i) / numSteps) << endl;
      ++counter;
    }
  }
}

void run_cubic(int numSteps) {
  float y1, y2, y3;

  cin >> y1;
  cin >> y2;
  cin >> y3;
  Math::Interpolator::Cubic<float> ci(y1, y2, y3);
  int counter = 0;
  for (int i = 0; i < numSteps; ++i) {
    cout << counter << " " << ci.getAt(float(i) / numSteps) << endl;
    ++counter;
  }
  while (cin >> y3) {
    ci.shiftAndFeed(y3);
    for (int i = 0; i < numSteps; ++i) {
      cout << counter << " " << ci.getAt(float(i) / numSteps) << endl;
      ++counter;
    }
  }
  ci.shift();
  for (int i = 0; i < numSteps; ++i) {
    cout << counter << " " << ci.getAt(float(i) / numSteps) << endl;
    ++counter;
  }
}

void run_cosine(int numSteps) {
  float y1, y2;

  cin >> y1;
  cin >> y2;
  
  Math::Interpolator::Cosine<float> ci(y1, y2);
  int counter = 0;
  for (int i = 0; i < numSteps; ++i) {
    cout << counter << " " << ci.getAt(float(i) / numSteps) << endl;
    ++counter;
  }
  while (cin >> y2) {
    ci.shiftAndFeed(y2);
    for (int i = 0; i < numSteps; ++i) {
      cout << counter << " " << ci.getAt(float(i) / numSteps) << endl;
      ++counter;
    }
  }
}

void run_hermite(int numSteps, float tension, float bias) {
  float y1, y2, y3;

  cin >> y1;
  cin >> y2;
  cin >> y3;
  Math::Interpolator::Hermite<float> hi(y1, y2, y3, tension, bias);
  int counter = 0;
  for (int i = 0; i < numSteps; ++i) {
    cout << counter << " " << hi.getAt(float(i) / numSteps) << endl;
    ++counter;
  }
  while (cin >> y3) {
    hi.shiftAndFeed(y3);
    for (int i = 0; i < numSteps; ++i) {
      cout << counter << " " << hi.getAt(float(i) / numSteps) << endl;
      ++counter;
    }
  }
  hi.shift();
  for (int i = 0; i < numSteps; ++i) {
    cout << counter << " " << hi.getAt(float(i) / numSteps) << endl;
    ++counter;
  }
}


int main(int argc, char* argv[]) {

  int numSteps = 10;
  int run = 2;

  float hermite_bias = 1.0f;
  float hermite_tension = 1.0f;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"linear", 0, 0, 'l'},
      {"cosine", 0, 0, 'o'},
      {"cubic", 0, 0, 'c'},
      {"hermite", 0, 0, 'h'},
      {"steps", 1, 0, 's'},
      {0, 0, 0, 0}
    };

    int c = getopt_long (argc, argv, "h",
        long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
      case 's':
        numSteps = atoi(optarg);
        break;
      case 'o':
        run = 1;
        break;
      case 'c':
        run = 2;
        break;
      case 'l':
        run = 0;
        break;
      case 'h':
        run = 3;
        break;
      default:
        exit(1);
        break;
    }
  }

  switch(run) {
    case 2:
      run_cubic(numSteps);
      break;
    case 1:
      run_cosine(numSteps);
      break;
    case 0:
      run_linear(numSteps);
      break;
    case 3:
      run_hermite(numSteps, hermite_tension, hermite_bias);
      break;
  }

}
