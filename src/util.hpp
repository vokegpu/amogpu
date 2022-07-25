#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <iostream>
#include <string>

namespace util {
	void log(const std::string &input_str);
	bool read_file(std::string &input_str, const std::string &path);

	struct rect {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
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