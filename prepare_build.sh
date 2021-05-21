#!/bin/bash

function program_exists() {
  $1 $2 1>/dev/null 2>&1
  if [ $? -eq 127 ]; then
    return 0
  fi
  upname=$(echo $1 | tr '[a-z]' '[A-Z]' | tr '-' '_')
  eval "$upname=$1"
  return 1
}

function print_make_file_list() {
FOO=GL_SRC
FOOO=GL_OBJ
( grep -l "^namespace OpenGL" *.cpp ;echo "gl_frustum.cpp";echo "math/obox.cpp math/plane.cpp coldet/math3d.cpp util/physfsrwops.c" ) | sort | xargs echo "$FOO ="
echo "$FOOO = \${$FOO:.cpp=.o}"
FOO=OGTA_SRC
FOOO=OGTA_OBJ
( grep -l "^namespace OpenGTA" *.cpp ; echo "slope_height_func.cpp" )|grep -v viewer.cpp |grep -v sprite_anim_player.cpp| sort | xargs echo "$FOO ="
echo "$FOOO = \${$FOO:.cpp=.o}"

UTIL_SRC=$(ls util/*.cpp | grep -v color.cpp | grep -v sound | xargs echo)
SOUND_SRC=$(ls util/*.cpp | grep sound.* | xargs echo)
LUA_SRC=$(ls lua_addon/*.cpp | xargs echo)

cat <<EOF


# list of used source files
UTIL_SRC  = $UTIL_SRC
UTIL_OBJ  = \${UTIL_SRC:.cpp=.o}
LUA_SRC   = $LUA_SRC
LUA_OBJ   = \${LUA_SRC:.cpp=.o}
SOUND_SRC = $SOUND_SRC
SOUND_OBJ = \${SOUND_SRC:.cpp=.o}

OSTEER_SRC = opensteer/src/Clock.cpp
OSTEER_OBJ = \${OSTEER_SRC:.cpp=.o}

SOUND_SRC = util/sound_device.cpp util/sound_fx_cache.cpp util/sound_music_player.cpp \
util/sound_resample2.cpp util/sound_system.cpp
SOUND_OBJ = \$(SOUND_SRC:.cpp=.p)

EOF
}

if [ "$1" == "WIN32" ]; then
  EXE_PFIX=.exe
fi

function print_target_list() {
cat <<EOF

# list of make targets (programs)

TARGETS = viewer${EXE_PFIX}
LUA_TARGETS = luaviewer${EXE_PFIX}
all: \${TARGETS}

gfxextract${EXE_PFIX}: gfx_extract.cpp read_gry.o read_g24.o read_cmp.o navdata.o dataholder.o \
\$(UTIL_OBJ)
	\$(CXX) \$(CATCH_E) \$(GFX_DDUMP) \$(FLAGS) \$(DEFS) \$(LINK_LAZY) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(SDL_IMG_LIB) \$(PHYSFS_LIB)

viewer${EXE_PFIX}: main2.cpp viewer.o \$(OGTA_OBJ) \$(GL_OBJ) \$(UTIL_OBJ) \$(OSTEER_OBJ)
	\$(CXX) \$(CATCH_E) \$(FLAGS) \$(DEFS) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(PHYSFS_LIB) \$(COLDET_LIB)

luaviewer${EXE_PFIX}: main2.cpp viewer.o \$(OGTA_OBJ) \$(GL_OBJ) \$(UTIL_OBJ) \$(OSTEER_OBJ) \
\$(LUA_OBJ)
	\$(CXX) \$(CATCH_E) \$(FLAGS) \$(DEFS) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(PHYSFS_LIB) \$(COLDET_LIB) \$(LUA_LIB)


spriteplayer${EXE_PFIX}: sprite_anim_player.o \$(OGTA_OBJ) \$(GL_OBJ) \$(UTIL_OBJ) \$(OSTEER_OBJ) main2.cpp
	\$(CXX) \$(CATCH_E) \$(FLAGS) \$(DEFS) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(PHYSFS_LIB) \$(COLDET_LIB)

slopeview: main.o tools/display_slopes.o navdata.o read_cmp.o \
\$(UTIL_OBJ) common_sdl_gl.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB) -lSDL_image

blockview: main2.o tools/blockview.cpp util/log.o util/m_exceptions.o \
gl_camera.o gl_screen.o read_cmp.o dataholder.o slope_height_func.o navdata.o \
datahelper.o read_gry.o read_g24.o util/set.o util/buffercache.o read_fxt.o blockdata.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(SDL_GL_LIB) \$(PHYSFS_LIB)

g24: read_g24.cpp read_gry.o \$(UTIL_OBJ)
	\$(CXX) -DG24_DUMPER \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

objdump: tools/obj_dump.cpp read_gry.o \$(UTIL_OBJ) main2.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

objdump_map: tools/obj_dump.cpp read_gry.o \$(UTIL_OBJ) main2.o read_cmp.o navdata.o
	\$(CXX) \$(CXXFLAGS) -DDUMP_OBJ_IN_MAP -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

car_dump: tools/car_dump.cpp dataholder.o read_gry.o read_cmp.o read_g24.o navdata.o \
main2.o read_fxt.o util/set.o util/buffercache.o util/log.o util/m_exceptions.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

plane_test: tests/test_plane.cpp math/line_intersect.o util/log.o \
util/m_exceptions.o util/cell_iterator.o util/set.o util/buffercache.o \
read_cmp.o datahelper.o navdata.o read_fxt.o read_gry.o read_g24.o \
dataholder.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

lua_map_test: tests/lua_map_test.o util/file_helper.o util/log.o \
util/m_exceptions.o  util/buffercache.o  util/set.o navdata.o \
dataholder.o read_cmp.o read_gry.o read_g24.o read_fxt.o datahelper.o \
main2.o lua_addon/lua_map.o lua_addon/lua_vm.o lua_addon/lua_stackguard.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB) \$(LUA_LIB)

sound_test: read_sdt.o util/m_exceptions.o util/sound_resample2.o \
util/sound_device.o util/sound_system.cpp util/sound_fx_cache.o \
util/sound_music_player.o util/physfsrwops.c util/log.o
	\$(CXX) -DSOUND_TEST \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB) \$(AUDIO_LIB)
EOF
}

#print_make_file_list > src_list.make
#print_target_list >> src_list.make

function pkg_config_haslib () {
  $PKG_CONFIG $1 1>/dev/null 2>&1
  if [ $? -eq 0 ]; then
    return 1
  fi
  return 0
}

function check_sdl () {
  program_exists sdl-config
  if [ $? -eq 1 ]; then
    SDL_INC=$($SDL_CONFIG --cflags)
    SDL_LIB=$($SDL_CONFIG --libs)
  else
    program_exists pkg-config
    if [ $? -eq 1 ]; then
      pkg_config_try_multiple SDL SDL sdl
    fi
  fi
  cat <<EOF >_out_$$.cpp
#include <SDL.h>
int main(int argc, char* argv[]) {
  SDL_GL_SWAP_CONTROL;
}
EOF
  g++ $SDL_INC _out_$$.cpp 2>/dev/null
  if [ $? -eq 0 ]; then
    SDL_GL_SWAP_CONTROL='#define HAVE_SDL_VSYNC'
  else
    SDL_GL_SWAP_CONTROL='#undef HAVE_SDL_VSYNC'
  fi
  rm -f _out_$$.cpp a.out
}

function pkg_config_try_multiple () {
  local _prefixName=$1
  shift
  while [ $# -gt 0 ]; do
    pkg_config_haslib $1
    if [ $? -eq 1 ]; then
      eval "${_prefixName}_INC=\$($PKG_CONFIG --cflags $1)"
      eval "${_prefixName}_LIB=\$($PKG_CONFIG --libs $1)"
      return
    fi
    shift
  done
}

function check_lua () {
  program_exists pkg-config
  if [ $? -eq 1 ]; then
    pkg_config_try_multiple LUA lua5.1 lua51 lua5 lua
  else
    program_exists lua-config
    if [ $? -eq 1 ]; then
      LUA_INC=$($LUA_CONFIG --include)
      LUA_LIB=$($LUA_CONFIG --libs)
    fi
  fi
}

function check_sdl_audio () {
local good=0
cat <<EOF >_out_$$.cpp
#include <SDL_mixer.h>
int main(int argc, char* argv[]) {
}
EOF
g++ $SDL_INC _out_$$.cpp 2>/dev/null
if [ $? -eq 0 ]; then
  let good=$good+1
fi
rm -f _out_$$.cpp

cat <<EOF >_out_$$.cpp
#include <SDL_sound.h>
int main(int argc, char* argv[]) {
}
EOF
g++ $SDL_INC _out_$$.cpp 2>/dev/null
if [ $? -eq 0 ]; then
  let good=$good+1
fi
rm -f _out_$$.cpp a.out

if [ $good -eq 2 ]; then
  AUDIO_LD="-lSDL_mixer -lSDL_sound"
  SDL_SOUND_MIXER='#define WITH_SOUND'
else
  SDL_SOUND_MIXER='#undef WITH_SOUND'
fi
}

function check_physfs () {
  program_exists pkg-config
  if [ $? -eq 1 ]; then
    pkg_config_try_multiple PHYSFS physfs 
  fi
}

function check_compiler () {
  if [ "$OGTA_PLATFORM" == "WIN32" ]; then
    _CXX=i586-mingw32msvc-g++
    _CC=i586-mingw32msvc-gcc
  else
    _CXX=g++
    _CC=gcc
  fi
  $_CXX 1>/dev/null 2>&1
  if [ $? -eq 1 ]; then
    CXX=$_CXX
  else
    CXX=
  fi
  $_CC 1>/dev/null 2>&1
  if [ $? -eq 1 ]; then
    CC=$_CC
  else
    CC=
  fi
  GCC_VERSION=$($CXX --version | head -n 1)
}

# defaults

DEBUG=-ggdb
WARN=-Wall
OPT=-O2
PHYSFS_LIB=-lphysfs
SDL_LIB=-lSDL
LUA_LIB=-llua51

function print_detected() {
  cat <<EOF
# using these compilers
CXX = $CXX
CC  = $CC

DEBUG = $DEBUG
OPT   = $OPT
WARN  = $WARN
DEFS  = $DEFS

# def only for 'main' programs to let gdb handle the exception 
#CATCH_E = -DDONT_CATCH

#GFX_DDUMP = -DDUMP_DELTA_DEBUG

# the external libraries
PHYSFS_INC = $PHYSFS_INC
PHYSFS_LIB = $PHYSFS_LIB

SDL_INC    = $SDL_INC
SDL_LIB    = $SDL_LIB

SDL_GL_LIB  = -lGL -lGLU
SDL_IMG_LIB = -lSDL_image

# this better be lua >= 5.1 (but not 5.0); not detected
LUA_INC    = $LUA_INC
LUA_LIB    = $LUA_LIB

AUDIO_INC  =
AUDIO_LIB   = $AUDIO_LD

LINK_LAZY  = -Xlinker --unresolved-symbols -Xlinker ignore-all

EOF
}

function print_w32settings() {
  cat <<EOF
# using these compilers
CXX = i586-mingw32msvc-g++
CC  = i586-mingw32msvc-gcc

#DEBUG = $DEBUG
OPT   = $OPT
WARN  = $WARN
DEFS  = $DEFS

LIB_RT_PATH = libs

# def only for 'main' programs to let gdb handle the exception 
#CATCH_E = -DDONT_CATCH

#GFX_DDUMP = -DDUMP_DELTA_DEBUG

# the external libraries
PHYSFS_INC = -Iinc
PHYSFS_LIB = -Llibs -lphysfs-1-1-0 -lzlib1 

SDL_INC    = -Iinc -D_GNU_SOURCE=1 -D_REENTRANT
SDL_LIB    = -Llibs -lSDLmain -lSDL

SDL_GL_LIB  = -lopengl32 -lglu32
SDL_IMG_LIB = -lSDL_image

# this better be lua >= 5.1 (but not 5.0); not detected
LUA_INC    = $LUA_INC
LUA_LIB    = $LUA_LIB

# good idea here?
LINK_LAZY  = -Xlinker --unresolved-symbols -Xlinker ignore-all

EOF
}

function print_all() {
check_sdl
check_lua
check_physfs
check_sdl_audio
print_detected
print_make_file_list
print_target_list
cat <<EOF

include depend
EOF
}

function print_config_h() {
  if [ -e ogta_version ]; then
    LAST_TOUCHED=$(cat ogta_version)
  else
    LAST_TOUCHED=$(find . -type f -exec ls -tl {} \; | grep -v CVS | tail -1 | awk '{print $6}')
    echo $LAST_TOUCHED > ogta_version
  fi

  cat <<EOF
// WIN32 already defined when cross-compiling...
#ifndef $OGTA_PLATFORM
#define $OGTA_PLATFORM
#endif

// platform specific switches
#ifdef LINUX
#define OGTA_PLATFORM_INFO "linux"
// for coldet; FIXME: really only for linux?
#define GCC

#elif WIN32
#define OGTA_PLATFORM_INFO "win32"
#else
#error(No platform defined)
#endif

// global compile-time vars
#define OGTA_VERSION_INFO "$LAST_TOUCHED"

#define DEFAULT_SCREEN_WIDTH  640
#define DEFAULT_SCREEN_HEIGHT 480

// 0 - no vsync (default), 1 - try to use SDL_GL_SWAP_CONTROL,
// 2 - try native call (GLX on linux, unsupported on win32)
//#undef DEFAULT_SCREEN_VSYNC
#define DEFAULT_SCREEN_VSYNC 2

// default pathes; env-variables can override
#define OGTA_DEFAULT_DATA_PATH "gtadata.zip"
#define OGTA_DEFAULT_MOD_PATH  ""
#define OGTA_DEFAULT_HOME_PATH PHYSFS_getBaseDir()

// just for fun
#define USED_GCC_VERSION "$GCC_VERSION"

// enable features
#define DO_SCALE2X
$SDL_SOUND_MIXER
$SDL_GL_SWAP_CONTROL

// use escape sequences to mark Log::info/warn/error()
#ifdef LINUX
#define LOG_USE_ANSI_COLORS
#endif
EOF
}

if [[ -n "$1" && "$1" != "LINUX" ]]; then
  OGTA_PLATFORM=WIN32
  echo "*** WIN32 ***"
  DEFS="-include config.h"
  check_compiler
  print_w32settings > src_list.make
  print_make_file_list >> src_list.make
  print_target_list >> src_list.make
  SDL_GL_SWAP_CONTROL='#define HAVE_SDL_VSYNC'
else
  OGTA_PLATFORM="LINUX"
  echo "*** LINUX ***"
  check_compiler
  DEFS="-include config.h"
  print_all > src_list.make
fi
print_config_h > config.h
