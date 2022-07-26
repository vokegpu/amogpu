#include "gpu_handler.hpp"

dynamic_batching draw::batch;
bool draw::refresh;

gpu_gl_program dynamic_batching::fx_shape;
float dynamic_batching::matrix_view_ortho[16];
float dynamic_batching::matrix_viewport[4];

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
		} else {
			util::log("'" + std::string(vsh_path) + "' & '" + std::string(fsh_path) + "' shaders compiled.");
		}
		
		program.validation = link_status;

		glDeleteShader(vsh);
		glDeleteShader(fsh);
	}

	return program.validation;
}

void gpu_gl_program::use() {
	glUseProgram(this->program);
}

void gpu_gl_program::end() {
	glUseProgram(0);
}

void gpu_gl_program::setb(const std::string &uniform_name, bool val) {
	glUniform1i(glGetUniformLocation(this->program, uniform_name.c_str()), (int32_t) val);
}

void gpu_gl_program::seti(const std::string &uniform_name, int32_t val) {
	glUniform1i(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void gpu_gl_program::setf(const std::string &uniform_name, float val) {
	glUniform1f(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void gpu_gl_program::set2f(const std::string &uniform_name, const float* val) {
	glUniform2fv(glGetUniformLocation(this->program, uniform_name.c_str()), GL_TRUE, val);
}

void gpu_gl_program::set4f(const std::string &uniform_name, const float* val) {
	glUniform4fv(glGetUniformLocation(this->program, uniform_name.c_str()), GL_TRUE, val);
}

void gpu_gl_program::setm4f(const std::string &uniform_name, const float* val) {
	glUniformMatrix4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, GL_FALSE, val);
}

void dynamic_batching::init() {
	opengl::create_program(dynamic_batching::fx_shape, "data/fx/fx_shape.vsh", "data/fx/fx_shape.fsh");
}

void dynamic_batching::matrix() {
	util::viewport(dynamic_batching::matrix_viewport);
	util::projection_view_ortho(dynamic_batching::matrix_view_ortho, 0.0f, dynamic_batching::matrix_viewport[2], dynamic_batching::matrix_viewport[3], 0.0f);
}

void dynamic_batching::invoke() {
	this->sizeof_allocated_gpu_data = 0;
	this->sizeof_allocated_vertices = 0;
}

void dynamic_batching::instance(float x, float y, int32_t factor) {
	gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];
	
	data.begin = this->sizeof_allocated_vertices;
	data.texture = 0;
	data.texture_slot = 0;

	data.pos[0] = x;
	data.pos[1] = y;

	this->factor(factor);
	this->sizeof_instanced_allocated_vertices = 0;
}

void dynamic_batching::factor(int32_t factor) {
	if (factor != -1 && this->allocated_gpu_data[this->sizeof_allocated_gpu_data].factor != factor) {
		this->should_alloc_new_buffers = true;
	}
}

void dynamic_batching::fill(const util::vec4f &color) {
	this->fill(color.x, color.y, color.z, color.w);
}

void dynamic_batching::fill(float r, float g, float b, float a) {
	gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];

	// RGBA - normalised value.
	data.color[0] = r;
	data.color[1] = g;
	data.color[2] = b;
	data.color[3] = a;
}

void dynamic_batching::vertex(float x, float y) {
	this->concurrent_allocated_vertices.push_back(x);
	this->concurrent_allocated_vertices.push_back(y);
	this->sizeof_instanced_allocated_vertices++;
}

void dynamic_batching::bind(GLuint texture) {
	gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];

	// Why alloc more textures? there is no reason for that.
	for (uint8_t i = 0; i < this->concurrent_allocated_textures.size(); i++) {
		GLuint allocated_texture = this->concurrent_allocated_textures.at(i);

		if (allocated_texture == texture) {
			data.texture = texture;
			data.texture_slot = i;

			break;
		}
	}

	if (data.texture == 0 && data.texture_slot == 0) {
		// Set the slot and push new texture to memory.
		data.texture = texture;
		data.texture_slot = (uint8_t) this->concurrent_allocated_textures.size();

		this->concurrent_allocated_textures.push_back(texture);
	}
}

