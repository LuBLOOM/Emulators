#include <SDL2/SDL.h>
#include <stdio.h>

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

#define printop(x) printf("%4x\n", x)

enum bool {
     true = 1,
     false = 0,
};
typedef enum bool bool;

typedef unsigned short _chipopcode;
typedef unsigned char u8;

static unsigned int     pc;
static unsigned char    ram[CHIP_RAMSIZE];
static unsigned char    v[CHIP_REGCOUNT];
static unsigned short   I;
static unsigned int     sp;

SDL_Window *window;
SDL_Renderer *renderer;

static void emu_init(void);
static void emu_chip_nextinstr(void);
static _chipopcode emu_chip_getopcode(u8, u8);
static void emu_chip_decode_and_execute(_chipopcode);
static int emu_chip_readrom(const char *);
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

  for (i = 0; i < CHIP_RAMSIZE; i++) {
    ram[i] = 0;
  }

  extern const char *romname;
  emu_chip_readrom(romname);
}

static _chipopcode emu_chip_getopcode(u8 b1, u8 b2)
{
  _chipopcode opcode = b2;
  return (opcode<<7) | b1;
}

static int emu_chip_readrom(const char *path)
{
  int i, c;
  FILE* fp = fopen(path, "rb");

  for (i = 0; (c = getc(fp)) != EOF; i++) {
    ram[pc+i] = c;
  } 
  
  fclose(fp);

  sp = pc + i+1;
  
  //size of the rom
  return i+1;
}

static void emu_chip_decode(_chipopcode opcode)
{
  switch (opcode & 0xf000) {
  case 0x0000:
    if (opcode == 0x00e0) {
      SDL_RenderClear(renderer);
    } else if (opcode == 0x00ee) {
      ram[sp--] = pc;
    }
    break;
  case 0x1000:
    ram[++sp] = pc;
    break;
  }
}

static void emu_chip_nextinstr(void)
{
  if (pc > 0xfff) {
    pc = CHIP_ENTRYPOINT;
  }
  _chipopcode opcode = emu_chip_getopcode(ram[pc++], ram[pc++]);
  emu_chip_decode(opcode);
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
    emu_chip_nextinstr();
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
