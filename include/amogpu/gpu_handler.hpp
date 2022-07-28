#pragma once
#ifndef GPU_HANDLER_H
#define GPU_HANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <gl/glew.h>

/**
 * Mini-programs on GPU.
 **/
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

/**
 * Store GPU data.
 **/
struct gpu_data {
	uint32_t factor;
	float color[4];
	float pos[2];

	GLint begin = 0;
	GLint end = 0;

	uint8_t texture_slot = 0;
	GLuint texture = 0;
};

/**
 * Batch but dynamic, high performance.
 **/
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
	/*
	 * Init the dynamic batching.
	 */
	static void init();

	/*
	 * Update global matrices.
	 */
	static void matrix();

	/*
	 * Invoke GPU.
	 */
	void invoke();

	/*
	 * Create a sub-section into GPU section.
	 */
	void instance(float x, float y, int32_t factor = -1);

	/*
	 * For complex shapes as string or tile.
	 */
	void factor(int32_t factor);

	/*
	 * Fill with RGBA color normalised.
	 */
	void fill(const util::vec4f &color);

    /*
	 * Fill with RGBA color normalised.
	 */
	void fill(float r, float g, float b, float a = 1.0f);

    /*
	 * Add one vertex.
	 */
	void vertex(float x, float y);

    /*
	 * Bind a texture.
	 */
	void bind(GLuint texture);

	/*
	 * Add one uv coordinates for texture, if there is not any binded texture just put 0.0f at uv(s) parameters.
	 */
	void coords(float u, float v);

	/*
	 * End the sub-segment.
	 */
	void next();

	/*
	 * End GPU communication.
	 */
	void revoke();

	/*
	 * Draw the current batch.
	 */
	void draw();

	/*
	 * Delete the buffers of batch.
	 */
	void free_buffers();
};

#endif