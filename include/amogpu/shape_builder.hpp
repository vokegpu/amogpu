#pragma once
#ifndef SHAPE_BUILDER_H
#define SHAPE_BUILDER_H

#include <gl/glew.h>
#include "core.hpp"

/**
 * Buiild static geometry shape, fast and immediate.
 **/
class shape_builder {
protected:
	static amogpu::gpu_gl_program fx_shape;
	
	static GLuint vertex_array;
	static GLuint vertex_buffer;

	uint16_t enum_flag_format;
	amogpu::gpu_data concurrent_gpu_data;

	float texture_rect[4];
public:
	static void init();
	static void free_buffers();

	void invoke(const uint16_t &format, const amogpu::vec4f &color, GLuint texture = 0);
	void draw(float x, float y, float w, float h);
};

#endif