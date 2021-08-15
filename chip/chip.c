#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EMU_CHIP_WIDTH 64
#define EMU_CHIP_HEIGHT 32
#define EMU_CHIP_SCREENWIDTH 640
#define EMU_CHIP_SCREENHEIGHT 320
#define EMU_CHIP_RAMSIZE 0x1000
#define EMU_CHIP_STACKSIZE 0x10
#define EMU_CHIP_REGSIZE 0x10
#define EMU_CHIP_ENTRYPOINT 0x200
#define EMU_CHIP_REGSIZE 0x10
#define EMU_CHIP_KEYCOUNT 0x10
#define EMU_CHIP_KEYBYTES 0x50

enum bool {
        true = 1,
        false = 0,
};
typedef enum bool bool;

SDL_Window *window;
SDL_Renderer *renderer;

static unsigned short pc;
static unsigned char sp;
static unsigned char ram[EMU_CHIP_RAMSIZE];
static unsigned int stack[EMU_CHIP_STACKSIZE];
static unsigned short I;
static unsigned char DISPLAY[EMU_CHIP_WIDTH*EMU_CHIP_HEIGHT];
static unsigned int DISPLAY_PIXELS[EMU_CHIP_WIDTH*EMU_CHIP_HEIGHT];
static unsigned char v[EMU_CHIP_REGSIZE];
static unsigned char DT;
static unsigned char ST;
static unsigned short key_state;
static unsigned char EMU_CHIP_KEYS[EMU_CHIP_KEYCOUNT] = {
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_4,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_r,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_f,
        SDLK_z,
        SDLK_x,
        SDLK_c,
        SDLK_v,
};

static unsigned char EMU_CHIP_NUMFONT[EMU_CHIP_KEYBYTES] = {
        0xf0, 0x90, 0x90, 0x90, 0xf0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xf0, 0x10, 0xf0, 0x80, 0xf0,
        0xf0, 0x10, 0xf0, 0x10, 0xf0,
        0x90, 0x90, 0xf0, 0x10, 0x10,
        0xf0, 0x80, 0xf0, 0x10, 0xf0,
        0xf0, 0x80, 0xf0, 0x90, 0xf0,
        0xf0, 0x10, 0x20, 0x40, 0x40,
        0xf0, 0x90, 0xf0, 0x90, 0xf0,
        0xf0, 0x90, 0xf0, 0x10, 0xf0,
        0xf0, 0x90, 0xf0, 0x90, 0x90,
        0xe0, 0x90, 0xe0, 0x90, 0xe0,
        0xf0, 0x80, 0x80, 0x80, 0xf0,
        0xe0, 0x90, 0x90, 0x90, 0xe0,
        0xf0, 0x80, 0xf0, 0x80, 0xf0,
        0xf0, 0x80, 0xf0, 0x80, 0x80,
};

SDL_Texture *texture_display;
SDL_Renderer *renderer;
SDL_Window *window;

void 
_entrypoint_emu_chip(void);

static void
emu_chip_init(void);

static unsigned short 
emu_chip_load_rom(void);

static void
emu_chip_run(void);

static void 
emu_chip_decode_and_execute(void);

static void 
emu_chip_free(void);

static inline unsigned short 
emu_chip_getopcode(unsigned char, unsigned char);

static void 
emu_chip_draw(unsigned char vx, unsigned char vy, unsigned char n);

static void 
emu_chip_showdisplay(void);

static void 
emu_chip_showpixels(void);

static void 
emu_chip_init(void)
{
        SDL_Init(SDL_INIT_VIDEO);
        texture_display = SDL_CreateTexture(renderer, 0, 0,
                EMU_CHIP_WIDTH, EMU_CHIP_HEIGHT);

        window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, EMU_CHIP_SCREENWIDTH,
                EMU_CHIP_SCREENHEIGHT, SDL_WINDOW_SHOWN);

        renderer = SDL_CreateRenderer(window, -1, 0);

        pc = EMU_CHIP_ENTRYPOINT;
        I = sp = ST = DT = key_state = 0;
        unsigned int i;

        for (i = 0; i < EMU_CHIP_RAMSIZE; i++) {
                ram[i] = 0x00;
        }

        for (i = 0; i < EMU_CHIP_KEYBYTES; i++) {
                ram[i] = EMU_CHIP_NUMFONT[i];
        }

        for (i = 0; i < EMU_CHIP_STACKSIZE; i++) {
                stack[i] = 0x00;
        }

        for (i = 0; i < EMU_CHIP_REGSIZE; i++) {
                v[i] = 0x00;
        }

        if (-1 == emu_chip_load_rom()) {
                exit(EXIT_FAILURE);
        }
}

