#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <sys/time.h>

#include "../include/emud.h"
#include "../include/shader.h"
#include "../include/menu.h"

static struct timeval start_time, curr_time;

static void emud_init(const char *, unsigned, unsigned, unsigned, unsigned, unsigned);
static void emud_render(void);
static void emud_free(void);

int main(int argc, char **argv)
{
	emud_init("emud", 1200, 800, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (menu_init("shaders/menu_shader.glvs", "shaders/menu_shader.glfs") < 0) return printf("Could not initialise menu\n");
	emud_render();
	emud_free();
	return 0;
}

static void emud_init(const char *title, unsigned w, unsigned h, unsigned x, unsigned y, unsigned flags)
{
	ui_active = 1;
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(title, x, y, w, h, flags);
	context = SDL_GL_CreateContext(window);
	glewInit();

	glViewport(0.0, 0.0, w, h);
	glClearColor(0.f ,0.f, 0.f, 1.f);

	eTime = 0.f;
	eResolution = (GLuint *)malloc(2 * sizeof *eResolution);
	gettimeofday(&start_time, 0);
	SDL_GetWindowSize(window, eResolution+0, eResolution+1);

	printf("Resolution: %dx%d\n", *(eResolution+0), *(eResolution+1));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void emud_render()
{
	while (ui_active) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				ui_active = 0;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		menu_render();
		SDL_GL_SwapWindow(window);
		
		gettimeofday(&curr_time, 0);
		eTime = (curr_time.tv_sec - start_time.tv_sec) + (curr_time.tv_usec - start_time.tv_usec)/1000000.f;
	}
}

static void emud_free()
{
	menu_free();
	free(eResolution);
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	IMG_Quit();
}