void dynamic_batching::coords(float u, float v) {
	this->concurrent_allocated_texture_coords.push_back(u);
	this->concurrent_allocated_texture_coords.push_back(v);
}

void dynamic_batching::next() {
	this->allocated_gpu_data[this->sizeof_allocated_gpu_data].end = this->sizeof_instanced_allocated_vertices;
	this->sizeof_allocated_vertices += this->sizeof_instanced_allocated_vertices;
	this->sizeof_allocated_gpu_data++;
}

void dynamic_batching::revoke() {
	if (!this->should_not_create_buffers) {
		glGenVertexArrays(1, &this->vertex_arr_object);
		glGenBuffers(1, &this->vbo_vertices);
		glGenBuffers(1, &this->vbo_texture_coords);
		this->should_not_create_buffers = true;
	}

	if ((this->sizeof_previous_allocated_gpu_data != this->sizeof_allocated_gpu_data) || this->should_alloc_new_buffers) {
		this->sizeof_previous_allocated_gpu_data = this->sizeof_allocated_gpu_data;
		this->should_alloc_new_buffers = false;

		glBindVertexArray(this->vertex_arr_object);

		glEnableVertexAttribArray(0); // Location 0; the vertexes data.
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->concurrent_allocated_vertices.size(), &this->concurrent_allocated_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		glEnableVertexAttribArray(1); // Location 1; the coordinates texture data.
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texture_coords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->concurrent_allocated_texture_coords.size(), &this->concurrent_allocated_texture_coords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		glBindVertexArray(0);
	}

	this->concurrent_allocated_vertices.clear();
	this->concurrent_allocated_textures.clear();
	this->concurrent_allocated_texture_coords.clear();
}

void dynamic_batching::draw() {
	dynamic_batching::fx_shape.use();
	dynamic_batching::fx_shape.setm4f("u_mat_projection", dynamic_batching::matrix_view_ortho);

	gpu_data data;
	bool flag;

	glBindVertexArray(this->vertex_arr_object);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (uint32_t i = 0; i < this->sizeof_allocated_gpu_data; i++) {
		data = this->allocated_gpu_data[i];
		flag = data.texture != 0;

		dynamic_batching::fx_shape.set2f("u_vec_pos", data.pos);
		dynamic_batching::fx_shape.set4f("u_vec_color", data.color);
		dynamic_batching::fx_shape.setb("u_bool_texture_active", flag);
		dynamic_batching::fx_shape.setf("u_float_zdepth", static_cast<float>(i + 1));

		if (flag) {
			glActiveTexture(GL_TEXTURE0 + data.texture_slot);
			glBindTexture(GL_TEXTURE_2D, data.texture);
			
			dynamic_batching::fx_shape.seti("u_sampler_texture_slot", data.texture_slot);
		}

		glDrawArrays(GL_TRIANGLES, data.begin, data.end);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	dynamic_batching::fx_shape.end();
	glBindVertexArray(0);
}

void dynamic_batching::free_buffers() {
	glDeleteBuffers(1, &this->vbo_vertices);
	glDeleteBuffers(1, &this->vbo_texture_coords);
}

void draw::rectangle(float x, float y, float w, float h, const util::vec4f &color) {
	// Clamp the sizes.
	w = w < 0 ? 1 : w;
	h = h < 0 ? 1 : h;

	// NOTE: the factor value in instance is to mark concurrent changes in geometry,
	// flagging it the gpu handler can alloc the new size buffer. 

	// Call an instance of the global batch.
	draw::batch.instance(x, y, w / h);

	// We need to reset the coords for the geometry mesh.
	x = 0;
	y = 0;

	// Fill with the color.
	draw::batch.fill(color);

	// First triangle part of rect.
	draw::batch.vertex(x, y);
	draw::batch.vertex(x, y + h);
	draw::batch.vertex(x + w, y + h);

	// Second triangle part of rect.
	draw::batch.vertex(x + w, y + h);
	draw::batch.vertex(x + w, y);
	draw::batch.vertex(x, y);

	// We do not need to set texture coords so set everything to 0.
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);

	// End the instance.
	draw::batch.next();
}