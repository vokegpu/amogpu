#pragma once
#ifndef UTIL_H
#define UTIL_H

#include "font_renderer.hpp"
#include "gpu_handler.hpp"

/**
 * @author Rina
 * @since 28/07/22 at 15:07pm
 * 
 * THE AMOGPU LIBRARY.
 * ALL CREDITS RESERVED TO RINA (aka MrsRina).
 * FREE COMERCIAL USE, YOU NEED USE THE PUBLIC LICENSE.
 **/
namespace amogpu {
	/*
	 * Init amogpu library.
	 */ 
	void init();

	/*
	 * Quit amogpu library.
	 */ 
	void init();

	/*
	 * Update matrices.
	 */
	void matrix();

	/*
	 * Send output log.
	 */
	void log(const std::string &input_str);

	/*
	 * Open and get string data output from file.
	 */
	bool read_file(std::string &input_str, const std::string &path);
	
	/*
	 * Pass to matrix the current window viewport.
	 */
	void viewport(float* mat);

	/*
	 * Pass to matrix the projection view ortho 2D.
	 */
	void projection_view_ortho(float* mat, float left, float right, float bottom, float top);

	/*
	 * Compile shader.
	 */
	bool compile_shader(GLuint &shader, GLuint mode, const char* src);

	/*
     * Create mini-program into GPU.
	 */
	bool create_program(gpu_gl_program &program, const char* vsh_path, const char* fsh_path);

	/*
	 * Rectangle.
	 */
	struct rect {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;

		/*
		 * Detect if the rect is colliding with a point in space.
		 */
		bool aabb_collide_with_point(float x, float y);

		void operator += (util::rect &rect) {
			this->x += rect.x;
			this->y += rect.y;
		}

		void operator -= (util::rect &rect) {
			this->x -= rect.x;
			this->y -= rect.y;
		}
	};

	/*
	 * Vector.
	 */
	struct vec4f {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		vec4f(float x, float y, float z, float w);
	};

	/*
	 * DEPRECATED
	 * Global settings of a clock.
	 */
	struct clock {
		static float dt;
		static uint32_t fps;
	};
}


#endif