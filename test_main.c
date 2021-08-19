#include <stdio.h>
#include <string.h>

//used to test the chip 8 emulator
//independent of the interface 

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

SDL_Window *window;
SDL_Renderer *renderer;

extern void 
_entrypoint_emu_chip();

char romname[1024];

int 
main(int argc, char **argv)
{
	if (argc == 1) {
		printf("Usage: ./<emuname> <romname>\n");
		return -1; 
	} else if (argc == 2) {
		memcpy(romname, *(argv+1), 1024);
	}
	_entrypoint_emu_chip();
	return 0;
}