#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <iostream>
#include <string>

namespace util {
	void log(const std::string &input_str);
	bool read_file(std::string &input_str, const std::string &path);
	
	void viewport(float* mat);
	void projection_view_ortho(float* mat, float left, float right, float bottom, float top);

	struct rect {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;

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

	struct vec4f {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		vec4f(float x, float y, float z, float w);
	};

	struct clock {
		static float dt;
		static uint32_t fps;
	};
}


#endif