#ifndef _EMUD_H_
#define _EMUD_H_

#define EMUD_LOG ".debug"

FILE *debug;

SDL_Window    *window;
SDL_GLContext *context;
char           ui_active;
SDL_Event      event;
GLfloat        eTime;
GLuint        *eResolution;

#endif /* _EMUD_H_ */