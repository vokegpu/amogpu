#include "amogpu/amogpu.hpp"

FT_Library font_renderer::ft_library;

dynamic_batching* font_renderer::batch() {
	return this->batch_mode == amogpu::invoked ? dynamic_batching::invoked : this->binded_batch;
}

void font_renderer::init() {
	FT_Init_FreeType(&font_renderer::ft_library);
}

void font_renderer::end_ft_library() {
	FT_Done_FreeType(font_renderer::ft_library);
}

void font_renderer::load(const std::string &font_path, uint8_t font_size) {
	if (this->batch_mode == 0) {
		this->from(amogpu::invoked);
	}

	if (this->current_font_path == font_path && this->current_font_size == font_size) {
		return;
	}

	if (this->current_font_path != font_path && FT_New_Face(font_renderer::ft_library, font_path.c_str(), 0, &this->ft_face)) {
		amogpu::log("Could not load font or invalid path.");
		return;
	}

	this->current_font_path = font_path;
	this->current_font_size = font_size;

	this->texture_width = 0;
	this->texture_height = 0;

	FT_Set_Pixel_Sizes(this->ft_face, 0, font_size);
	this->use_kerneking = FT_HAS_KERNING(this->ft_face);
	this->ft_glyph_slot = this->ft_face->glyph;

	for (uint8_t i = 0; i < 128; i++) {
		if (FT_Load_Char(this->ft_face, i, FT_LOAD_RENDER)) {
			continue;
		}

		this->texture_width += this->ft_glyph_slot->bitmap.width;
		this->texture_height = std::max(this->texture_height, this->ft_glyph_slot->bitmap.rows);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (this->texture_bitmap == 0) {
		glGenTextures(1, &this->texture_bitmap);
	} else {
		glDeleteTextures(1, &this->texture_bitmap);
		glGenTextures(1, &this->texture_bitmap);
	}

	glBindTexture(GL_TEXTURE_2D, this->texture_bitmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (int32_t) this->texture_width, (int32_t) this->texture_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);

	float offset = 0.0f;

	for (uint8_t i = 0; i < 128; i++) {
		if (FT_Load_Char(this->ft_face, i, FT_LOAD_RENDER)) {
			continue;
		}

		amogpu::font_char &f_char = this->allocated_font_char[i];

		f_char.x = offset / static_cast<float>(this->texture_width);
		f_char.w = static_cast<float>(this->ft_glyph_slot->bitmap.width);
		f_char.h = static_cast<float>(this->ft_glyph_slot->bitmap.rows);

		f_char.left = static_cast<float>(this->ft_glyph_slot->bitmap_left);
		f_char.top = static_cast<float>(this->ft_glyph_slot->bitmap_top);
		f_char.texture_x = static_cast<float>(this->ft_glyph_slot->advance.x >> 6);

		glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(offset), 0, f_char.w, f_char.h, GL_ALPHA, GL_UNSIGNED_BYTE, this->ft_glyph_slot->bitmap.buffer);
		offset += f_char.w;
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	amogpu::log("Font loaded, created bitmap!");
}

void font_renderer::from(dynamic_batching* concurrent_batch) {
	this->binded_batch = concurrent_batch;
	this->batch_mode = amogpu::dynamic;
}

void font_renderer::from(uint8_t mode) {
	this->batch_mode = mode;
}

float font_renderer::get_text_width(const std::string &text) {
	FT_Vector vec;
	this->previous = 0;

	float start_x = 0.5f;
	float render_x = 0.0f;
	float text_width = 0.0f;

	amogpu::font_char f_char;

	for (const char* i = text.c_str(); *i; i++) {
		if (this->use_kerneking && this->previous && *i) {
			FT_Get_Kerning(this->ft_face, this->previous, *i, 0, &vec);
			start_x += static_cast<float>(vec.x >> 6);
		}

		f_char = this->allocated_font_char[*i];

		render_x = start_x + f_char.left;
		start_x += f_char.texture_x;

		this->previous = *i;
		text_width = render_x + f_char.w;
	}

	return static_cast<float>(text_width);
}

float font_renderer::get_text_height() {
	float h = static_cast<float>(this->texture_height);
	return h + (h / 8);
}

void font_renderer::render(const std::string &text, float x, float y, const amogpu::vec4f &color) {
	const char* char_str = text.c_str();
	const int32_t str_len = strlen(char_str);

  	float render_x = 0, render_y = 0, render_w = 0, render_h = 0;
    float texture_x = 0, texture_y = 0, texture_w = 0, texture_h = 0;
    float impl = (static_cast<float>(this->texture_height) / 8);
    int32_t diff = 1;

    x = static_cast<float>(static_cast<int32_t>(x));
    y = static_cast<float>(static_cast<int32_t>(y));

    // Call GPU instance.
    dynamic_batching* batch = this->batch();

    batch->instance(x, y - (impl / 2));
    batch->fill(color);
    batch->bind(this->texture_bitmap);

    // Reset to mesh geometry.
    x = 0;
    y = 0;

    this->previous = 0;
    amogpu::font_char f_char;

    for (const char* i = char_str; *i; i++) {
    	if (this->use_kerneking && this->previous && *i) {
    		FT_Get_Kerning(this->ft_face, this->previous, *i, 0, &this->previous_char_vec);
    		x += static_cast<float>(this->previous_char_vec.x >> 6);
    	}

    	f_char = this->allocated_font_char[*i];

        render_x = x + f_char.left;
        render_y = y + (static_cast<float>(this->texture_height) - f_char.top);

        render_w = f_char.w;
        render_h = f_char.h;

        texture_x = f_char.x;
        texture_w = render_w / static_cast<float>(this->texture_width);
        texture_h = render_h / static_cast<float>(this->texture_height);
        diff += static_cast<int32_t>(texture_x);

        // Draw a quad.
        batch->vertex(render_x, render_y);
        batch->vertex(render_x, render_y + render_h);
        batch->vertex(render_x + render_w, render_y + render_h);
        batch->vertex(render_x + render_w, render_y + render_h);
        batch->vertex(render_x + render_w, render_y);
        batch->vertex(render_x, render_y);
        
        // Also set the modal rect texture.
        batch->coords(texture_x, texture_y);
        batch->coords(texture_x, texture_y + texture_h);
        batch->coords(texture_x + texture_w, texture_y + texture_h);
        batch->coords(texture_x + texture_w, texture_y + texture_h);
        batch->coords(texture_x + texture_w, texture_y);
        batch->coords(texture_x, texture_y);

        x += f_char.texture_x;
        this->previous = *i;
    }

    batch->factor(str_len + diff);
    batch->next();
}