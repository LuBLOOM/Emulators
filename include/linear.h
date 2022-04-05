#ifndef _EMUD_LINEAR_H_
#define _EMUD_LINEAR_H_

static void inline _mat4_swap(float *, float *);
void mat4_init(float [16]);
void mat4_scale(float [16], float, float, float);
void mat4_translate(float [16], float, float, float);
void mat4_ortho(float [16], float, float, float, float, float, float);
void mat4_transpose(float [16]);

#endif /* _EMUD_LINEAR_H_ */
