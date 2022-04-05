#include "../include/chip.h"

/* chip_init - initializes the chip-8 registers and other local variables */
void chip_init(void)
{
	chip_running = 1;
	pc = CHIP_ENTRYPOINT;
	sp = I = DT = ST = chip_shader = 0;

	glGenTextures(1, &chip_texture);
	glBindTexture(GL_TEXTURE_2D, chip_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHIP_WIDTH, CHIP_HEIGHT,
		     0, GL_RGBA, GL_UNSIGNED_BYTE, chip_display);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
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

	memset(chip_display, 0xffffffff, sizeof chip_display);
	
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
		if (event.type == SDL_QUIT) {
			chip_running = 0;
		}
	}
}

/* chip_render - renders the chip-8 display to the openGL context in the form of a shader. 
                 (work in progress)*/
void chip_render(void)
{
	glBindTexture(GL_TEXTURE_2D, chip_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CHIP_WIDTH, CHIP_HEIGHT,
			GL_RGBA, GL_UNSIGNED_BYTE, chip_display);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
	glTexCoord2f(1.f, 0.f); glVertex2f(EMUD_SCREENWIDTH, 0.f);
	glTexCoord2f(1.f, 1.f); glVertex2f(EMUD_SCREENWIDTH, EMUD_SCREENHEIGHT);
	glTexCoord2f(0.f, 1.f); glVertex2f(0.f, EMUD_SCREENHEIGHT);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* chip_free - frees any memory allocated during the up-time of the chip-8 */
void chip_free(void)
{
	glDeleteTextures(1, &chip_texture);
	pc = sp = I = DT = ST = chip_shader = chip_running = chip_texture = 0;
}