static unsigned short 
emu_chip_load_rom(void)
{
        unsigned int i;

        extern const char *romname;
        FILE *rom = fopen(romname, "rb");

        if (NULL == rom) {
                fprintf(stderr, "Could not open the selected rom\n");
                return -1;
        }

        fseek(rom, 0, SEEK_END);
        size_t size = ftell(rom);
        rewind(rom);

        unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char)*size);

        if (NULL == buffer) {
                fprintf(stderr, "Could not allocate memory for the buffer\n");
                return -1;
        }

        size_t bsize = fread(buffer, sizeof(unsigned char), size, rom);

        if (bsize != size) {
                fprintf(stderr, "Could not read rom into buffer\n");
                return -1;
        }

        if (size < 0xe00) {
                for (i = 0; i < size; i++) {
                        ram[i + EMU_CHIP_ENTRYPOINT] = buffer[i];
                }
        } else {
                fprintf(stderr, "rom too BIG\n");
                return -1;
        }
        fclose(rom);
        free(buffer);
}

static void 
emu_chip_decode_and_execute(void)
{
        unsigned short opcode = emu_chip_getopcode(ram[pc], ram[pc + 1]);

        unsigned int i; bool pressed;

        switch (opcode & 0xf000) {
                case 0x0000:
                        switch (opcode & 0x00ff) {
                                case 0x00e0:
                                        for (i = 0; i < EMU_CHIP_WIDTH*EMU_CHIP_HEIGHT; i++) {
                                                DISPLAY[i] = 0x00;
                                        }
                                        pc += 2;
                                        break;
                                case 0x00ee:
                                        pc = stack[--sp];
                                        pc += 2;
                                        break;
                                default:
                                        fprintf(stdout, "Unsupported opcode\n");
                                        return;
                        }
                        break;
                case 0x1000:
                        pc = opcode & 0x0fff;
                        break;
                case 0x2000:
                        stack[sp++] = pc;
                        pc = opcode & 0x0fff;
                        break;
                case 0x3000:
                        if (v[(opcode & 0xf00)>>8] == (opcode & 0x00ff)) {
                                pc += 4;
                        } else {
                                pc += 2;
                        }
                        break;
                case 0x4000:
                        if (v[(opcode & 0x0f00)>>8] != (opcode & 0x00ff)) {
                                pc += 4;
                        } else {
                                pc += 2;
                        }
                        break;
                case 0x5000:
                        if (v[(opcode & 0x0f00)>>8] == v[(opcode & 0x00f0)>>4]) {
                                pc += 4;
                        } else {
                                pc += 2;
                        }
                        break;
                case 0x6000:
                        v[(opcode & 0x0f00)>>8] = (opcode & 0x00ff);
                        pc += 2;
                        break;
                case 0x7000:
                        v[(opcode & 0x0f00)>>8] += (opcode & 0x00ff);
                        pc += 2;
                        break;
                case 0x8000:
                        switch (opcode & 0x000f) {
                                case 0x0000:
                                        v[(opcode & 0x0f00)>>8] = v[(opcode & 0x00f0)>>4];
                                        pc += 2;
                                        break;
                                case 0x0001:
                                        v[(opcode & 0x0f00)>>8] |= v[(opcode & 0x00f0)>>4];
                                        pc += 2;
                                        break;
                                case 0x0002:
                                        v[(opcode & 0x0f00)>>8] &= v[(opcode & 0x00f0)>>4];
                                        pc += 2;
                                        break;
                                case 0x0003:
                                        v[(opcode & 0x0f00)>>8] ^= v[(opcode & 0x00f0)>>4];
                                        pc += 2;
                                        break;
                                case 0x0004:
                                        if (v[(opcode & 0x0f00)>>8] > v[(opcode & 0x00f0)>>4]) {
                                                v[0xf] = 1;
                                        } else {
                                                v[0xf] = 0;
                                        }
                                        v[(opcode & 0x0f00)>>8] += v[(opcode & 0x00f0)>>4];
                                        pc += 2;
                                        break;
                                case 0x0005:
                                        if (v[(opcode & 0x00f0)>>4] > v[(opcode & 0x0f00)>>8]) {
                                                v[0xf] = 0;
                                        } else {
                                                v[0xf] = 1;
                                        }
                                        v[(opcode & 0x0f00)>>8] -= v[(opcode & 0x00f0)>>4];
                                        pc += 2;
                                        break;
                                case 0x0006:
                                        v[0xf] = v[(opcode & 0x0f00)>>8] & 0x1;
                                        v[(opcode & 0x0f00)>>8] >>= 1;
                                        pc += 2;
                                        break;
                                case 0x0007:
                                        if (v[(opcode & 0x0f00)>>8] > v[(opcode & 0x00f0)>>4]) {
                                                v[0xf] = 0;
                                        } else {
                                                v[0xf] = 1;
                                        }
                                        v[(opcode & 0x0f00)>>8] = v[(opcode & 0x00f0)>>4] - v[(opcode & 0x0f00)>>8];
                                        pc += 2;
                                        break;
                                case 0x000e:
                                        v[0xf] = v[(opcode & 0x0f00)>>8] & 0x80;
                                        v[(opcode & 0x0f00)>>8] <<= 1;
                                        pc += 2;
                                        break;
                                default:
                                        fprintf(stdout, "Unsupported opcode\n");
                                        return;
                        }
                        break;
                case 0x9000:
                        if (v[(opcode & 0x0f00)>>8] != v[(opcode & 0x00f0)>>4]) {
                                pc += 4;
                        } else {
                                pc += 2;
                        }
                        break;
                case 0xa000:
                        I = opcode & 0x0fff;
                        pc += 2;
                        break;
                case 0xb000:
                        pc = (opcode & 0x0fff) + v[0];
                        break;
                case 0xc000:
                        v[(opcode & 0x0f00)>>8] = (rand() % (0x100)) & (opcode & 0x00ff);
                        pc += 2;
                        break;
                case 0xd000:
                        emu_chip_draw(v[(opcode & 0x0f00)>>8], v[(opcode & 0x00f0)>>4], (opcode & 0x000f));
                        pc += 2;
                        break;
                case 0xe000:
                        switch (opcode & 0x00ff) {
                                case 0x009e:
                                        if (key_state&(1<<v[(opcode & 0x0f00)>>8]) != 0) {
                                                pc += 4;
                                        } else {
                                                pc += 2;
                                        }
                                        break;
                                case 0x00a1:
                                        if (key_state&(1<<v[(opcode & 0x0f00)>>8]) == 0) {
                                                pc += 4;
                                        } else {
                                                pc += 2;
                                        }
                                        break;
                                default:
                                        fprintf(stdout, "Unsupported opcode\n");
                        }
                        break;
                case 0xf000:
                        switch (opcode & 0x00ff) {
                                case 0x0007:
                                        v[(opcode & 0x0f00)>>8] = DT;
                                        pc += 2;
                                        break;
                                case 0x000a:
                                        for (i = 0; i < 16; i++) {
                                                if (key_state&(1<<i) != 0) {
                                                        v[(opcode & 0x0f00)>>8] = i;
                                                        pressed = true;
                                                }
                                        }
                                        if (!pressed) {
                                                pressed = false;
                                                return;
                                        }

                                        pc += 2;
                                        break;
                                case 0x0015:
                                        DT = v[(opcode & 0x0f00)>>8];
                                        pc += 2;
                                        break;
                                case 0x0018:
                                        ST = v[(opcode & 0x0f00)>>8];
                                        pc += 2;
                                        break;
                                case 0x001e:
                                        if (I + v[(opcode & 0x0f00)>>8] > 0xfff) {
                                                v[0xf] = 1;
                                        } else {
                                                v[0xf] = 0;
                                        }
                                        I += v[(opcode & 0x0f00)>>8];
                                        pc += 2;
                                        break;
                                case 0x0029:
                                        I = v[(opcode & 0x0f00)>>8] * 0x5;
                                        pc += 2;
                                        break;
                                case 0x0033:
                                        ram[I] = v[(opcode & 0x0f00)>>8] / 100;
                                        ram[I + 1] = (v[(opcode & 0x0f00)>>8]/10)%10;
                                        ram[I + 2] = v[(opcode & 0x0f00)>>8]%10;
                                        pc += 2;
                                        break;
                                case 0x0055:
                                        for (i = 0; i <= ((opcode & 0x0f00)>>8); i++) {
                                                ram[I+i] = v[i];
                                        }

                                        I += ((opcode & 0x0f00)>>8) + 1;
                                        pc += 2;
                                        break;
                                case 0x0065:
                                        for (i = 0; i <= ((opcode & 0x0f00)>>8); i++) {
                                                v[i] = ram[I + i];
                                        }

                                        I += ((opcode & 0x0f00)>>8) + 1;
                                        pc += 2;
                                        break;
                                default:
                                        fprintf(stdout, "Undefined opcode\n");
                        }
                        break;
                default:
                        fprintf(stdout, "Undefined opcode\n");
        }
        if (DT > 0) {
                --DT;
        }
}

