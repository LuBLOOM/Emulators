#ifndef _EMUD_SHADER_H
#define _EMUD_SHADER_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

struct uniforms {
	unsigned modelviewmatrix_loc;
	float   *modelviewmatrix;
	unsigned projectionmatrix_loc;
	float   *projectionmatrix;
	unsigned eTime_loc;
	float    eTime;
	unsigned eResolution_loc;
	unsigned eResolution[2];
};

struct shader {
	struct uniforms u;
	unsigned id;
};

unsigned shader_load(const char *, GLenum);
void shader_slog(unsigned);
void shader_plog(unsigned);

#endif /* _EMUD_SHADER_H */
