#ifndef _EMUD_H_
#define _EMUD_H_

#define EMUD_LOG ".debug"
#define EMUD_SCREENWIDTH 1280
#define EMUD_SCREENHEIGHT 640

FILE *debug;

SDL_Window    *window;
SDL_GLContext *context;
char           ui_active;
SDL_Event      event;
GLfloat        eTime;
GLuint        *eResolution;
float          modelview[16];
float          projection[16];

#endif /* _EMUD_H_ */
