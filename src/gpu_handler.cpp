#include "amogpu/amogpu.hpp"

float amogpu::clock::dt = 0.0f;
uint32_t amogpu::clock::fps = 0;

amogpu::gpu_gl_program dynamic_batching::fx_shape;
dynamic_batching* dynamic_batching::invoked = nullptr;
uint32_t dynamic_batching::depth = 0;

float dynamic_batching::matrix_view_ortho[16];
float dynamic_batching::matrix_viewport[4];

void dynamic_batching::init() {
    const char* vsh_src = "#version 330 core\n"
                    "\n"
                    "layout (location = 0) in vec2 attrib_vertexes;\n"
                    "layout (location = 1) in vec2 attrib_tex_coords;\n"
                    "\n"
                    "uniform mat4 u_mat_projection;\n"
                    "uniform vec4 u_vec_rect;\n"
                    "uniform float u_float_zdepth;\n"
                    "\n"
                    "out vec2 varying_attrib_tex_coords;\n"
                    "\n"
                    "void main() {\n"
                    "\tif (u_vec_rect.z != 0 || u_vec_rect.w != 0) {\n"
                    "\t\tgl_Position = u_mat_projection * vec4((attrib_vertexes * u_vec_rect.zw) + u_vec_rect.xy, (u_float_zdepth / 100000), 1.0f);\n"
                    "\t} else {\n"
                    "\t\tgl_Position = u_mat_projection * vec4(attrib_vertexes + u_vec_rect.xy, (u_float_zdepth / 100000), 1.0f);\n"
                    "\t}\n"
                    "\n"
                    "\tvarying_attrib_tex_coords = attrib_tex_coords;\n"
                    "}";

	const char* fsh_src = "#version 330 core\n"
                    "\n"
                    "out vec4 out_frag_color;\n"
                    "uniform vec4 u_vec_color;\n"
                    "uniform sampler2D u_sampler_texture_slot;\n"
                    "uniform bool u_bool_texture_active;\n"
                    "\n"
                    "in vec2 varying_attrib_tex_coords;\n"
                    "\n"
                    "void main() {\n"
                    "\tvec4 frag_color = u_vec_color;\n"
                    "\n"
                    "\tif (u_bool_texture_active) {\n"
                    "\t\tfrag_color = texture(u_sampler_texture_slot, varying_attrib_tex_coords);\n"
                    "\t\tfrag_color = vec4(frag_color.xyz - ((1.0 - u_vec_color.xyz) - 1.0), frag_color.w * u_vec_color.w);\n"
                    "\t}\n"
                    "\n"
                    "\tout_frag_color = frag_color;\n"
                    "}";

	amogpu::create_program_from_src(dynamic_batching::fx_shape, vsh_src, fsh_src);
}

void dynamic_batching::matrix() {
	// Reset the depth pos.
	dynamic_batching::depth = 0;

	// Calculate the matrixes.
	amogpu::viewport(dynamic_batching::matrix_viewport);
	amogpu::projection_view_ortho(dynamic_batching::matrix_view_ortho, 0.0f, dynamic_batching::matrix_viewport[2], dynamic_batching::matrix_viewport[3], 0.0f);
}

void dynamic_batching::invoke() {
	this->sizeof_allocated_gpu_data = 0;
	this->sizeof_allocated_vertices = 0;

	dynamic_batching::invoked = this;
}

void dynamic_batching::instance(float x, float y, int32_t factor) {
	amogpu::gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];
	
	data.begin = this->sizeof_allocated_vertices;
	data.texture = 0;
	data.texture_slot = 0;

	data.rect[0] = x;
	data.rect[1] = y;

	if (factor != -1) {
		data.rect[2] = 0;
		data.rect[3] = 0;
	}

	this->factor(factor);
	this->sizeof_instanced_allocated_vertices = 0;
}

void dynamic_batching::modal(float w, float h) {
	amogpu::gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];

	data.rect[2] = w;
	data.rect[3] = h;
}

void dynamic_batching::factor(int32_t factor) {
	amogpu::gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];

	data.rect[2] = 0;
	data.rect[3] = 0;

	if (factor != -1 && data.factor != factor) {
		this->should_alloc_new_buffers = true;
	}
}

void dynamic_batching::fill(const amogpu::vec4f &color) {
	this->fill(color.x, color.y, color.z, color.w);
}

void dynamic_batching::fill(float r, float g, float b, float a) {
	amogpu::gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];

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
	amogpu::gpu_data &data = this->allocated_gpu_data[this->sizeof_allocated_gpu_data];

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

	amogpu::gpu_data data;
	bool flag;

	glBindVertexArray(this->vertex_arr_object);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (uint32_t i = 0; i < this->sizeof_allocated_gpu_data; i++) {
		data = this->allocated_gpu_data[i];
		flag = data.texture != 0;

		dynamic_batching::fx_shape.set4f("u_vec_rect", data.rect);
		dynamic_batching::fx_shape.set4f("u_vec_color", data.color);
		dynamic_batching::fx_shape.setb("u_bool_texture_active", flag);
		dynamic_batching::fx_shape.setf("u_float_zdepth", static_cast<float>(dynamic_batching::depth + i + 1));

		if (flag) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, data.texture);

			dynamic_batching::fx_shape.seti("u_sampler_texture_slot", data.texture_slot);
		}

		glDrawArrays(GL_TRIANGLES, data.begin, data.end);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Plus the concurrent allocated gpu data to global value of depth space.
	dynamic_batching::depth += this->sizeof_allocated_gpu_data;
	dynamic_batching::fx_shape.end();

	glBindVertexArray(0);
}

void dynamic_batching::free_buffers() {
	glDeleteBuffers(1, &this->vbo_vertices);
	glDeleteBuffers(1, &this->vbo_texture_coords);
}