#include "util.hpp"
#include <fstream>

float util::clock::dt = 0.0f;
uint32_t util::clock::fps = 0;

void util::log(const std::string &input_str) {
	std::cout << ("[MAIN] " + input_str).c_str() << std::endl;
}

bool util::read_file(std::string &input_str, const std::string &path) {
	std::ifstream ifs(path.c_str());

	if (ifs.is_open()) {
		std::string string_buffer_builder = "";

		while (getline(ifs, string_buffer_builder)) {
			input_str += "\n" + string_buffer_builder;
		}

		ifs.close();
		return true;
	}

	return false;
}

util::vec4f::vec4f(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}