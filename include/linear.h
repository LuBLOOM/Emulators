#ifndef _EMUD_LINEAR_H_
#define _EMUD_LINEAR_H_

/* These functions have defaulted to column major order for ordering the matrices */
void mat4_init(float *);
void mat4_scale(float *, float, float, float);
void mat4_translate(float *, float, float, float);
void mat4_ortho(float *, float, float, float, float, float, float);

#endif /* _EMUD_LINEAR_H_ */