static void 
emu_chip_free(void)
{
        unsigned int i;

        I = sp = ST = DT = key_state = 0;

        for (i = 0; i < EMU_CHIP_RAMSIZE; i++) {
                ram[i] = 0x00;
        }

        for (i = 0; i < EMU_CHIP_STACKSIZE; i++) {
                stack[i] = 0x00;
        }

        for (i = 0; i < EMU_CHIP_REGSIZE; i++) {
                v[i] = 0x00;
        }

        SDL_Quit();
}

static inline unsigned short 
emu_chip_getopcode(unsigned char b1, unsigned char b2)
{
        unsigned short opcode = b1;
        return (opcode << 8) | b2;
}

static void
emu_chip_run(void)
{
        unsigned int i; 

        SDL_Event event;
        bool running = true;

        while (running) {
                while (SDL_PollEvent(&event)) {

                        if (event.type == SDL_QUIT) {
                                running = false;
                        }

                        if (event.type == SDL_KEYDOWN) {

                                if (event.key.keysym.sym == SDLK_ESCAPE) {
                                        running = false;
                                }

                                for (i = 0; i < 0x10; i++) {
                                        if (event.key.keysym.sym == EMU_CHIP_KEYS[i]) {
                                                key_state |= (1<<i);
                                        }
                                }
                        }

                        if (event.type == SDL_KEYUP) {

                                for (i = 0; i < 0x10; i++) {
                                        if (event.key.keysym.sym == EMU_CHIP_KEYS[i]) {
                                                key_state &= ~(1<<i);
                                        }
                                }
                        }
                }
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                emu_chip_decode_and_execute();
                for (i = 0; i < 0x800; i++) {
                        unsigned char curr_pixel = DISPLAY[i];
                        DISPLAY_PIXELS[i] = (curr_pixel * 0x00FFFFFF) | 0xFF000000;
                }
                SDL_UpdateTexture(texture_display, NULL, 
                        DISPLAY_PIXELS, 64 * sizeof(unsigned int));

                emu_chip_showpixels();
                SDL_RenderCopy(renderer, texture_display, NULL, NULL);
                SDL_RenderPresent(renderer);
        }
}

