#pragma once
#ifndef GPU_HANDLER_H
#define GPU_HANDLER_H

#include "util.hpp"
#include <vector>
#include <gl/glew.h>

struct gpu_gl_program {
	GLuint program = 0;
	bool validation = false;

	void use();
	void end();

	/* Start of uniform setters. */
	void setb(const std::string &uniform_name, bool val);
	void seti(const std::string &uniform_name, int32_t val);
	void setf(const std::string &uniform_name, float val);
	void set2f(const std::string &uniform_name, const float* val);
	void set4f(const std::string &uniform_name, const float* val);
	void setm4f(const std::string &uniform_name, const float* val);
	/* End of uniform setters. */
};

namespace opengl {
	bool compile_shader(GLuint &shader, GLuint mode, const char* src);
	bool create_program(gpu_gl_program &program, const char* vsh_path, const char* fsh_path);
}

struct gpu_data {
	uint32_t factor;
	float color[4];
	float pos[2];

	GLint begin = 0;
	GLint end = 0;

	uint8_t texture_slot = 0;
	GLuint texture = 0;
};

struct dynamic_batching {
protected:
	std::vector<GLfloat> concurrent_allocated_textures;
	std::vector<float> concurrent_allocated_vertices;
	std::vector<float> concurrent_allocated_texture_coords;

	uint32_t sizeof_allocated_gpu_data;
	uint32_t sizeof_previous_allocated_gpu_data;

	uint32_t sizeof_allocated_vertices;
	uint32_t sizeof_instanced_allocated_vertices;

	gpu_data allocated_gpu_data[2048];
	bool should_alloc_new_buffers;
	bool should_not_create_buffers;

	GLuint vertex_arr_object;
	GLuint vbo_vertices;
	GLuint vbo_texture_coords;

	static gpu_gl_program fx_shape;
	static float matrix_view_ortho[16];
	static float matrix_viewport[4];
public:
	static void init();
	static void matrix();

	void invoke();
	void instance(float x, float y, int32_t factor = -1);
	void factor(int32_t factor);

	void fill(const util::vec4f &color);
	void fill(float r, float g, float b, float a = 1.0f);
	void vertex(float x, float y);

	void bind(GLuint texture);
	void coords(float u, float v);

	void next();
	void revoke();

	void draw();
	void free_buffers();
};

namespace draw {
	extern dynamic_batching batch;

	void rectangle(float x, float y, float w, float h, const util::vec4f &color);
}

#endif