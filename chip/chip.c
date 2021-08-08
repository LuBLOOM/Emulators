#include <SDL2/SDL.h>

/*  CHIP 8 Emulator. Written by ELORK 
    repo: github.com/ELORK/Emulators
    email: elork01@gmail.com
    Info Source: https://en.wikipedia.org/wiki/CHIP-8
*/

#define CHIP_RAMSIZE    0x1000
#define CHIP_ENTRYPOINT 0x200
#define CHIP_REGCOUNT   0x10
#define CHIP_WIDTH      640
#define CHIP_HEIGHT     320

enum bool {
     true = 1,
     false = 0,
};
typedef enum bool bool;

static unsigned int     pc;
static unsigned char    ram[CHIP_RAMSIZE];
static unsigned char    v[CHIP_REGCOUNT];
static unsigned short   I;
static unsigned int     sp;

SDL_Window *window;
SDL_Renderer *renderer;

static void emu_init(void);
static void emu_run(void);
static void emu_free(void);

void _entrypoint_emu(void);

static void emu_init(void)
{
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("CHIP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			    CHIP_WIDTH, CHIP_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, 0, -1);
  
  int i;
  pc = CHIP_ENTRYPOINT;
}

static void emu_run(void)
{
  SDL_Event event;
  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
	running = false;
      }
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
  }
}

static void emu_free(void)
{
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  window = NULL;
  renderer = NULL;
  SDL_Quit();
}

void _entrypoint_emu(void)
{
  emu_init();
  emu_run();
  emu_free();
}
