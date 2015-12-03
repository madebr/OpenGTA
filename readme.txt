~-~-~ OGTA is OpenGTA ~-~-~

= Required software =
* OpenGL
  [ http://www.opengl.org/ ]
* SDL ( & SDL_image )
  [ http://www.libsdl.org/ ]
* PhysicsFS
  [ http://www.icculus.org/physfs/ ]
* Loki
  [ http://sourceforge.net/projects/loki-lib/ ]

* g++ ( GNU C++ )
  [ http://gcc.gnu.org/ ]
* GNU Make
  [ http://www.gnu.org/software/make/ ]

= Compiling =

Only tested on GNU Linux using gcc & make; I assume that GL, SDL and 
PhysicsFS are installed, Loki is downloaded and compiled automatically.

Just run 'make' or specifically 'make viewer'.

The other programs are/were used for development or debugging.

= Installing the data-files =

You can download the game from http://www.rockstargames.com/classics/ .

These programs expect to find the data in the current directory, either
directly in the file-system or in a ZIP file (named 'gtadata.zip').
You want the content of the original game directory GTADATA, but not
the directory itself.
See mods/using_mods.txt for a slightly longer description.

Needed:
*.FXT, *.FON, *.CMP

STYLE*.GRY (for 8-bit graphics)
STYLE*.G24 (for 24-bit graphics)

= Running =

== gfxextract ==

Export/Display textures and sprites;
run ./gfxextract -h (or without any parameters) for usage info.

== viewer ==

Brain-dead immediate-mode renderer of the city (now with objects); with
plenty of bugs...

start as:
./viewer [flags] [0-2]

The optional param loads the respective city; default is 0:
0 - NYC.CMP
1 - SANB.CMP
2 - MIAMI.CMP

flags are:
* screen dimensions (have to specify neither or both)
-w width
-h height

* log verbosity
-l level (0 default; everything, 1 warn + error, 2 only error)

Using "-l 1" will make it easier to see actual error messages;
sometimes there is a lot of noise on 'info' level (0).

* style-file
-c 0 (default; uses .GRY 8bit styles)
-c 1 (uses .G24 24bit styles)

* other map & graphics files (have to specify neither or both)
-m map_filename -g style_filename

This is for loading non-GTA1 data; like GTA London.

keys:

ESC, cursor-keys, + and - do what you might except them to do;
furthermore:
  x       : pseudo-3d view
  z       : top-down view
  .       : decrease visible range
  ,       : increase visible range
  t       : display entire city (at a crawl)
  f       : toggle fullscreen/windowed
  PRINT   : save 'screenshot.bmp' in current directory
  p       : dump coords (in lua syntax) to stdout
  i,j,k,l : move player-char
  F2      : toggle drawing of sprite bounding-boxes
  F3      : toggle marking of sprite tex-border-boxes
  F4      : toggle free-move vs. follow-player
  F5      : toggle drawing of road heading arrows
  F6      : city map mode (ESC to exit, +, -, cursor keys)
  1 - 8   : choose a specific player-sprite animation
  9, 0    : set the player-sprite to a specific frame

in 3d view:
  F1      : demo-flight over the city
  w       : forward
  s       : backward
  space   : stop
  r       : toggle rotate cam (when not moving)
  g       : toggle ~gravity~ affect on 3d-cam

You can move the view with the mouse; when you switch
to 3d and the screen is black: move the mouse down.

== luaviewer: viewer + Lua (optional target) ==

Also needs Lua (only 5.1 tried) in path; run: 'make luaviewer' 
then start it like with:
  ./luaviewer -s scripts/demo1.lua [0-2]

See the scripts for a little documentation.

= License & Credits =

This is _not_ free software, as it must not be used for commercial
applications. [ http://www.opensource.org/docs/definition.php ]
Please read license.txt for the details.

Author: tok@openlinux.org.uk

Special thanks to:

Jernej 'Delfi' L. (jernejcoder@gmail.com) for providing the vertex-data
and invaluable help concerning several of the file formats.
