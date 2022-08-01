#include <fstream>
#include "amogpu/gpu_handler.hpp"
#include "amogpu/font_renderer.hpp"
#include "amogpu/shape_builder.hpp"

uint8_t amogpu::dynamic = 1;
uint8_t amogpu::invoked = 2;

float amogpu::matrix_viewport[4];
float amogpu::matrix_projection_ortho[16];

void amogpu::gpu_gl_program::use() {
	glUseProgram(this->program);
}

void amogpu::gpu_gl_program::end() {
	glUseProgram(0);
}

void amogpu::gpu_gl_program::setb(const std::string &uniform_name, bool val) {
	glUniform1i(glGetUniformLocation(this->program, uniform_name.c_str()), (int32_t) val);
}

void amogpu::gpu_gl_program::seti(const std::string &uniform_name, int32_t val) {
	glUniform1i(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void amogpu::gpu_gl_program::setf(const std::string &uniform_name, float val) {
	glUniform1f(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void amogpu::gpu_gl_program::set2f(const std::string &uniform_name, const float* val) {
	glUniform2fv(glGetUniformLocation(this->program, uniform_name.c_str()), GL_TRUE, val);
}

void amogpu::gpu_gl_program::set4f(const std::string &uniform_name, const float* val) {
	glUniform4fv(glGetUniformLocation(this->program, uniform_name.c_str()), GL_TRUE, val);
}

void amogpu::gpu_gl_program::setm4f(const std::string &uniform_name, const float* val) {
	glUniformMatrix4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, GL_FALSE, val);
}

void amogpu::init() {
	dynamic_batching::init();
	font_renderer::init();
	shape_builder::init();
}

void amogpu::quit() {
	font_renderer::end_ft_library();
	shape_builder::free_buffers();
}

void amogpu::matrix() {
	dynamic_batching::matrix();

	// Calculate the matrixes.
	amogpu::viewport(amogpu::matrix_viewport);
	amogpu::projection_view_ortho(amogpu::matrix_projection_ortho, 0.0f, amogpu::matrix_viewport[2], amogpu::matrix_viewport[3], 0.0f);
}

bool amogpu::rect::aabb_collide_with_point(float x, float y) {
	return x > this->x && x < this->x + this->w && y > this->y && y < this->y + this->h;
}

void amogpu::log(const std::string &input_str) {
	std::cout << ("[AMOGPU] " + input_str).c_str() << std::endl;
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

bool amogpu::compile_shader(GLuint &shader, GLuint mode, const char* src) {
	shader = glCreateShader(mode);

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint compile_status = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if (!compile_status) {
		char log[256];
		glGetShaderInfoLog(shader, 256, NULL, log);
		amogpu::log(log);
	}

	return compile_status;
}

bool amogpu::create_program(gpu_gl_program &program, const char* vsh_path, const char* fsh_path) {
	std::string vsh_src;
	std::string fsh_src;

	GLuint vsh;
	GLuint fsh;
	
	bool flag = true;

	flag = amogpu::read_file(vsh_src, vsh_path) && amogpu::read_file(fsh_src, fsh_path);
	flag = flag && amogpu::create_program_from_src(program, vsh_src.c_str(), fsh_src.c_str());

	if (program.validation) {
		amogpu::log("'" + std::string(vsh_path) + "' & '" + std::string(fsh_path) + "' shaders compiled.");
	}

	return program.validation;
}

bool amogpu::create_program_from_src(gpu_gl_program &program, const char* vsh_src, const char* fsh_src) {
	GLuint vsh;
	GLuint fsh;

	bool flag = amogpu::compile_shader(vsh, GL_VERTEX_SHADER, vsh_src) && amogpu::compile_shader(fsh, GL_FRAGMENT_SHADER, fsh_src);

	if (flag) {
		program.program = glCreateProgram();

		glAttachShader(program.program, vsh);
		glAttachShader(program.program, fsh);
		glLinkProgram(program.program);

		GLint link_status = GL_FALSE;
		glGetProgramiv(program.program, GL_LINK_STATUS, &link_status);

		if (!link_status) {
			char log[256];
			glGetProgramInfoLog(program.program, 256, NULL, log);
		} else {
			amogpu::log("Program linked.");
		}
		
		program.validation = link_status;

		glDeleteShader(vsh);
		glDeleteShader(fsh);
	}

	return program.validation;
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