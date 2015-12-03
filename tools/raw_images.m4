define(`FIND_AT', `(iname.find("$1") == $2)')dnl
define(`CONDITION', `if ($1) {
  $2
}')dnl
define(`RESULT', `width = $1; height = $2;')dnl
dnl
CONDITION(`FIND_AT(`CUT', 0) && FIND_AT(`.RA', 4)', `RESULT(640, 480)')
CONDITION(`FIND_AT(`F_BMG.RA', 0)', `RESULT(100, 50)')
CONDITION(`FIND_AT(`F_DMA.RA', 0)', `RESULT(78, 109)')
CONDITION(`FIND_AT(`F_LOGO', 0) && FIND_AT(`.RA', 7)', `RESULT(640, 168)')
CONDITION(`FIND_AT(`F_LOWER', 0) && FIND_AT(`.RA', 8)', `RESULT(640, 312)')
CONDITION(`FIND_AT(`F_PLAYN.RA', 0)', `RESULT(180, 50)')
CONDITION(`FIND_AT(`F_PLAY', 0) && FIND_AT(`.RA', 7)', `RESULT(102, 141)')
CONDITION(`FIND_AT(`F_UPPER.RA', 0)', `RESULT(640, 168)')
dnl
dnl   see: http://membres.lycos.fr/dolmen/gta/raw.html
dnl flag2.raw 20x200
dnl joy.raw   19x18
