#include "../include/chip.h"

/* chip_init - initializes the chip-8 registers and other local variables */
void chip_init(void)
{
	chip_running = 1;
	pc = CHIP_ENTRYPOINT;
	sp = I = DT = ST = chip_shader = 0;
}

/* 
   chip_load - loads the ROM into the chip-8 RAM, preparing for execution.. 
   returns 0 on success, -1 otherwise.
*/
int chip_load(const char *rom)
{
	FILE *fp = fopen(rom, "rb");
	if (fp == NULL) {
		fprintf(debug, "[CHIP]: '%1$s' does not exist\n", rom);
		return -1;
	}

	
	int c, i = CHIP_ENTRYPOINT; while ((c = fgetc(fp)) != EOF && i < CHIP_RAMSIZE) *(ram+(i++)) = c;
	if (i == CHIP_RAMSIZE) {
		fprintf(debug, "[CHIP]: '%1$s' is too big!\n", rom);
		return -1;
	}

	
	fclose(fp);
	return 0;
}

/* chip_exec - main loop of the chip emulator */
void chip_exec(void)
{
	while (chip_running) {
		glClear(GL_COLOR_BUFFER_BIT);
		chip_event();
		chip_render();
		SDL_GL_SwapWindow(window);
	}
	chip_free();
}

/* chip_event - handles any events whilst running the chip emulator */
void chip_event(void)
{
	while (SDL_PollEvent(&event)) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			chip_running = 0;
		}
	}
}

/* chip_render - renders the chip-8 display to the openGL context in the form of a shader. */
void chip_render(void)
{
	/* currently... nothing is being rendered :( */
}

/* chip_free - frees any memory allocated during the up-time of the chip-8 */
void chip_free(void)
{
	pc = sp = I = DT = ST = chip_shader = chip_running = 0;
}
