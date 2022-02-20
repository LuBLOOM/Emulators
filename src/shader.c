#include "../include/shader.h"

unsigned shader_load(const char *filename, GLenum type)
{
	FILE *sf = fopen(filename, "r");
	size_t size = 10000;
	GLchar *shader_src = malloc(size);
	GLuint shader = glCreateShader(type);

	if (!sf) {
		printf("Could not open file: %s\n", filename);
		return 0;
	}

	int i, c;
	for (i = 0; (c = fgetc(sf)) != EOF; i++) {
		*(shader_src+i) = c;
		if (i >= size) {
			size <<= 2;
			char *tmp = (char *)realloc(shader_src, size);
			if (tmp != 0) {
				memcpy(tmp, shader_src, size);
				free(shader_src);
				shader_src = tmp;
			} else {
				printf("Could not reallocate memory for shader buffer\n");
				return 0;
			}
		}
	}
	*(shader_src+i) = '\0';

	glShaderSource(shader, 1, (const GLchar **)&shader_src, NULL);
	glCompileShader(shader);

	GLint shader_success = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_success);
	if (shader_success != GL_TRUE) {
		printf("Unable to compile shader %d\nSource: %s\n", shader, shader_src);
		shader_slog(shader);
		glDeleteShader(shader);
		shader = 0;
	}
	fclose(sf);
	return shader;
}

void shader_slog(unsigned shader)
{
	if (glIsShader(shader)) {
		int log_length = 0;
		int max_length = log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
		char *log = malloc(max_length);
		glGetShaderInfoLog(shader, max_length, &log_length, log);
		if (log_length) {
			printf("%s\n", log);
		}
		free(log);
	} else {
		printf("Name %d is not a shader\n", shader);
	}
}

void shader_plog(unsigned program)
{
	if (glIsProgram(program)) {
		int log_length = 0;
		int max_length = log_length;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
		char *log = malloc(max_length);
		glGetProgramInfoLog(program, max_length, &log_length, log);
		if (log_length) {
			printf("%s\n", log);
		}
		free(log);
	} else {
		printf("Name %d is not a program\n", program);
	}
}
