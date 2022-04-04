#ifndef _EMUD_LINEAR_H_
#define _EMUD_LINEAR_H_

static void inline _mat4_swap(float *, float *);
void mat4_init(float *);
void mat4_scale(float *, float, float, float);
void mat4_translate(float *, float, float, float);
void mat4_ortho(float *, float, float, float, float, float, float);
void mat4_transpose(float *);

#endif /* _EMUD_LINEAR_H_ */
