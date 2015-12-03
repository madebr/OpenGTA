#include <stdio.h>
#include <stdlib.h>

void encode();
void decode();
int count = 0;

void usage(const char* prg) {
  printf("USAGE: %s [-e | -d] < src_file > target_file\n", prg);
  exit(0);
}

int main(int argc, char* argv[]) {
  int mode = 0;
  if (argc == 2) {
    if (argv[1][1] == 'd')
      mode = 1;
    if (argv[1][1] == 'e')
      mode = 2;
  }
  else
    usage(argv[0]);

  switch (mode) {
    case 1:
      decode();
      break;
    case 2:
      encode();
      break;
    default:
      usage(argv[0]);
      break;
  }
}

void decode() {
  int c;
  int addVal = 0;
  while ((c = getchar()) != EOF) {
    c--;
    if (count <= 7)
      c -= (0x63 << count);
    if (c == 195) {
      addVal = 64;
//      fprintf(stderr, "addval after %i bytes\n", count);
    }
    else {
      c += addVal;
/*      if (addVal)
        fprintf(stderr, "decoding-addval %i %i\n", c, c - addVal);
      else
        fprintf(stderr, "decoding: %i\n", (unsigned char)c);
*/
      addVal = 0;
      if (c == 0)
        c = '\n';
      putchar(c);
    }
    count++;
  }
}

void encode_out(int v) {
  if (count <= 7)
    v += (0x63 << count);
  v++;
  putchar(v);
}

void encode() {
  int c;
  count = 0;
  while ((c = getchar()) != EOF) {
    if (c == '\n') {
      c = 0;
    }

    if (c >= 192) {
      int _c = 195;
      encode_out(_c);
      c -= 64;
      encode_out(c);
    }
    else {
      encode_out(c);
    }
    count++;
  }
}
