#include <cstdlib>
#include <iostream>
#include <string>
#include "m_exceptions.h"
#include "log.h"

extern void parse_args(int argc, char* argv[]);
extern void on_exit();
extern void run_init(const char*);
extern void run_main();

int global_EC = 0;
int global_Done = 0;

#ifndef DONT_CATCH 
bool catch_exceptions = true;
#else
bool catch_exceptions = false;
#endif

using namespace std;

int main(int argc, char* argv[]) {
  if (argc > 1)
    parse_args(argc, argv);

  atexit(on_exit);

  if (!catch_exceptions)
    INFO << "ignoring exceptions" << std::endl;
  
  if (!catch_exceptions)
    run_init(argv[0]);
  else {
    try {
      run_init(argv[0]);
    }
    catch (Exception & e) {
      ERROR << "Exception during startup: " << e.what() << endl;
      global_EC = 1;
      exit(1);
    }
  }
  if (!catch_exceptions)
    run_main();
  else {
    try {
      run_main();
    }
    catch (const Exception & e) {
      ERROR << "Exception during game: " << e.what() << endl;
      global_EC = 1;
      exit(1);
    }

  }
  std::exit(0);
}
