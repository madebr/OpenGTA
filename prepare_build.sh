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
( grep -l "^namespace OpenGL" *.cpp ; echo "gl_frustum.cpp";echo "math/obox.cpp coldet/math3d.cpp" ) | sort | xargs echo "$FOO ="
echo "$FOOO = \${$FOO:.cpp=.o}"
FOO=OGTA_SRC
FOOO=OGTA_OBJ
( grep -l "^namespace OpenGTA" *.cpp ; echo "slope_height_func.cpp" ) | sort | xargs echo "$FOO ="
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
all: loki \${TARGETS}

gfxextract${EXE_PFIX}: gfx_extract.cpp read_gry.o read_g24.o read_cmp.o navdata.o dataholder.o \
\$(UTIL_OBJ)
	\$(CXX) \$(CATCH_E) \$(GFX_DDUMP) \$(FLAGS) \$(DEFS) \$(LINK_LAZY) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(SDL_IMG_LIB) \$(PHYSFS_LIB) \$(LOKI_LIB)

viewer${EXE_PFIX}: main2.cpp viewer.o \$(OGTA_OBJ) \$(GL_OBJ) \$(UTIL_OBJ)
	\$(CXX) \$(CATCH_E) \$(FLAGS) \$(DEFS) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(PHYSFS_LIB) \$(LOKI_LIB) \$(COLDET_LIB)

luaviewer${EXE_PFIX}: main2.cpp viewer.cpp \$(OGTA_OBJ) \$(GL_OBJ) \$(UTIL_OBJ) \
\$(LUA_OBJ)
	\$(CXX) \$(CATCH_E) -DWITH_LUA \$(FLAGS) \$(DEFS) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(PHYSFS_LIB) \$(LOKI_LIB) \$(COLDET_LIB) \$(LUA_LIB)


spriteplayer${EXE_PFIX}: sprite_anim_player.o \$(OGTA_OBJ) \$(GL_OBJ) \$(UTIL_OBJ) main2.cpp
	\$(CXX) \$(CATCH_E) \$(FLAGS) \$(DEFS) \\
  \$(INC) \\
    -o \$@ \$+ \\
      \$(SDL_LIB) \$(SDL_GL_LIB) \$(PHYSFS_LIB) \$(LOKI_LIB) \$(COLDET_LIB)

slopeview: main.o tools/display_slopes.o navdata.o read_cmp.o \
\$(UTIL_OBJ) common_sdl_gl.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB) -lSDL_image

g24: read_g24.cpp read_gry.o \$(UTIL_OBJ)
	\$(CXX) -DG24_DUMPER \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

objdump: tools/obj_dump.cpp read_gry.o \$(UTIL_OBJ) main2.o
	\$(CXX) \$(CXXFLAGS) -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

objdump_map: tools/obj_dump.cpp read_gry.o \$(UTIL_OBJ) main2.o read_cmp.o navdata.o
	\$(CXX) \$(CXXFLAGS) -DDUMP_OBJ_IN_MAP -o \$@ \$+ \$(SDL_LIB) \$(PHYSFS_LIB)

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

function check_physfs () {
  program_exists pkg-config
  if [ $? -eq 1 ]; then
    pkg_config_try_multiple PHYSFS physfs 
  fi
}

function check_compiler () {
  g++ 1>/dev/null 2>&1
  if [ $? -eq 1 ]; then
    CXX=g++
  else
    CXX=
  fi
  gcc 1>/dev/null 2>&1
  if [ $? -eq 1 ]; then
    CC=gcc
  else
    CC=
  fi
}

# defaults

DEBUG=-ggdb
WARN=-Wall
OPT=-O2
if [ "$1" == "LINUX" ]; then
 DEFS="-DLINUX -DDO_SCALEX"
else
 DEFS="-DWIN32 -DDO_SCALEX"
fi
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
DEFS  = $DEFS -DGCC

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
DEFS  = $DEFS -DGCC

# def only for 'main' programs to let gdb handle the exception 
#CATCH_E = -DDONT_CATCH

#GFX_DDUMP = -DDUMP_DELTA_DEBUG

# the external libraries
PHYSFS_INC = -Iinc
PHYSFS_LIB = -Llibs -lphysfs -lz 

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
check_compiler
print_detected
print_make_file_list
print_target_list
cat <<EOF

include depend
EOF
}

if [ "$1" == "LINUX" ]; then
  echo "*** LINUX ***"
  print_all > src_list.make
else
  echo "*** WIN32 ***"
  print_w32settings > src_list.make
  print_make_file_list >> src_list.make
  print_target_list >> src_list.make
fi
