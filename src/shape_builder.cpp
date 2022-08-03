#include "amogpu/shape_builder.hpp"

amogpu::gpu_gl_program shape_builder::fx_shape;

GLuint shape_builder::vertex_array = 0;
GLuint shape_builder::vertex_buffer = 0;

void shape_builder::init() {
	const char* vsh_src = "#version 330 core\n"
	"layout (location = 0) in vec2 attrib_vertexes;\n"
	"out vec2 varying_attrib_vertexes;\n"
	"uniform vec4 u_vec_rect;\n"
	"uniform mat4 u_mat_projection;\n"
	"void main() {\n"
	"gl_Position = u_mat_projection * vec4((attrib_vertexes * u_vec_rect.zw) + u_vec_rect.xy, 0.0, 1.0);\n"
	"varying_attrib_vertexes = attrib_vertexes;\n"
	"}\n";

	const char* fsh_src = "#version 330 core\n"
	"in vec2 varying_attrib_vertexes;\n"
	"out vec4 frag_color;"
	"uniform vec4 u_vec_color;\n"
	"uniform bool u_bool_texture;\n"
	"uniform sampler2D u_sampler_texture_slot;\n"
	"uniform vec4 u_vec_uv_rect;\n"
	"void main() {\n"
	"vec4 color = u_vec_color;\n"
	"if (u_bool_texture) {\n"
	"color = texture(u_sampler_texture_slot, (varying_attrib_vertexes * u_vec_uv_rect.zw) + u_vec_uv_rect.xy);\n"
	"color = u_vec_color * color;\n"
	"}\n"
	"frag_color = color;\n"
	"}\n";

	amogpu::create_program_from_src(shape_builder::fx_shape, vsh_src, fsh_src);

	const float mesh[12] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

	// Gen this buffer.
	glGenBuffers(1, &shape_builder::vertex_buffer);
	glGenVertexArrays(1, &shape_builder::vertex_array);
	glBindVertexArray(shape_builder::vertex_array);

	// 1 buffer is ok.
	glBindBuffer(GL_ARRAY_BUFFER, shape_builder::vertex_buffer);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, mesh, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Log.
	amogpu::log("Shape builder fx compiled.");
}

void shape_builder::free_buffers() {
	glDeleteBuffers(1, &shape_builder::vertex_buffer);
}

void shape_builder::build(const uint16_t &format, const amogpu::vec4f &color, GLuint texture) {
	this->enum_flag_format = format;

	// Set the index position.
	this->concurrent_gpu_data.begin = 0;
	this->concurrent_gpu_data.end = 6;

	// Pass RGB normalised values to gpu data.
	this->concurrent_gpu_data.color[0] = color.x;
	this->concurrent_gpu_data.color[1] = color.y;
	this->concurrent_gpu_data.color[2] = color.z;
	this->concurrent_gpu_data.color[3] = color.w;

	// Set texture settings.
	this->concurrent_gpu_data.texture = texture;
	this->concurrent_gpu_data.texture_slot = 0;

	// Set the details on build.
	shape_builder::fx_shape.setb("u_bool_texture", this->concurrent_gpu_data.texture != 0);
	shape_builder::fx_shape.set4f("u_vec_color", this->concurrent_gpu_data.color);

	if (this->concurrent_gpu_data.texture != 0) {
		glActiveTexture(GL_TEXTURE0 + this->concurrent_gpu_data.texture_slot);
		glBindTexture(GL_TEXTURE_2D, this->concurrent_gpu_data.texture);
		shape_builder::fx_shape.seti("u_sampler_texture_slot", this->concurrent_gpu_data.texture_slot);
	}
}

void shape_builder::modal(float x, float y, float w, float h) {
	if (this->concurrent_gpu_data.texture == 0) {
		return;
	}

	this->concurrent_gpu_data.rect[0] = x;
	this->concurrent_gpu_data.rect[1] = y;
	this->concurrent_gpu_data.rect[2] = w;
	this->concurrent_gpu_data.rect[3] = h;

	shape_builder::fx_shape.set4f("u_vec_uv_rect", this->concurrent_gpu_data.rect);
}

void shape_builder::draw(float x, float y, float w, float h) {
	this->concurrent_gpu_data.rect[0] = x;
	this->concurrent_gpu_data.rect[1] = y;
	this->concurrent_gpu_data.rect[2] = w;
	this->concurrent_gpu_data.rect[3] = h;

	shape_builder::fx_shape.set4f("u_vec_rect", this->concurrent_gpu_data.rect);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind buffer and draw it.
	glDrawArrays(GL_TRIANGLES, this->concurrent_gpu_data.begin, this->concurrent_gpu_data.end);

	// Bind off buffer and texture.
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void shape_builder::invoke() {
	shape_builder::fx_shape.use();
	shape_builder::fx_shape.setm4f("u_mat_projection", amogpu::matrix_projection_ortho);

	// Enable the VAO.
	glBindVertexArray(shape_builder::vertex_array);
}

void shape_builder::revoke() {
	shape_builder::fx_shape.end();
}