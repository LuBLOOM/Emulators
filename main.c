#include <stdio.h>

/*      Author: ELORK 
        repo: www.github.com/ELORK/Emulators.git
        contact: elork01@gmail.com
*/

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

SDL_Window *window;
SDL_Renderer *renderer;

extern void _entrypoint_interface(void);
char romname[1024];

int main(int argc, char **argv)
{
        _entrypoint_interface();
}
