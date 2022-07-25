#include "gpu_handler.hpp"

bool opengl::compile_shader(GLuint &shader, GLuint mode, const char* src) {
	shader = glCreateShader(mode);

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint compile_status = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if (!compile_status) {
		char log[256];
		glGetShaderInfoLog(shader, 256, NULL, log);
		util::log(log);
	}

	return compile_status;
}

bool opengl::create_program(gpu_gl_program &program, const char* vsh_path, const char* fsh_path) {
	std::string vsh_src;
	std::string fsh_src;

	GLuint vsh;
	GLuint fsh;
	
	bool flag = true;

	flag = util::read_file(vsh_src, vsh_path) && util::read_file(fsh_src, fsh_path);
	flag = flag && opengl::compile_shader(vsh, GL_VERTEX_SHADER, vsh_src.c_str()) && opengl::compile_shader(fsh, GL_FRAGMENT_SHADER, fsh_src.c_str());

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
		}
		
		program.validation = link_status;

		glDeleteShader(vsh);
		glDeleteShader(fsh);
	}

	return program.validation;
}

void dynamic_batching::init() {

}

void dynamic_batching::call() {

}

void dynamic_batching::fill(util::vec4f &color) {

}

void dynamic_batching::vertex(float x, float y) {

}

void dynamic_batching::bind(GLuint texture) {

}

void dynamic_batching::coords(float u, float v) {

}

void dynamic_batching::end() {

}

void dynamic_batching::release() {

}

void dynamic_batching::draw() {

}

void dynamic_batching::free_buffers() {

}