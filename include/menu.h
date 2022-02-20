#ifndef _EMUD_MENU_H
#define _EMUD_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "./emud.h"
#include "./shader.h"
#include "./linear.h"

struct shader menu_shader;
GLfloat      *menu_vertices;
GLuint        menu_vertex_buffer;
GLuint       *menu_indices;
GLuint        menu_index_buffer;

int menu_init(const char *, const char *);
void menu_render();
void menu_free();

#endif /* _EMUD_MENU_H */