static void 
emu_chip_draw(unsigned char vx, unsigned char vy, unsigned char n)
{
        unsigned int i, j;

        v[0xf] = 0;

        for (j = 0; j < n; j++) {
                for (i = 0; i < 0x8; i++) {
                        if ((ram[I + j] & (0x80 >> i)) != 0) {
                                if (DISPLAY[(vx + i + ((j + vy)*EMU_CHIP_WIDTH))] == 1) {
                                        v[0xf] = 1;
                                }
                                DISPLAY[vx + i + ((j + vy)*EMU_CHIP_WIDTH)] ^= 1;
                        }
                }
        }

}

static void 
emu_chip_showdisplay(void)
{
        unsigned int i;

        printf("\n\n=== DISPLAY ===\n\n");
        for (i = 0; i < EMU_CHIP_WIDTH*EMU_CHIP_HEIGHT; i++) {
                if (i % EMU_CHIP_WIDTH == 0) {
                        printf("\n");
                }
                printf("%d", DISPLAY[i]);
        }
}

static void 
emu_chip_showpixels(void)
{
        unsigned int i;

        printf("\n\n=== DISPLAY ===\n\n");
        for (i = 0; i < EMU_CHIP_WIDTH*EMU_CHIP_HEIGHT; i++) {
                if (i % EMU_CHIP_WIDTH == 0) {
                        printf("\n");
                }
                if (DISPLAY_PIXELS[i] == 0xff000000) {
                        printf("0");
                } else {
                        printf("1");
                }
        }
}

void
_entrypoint_emu_chip(void)
{
        emu_chip_init();
        emu_chip_run();
        emu_chip_free();
}