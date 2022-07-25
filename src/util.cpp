#include "util.hpp"
#include <fstream>
#include <gl/glew.h>

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
	} else {
		util::log("Could not open file '" + path + "'.");
	}

	return false;
}

void util::viewport(float* mat) {
	glGetFloatv(GL_VIEWPORT, mat);
}

void util::projection_view_ortho(float* mat, float left, float right, float bottom, float top) {
	const float znear = -1.0f;
	const float zfar = 1.0f;
	const float zinv = 1.0f / (zfar - znear);
	const float yinv = 1.0f / (top - bottom);
	const float xinv = 1.0f / (right -  left);

	mat[0] = (2.0f * xinv);
	mat[1] = 0.0f;
	mat[2] = 0.0f;
	mat[3] = 0.0f;

	mat[4] = 0.0f;
	mat[5] = 2.0f * yinv;
	mat[6] = 0.0f;
	mat[7] = 0.0f;

	mat[8] = 0.0f;
	mat[9] = 0.0f;
	mat[10] = -2.0f * zinv;
	mat[11] = 0.0f;

	mat[12] = (-(right + left) * xinv);
	mat[13] = (-(top + bottom) * yinv);
	mat[14] = (-(zfar - znear) * zinv);
	mat[15] = 1.0f;
}

util::vec4f::vec4f(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}