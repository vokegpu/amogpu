#pragma once
#ifndef GPU_HANDLER_H
#define GPU_HANDLER_H

#include "util.hpp"
#include <vector>
#include <gl/glew.h>

struct gpu_gl_program {
	GLuint program = 0;
	bool validation = false;
};

namespace opengl {
	bool compile_shader(GLuint &shader, GLuint mode, const char* src);
	bool create_program(gpu_gl_program &program, const char* vsh_path, const char* fsh_path);
}

struct gpu_data {
	uint32_t factor;
	float color[4];
	float pos[2];

	GLint begin;
	GLint end;

	GLuint texture_slot;
	GLuint texture;
};

struct dynamic_batching {
protected:
	std::vector<float> concurrent_allocated_vertices;
	std::vector<float> concurrent_allocated_texture_coords;

	uint32_t sizeof_allocated_gpu_data;
	uint32_t sizeof_instanced_allocated_gpu_data;

	gpu_data allocated_gpu_data[2048];

	GLuint vertex_arr_object;
	GLuint vbo_vertices;
	GLuint vbo_texture_coords;
public:
	static void init();

	void call();
	void begin();

	void fill(util::vec4f &color);
	void vertex(float x, float y);

	void bind(GLuint texture);
	void coords(float u, float v);

	void end();
	void release();

	void draw();
	void free_buffers();
};

namespace draw {
	void rectangle(float x, float y, float w, float h, util::vec4f &color);
}

#endif