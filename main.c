#include <stdio.h>

extern void _entrypoint_emu(void);
const char *romname;

/* 
*/

int main(int argc, char **argv)
{
  if (argc == 1) {
    printf("usage: ./<emuname> <romname>\n");
    return -1;
  } else if (argc == 2) {
    romname = *(argv+1);
  }
  _entrypoint_emu();
}
