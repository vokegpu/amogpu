#include "amogpu/amogpu.hpp"
#include <fstream>

float amogpu::clock::dt = 0.0f;
uint32_t amogpu::clock::fps = 0;

bool amogpu::rect::aabb_collide_with_point(float x, float y) {
	return x > this->x && x < this->x + this->w && y > this->y && y < this->y + this->h;
}

void amogpu::log(const std::string &input_str) {
	std::cout << ("[MAIN] " + input_str).c_str() << std::endl;
}

bool amogpu::read_file(std::string &input_str, const std::string &path) {
	std::ifstream ifs(path.c_str());

	if (ifs.is_open()) {
		std::string string_buffer_builder = "";

		while (getline(ifs, string_buffer_builder)) {
			input_str += "\n" + string_buffer_builder;
		}

		ifs.close();
		return true;
	} else {
		amogpu::log("Could not open file '" + path + "'.");
	}

	return false;
}

void amogpu::viewport(float* mat) {
	glGetFloatv(GL_VIEWPORT, mat);
}

void amogpu::projection_view_ortho(float* mat, float left, float right, float bottom, float top) {
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
	mat[14] = (-(zfar + znear) * zinv);
	mat[15] = 1.0f;
}

amogpu::vec4f::vec4f(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}