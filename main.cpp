#include <iostream>
#include <SDL.h>

void on_exit();
void run_main();
void initGL();
void initVideo(int w, int h, int bpp);

int global_EC = 0;
int global_Done = 0;
SDL_Surface* screen = NULL;
SDL_Surface* surface = NULL;

int city_num = 0;

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Fatal error initialising SDL!" << std::endl;
    global_EC = 1;
    exit(1);
  }
  atexit(on_exit);
  if (argc == 2) {
    city_num = atoi(argv[1]);
  }
  SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );
  initVideo(1024, 768, 32);
  initGL();
  
  run_main();
  exit(0);
}
