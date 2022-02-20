#include "../include/menu.h"

int menu_init(const char *vertex, const char *fragment)
{
	menu_vertices = (GLfloat *)malloc(12 * sizeof *menu_vertices);
	menu_indices = (GLuint *)malloc(6  * sizeof *menu_indices);

	*(menu_vertices+0) = 0.f;
	*(menu_vertices+1) = 0.f;
	*(menu_vertices+2) = 1.f;
	*(menu_vertices+3) = 0.f;
	*(menu_vertices+4) = 1.f;
	*(menu_vertices+5) = 1.f;
	*(menu_vertices+6) = 1.f;
	*(menu_vertices+7) = 1.f;
	*(menu_vertices+8) = 0.f;
	*(menu_vertices+9) = 1.f;
	*(menu_vertices+10) = 0.f;
	*(menu_vertices+11) = 0.f;

	for (int i = 0; i < 6; i++) {
		*(menu_indices+i) = i;
	}

	glGenBuffers(1, &menu_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, menu_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof *menu_vertices, menu_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &menu_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, menu_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof *menu_indices, menu_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	int shader_pid = glCreateProgram();
	GLuint vertex_shader = shader_load(vertex, GL_VERTEX_SHADER);
	if (0 == vertex_shader) {
		glDeleteProgram(shader_pid);
		shader_pid = 0;
		printf("Error: Vertex shader error\n");
		return -1;
	}
	glAttachShader(shader_pid, vertex_shader);

	GLuint fragment_shader = shader_load(fragment, GL_FRAGMENT_SHADER);
	if (0 == fragment_shader) {
		glDeleteShader(vertex_shader);
		glDeleteProgram(shader_pid);
		shader_pid = 0;
		printf("Error: Fragment shader error\n");
		return -1;
	}
	glAttachShader(shader_pid, fragment_shader);

	glLinkProgram(shader_pid);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	menu_shader.id = shader_pid;

	glUseProgram(menu_shader.id);
	
	menu_shader.u.eTime_loc =            glGetUniformLocation(menu_shader.id, "eTime");
	menu_shader.u.eResolution_loc =      glGetUniformLocation(menu_shader.id, "eResolution");
	menu_shader.u.modelviewmatrix_loc =  glGetUniformLocation(menu_shader.id, "modelviewmatrix");
	menu_shader.u.projectionmatrix_loc = glGetUniformLocation(menu_shader.id, "projectionmatrix");

	menu_shader.u.modelviewmatrix  = (float *)malloc(16 * sizeof *menu_shader.u.modelviewmatrix);
	menu_shader.u.projectionmatrix = (float *)malloc(16 * sizeof *menu_shader.u.projectionmatrix);

	mat4_init(menu_shader.u.modelviewmatrix);
	mat4_scale(menu_shader.u.modelviewmatrix, *(eResolution+0), *(eResolution+1), 1.);
	mat4_ortho(menu_shader.u.projectionmatrix, 0., *(eResolution+0), *(eResolution+1), 0., 100., -100.);
	
	glUniformMatrix4fv(menu_shader.u.projectionmatrix_loc, 1, GL_FALSE, menu_shader.u.projectionmatrix);
	glUniform2iv(menu_shader.u.eResolution_loc, 1, eResolution);
	glUseProgram(0);
	
	return 0;
}

void menu_render(void)
{
	glUseProgram(menu_shader.id);
	
	glUniformMatrix4fv(menu_shader.u.modelviewmatrix_loc, 1, GL_FALSE, menu_shader.u.modelviewmatrix);
	glUniformMatrix4fv(menu_shader.u.projectionmatrix_loc, 1, GL_FALSE, menu_shader.u.projectionmatrix);
	glUniform1f(menu_shader.u.eTime_loc, eTime);
	glUniform2iv(menu_shader.u.eResolution_loc, 1, eResolution);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, menu_vertex_buffer);
	glVertexPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, menu_index_buffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glUseProgram(0);
}

void menu_free(void)
{
	free(menu_vertices);
	free(menu_indices);
	free(menu_shader.u.modelviewmatrix);
	free(menu_shader.u.projectionmatrix);
	glDeleteProgram(menu_shader.id);
}
