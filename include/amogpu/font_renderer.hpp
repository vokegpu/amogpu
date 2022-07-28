#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include "util.hpp"
#include "gl/glew.h"

struct font_char {
	float x = 0;
	float texture_x = 0;

	float w = 0;
	float h = 0;

	float top = 0;
	float left = 0;
};

class font_renderer {
protected:
	static FT_Library ft_library;
	
	FT_Face ft_face;
	FT_GlyphSlot ft_glyph_slot;
	FT_Bool use_kerneking;
	FT_UInt previous;
	FT_Vector_ previous_char_vec;

	GLuint texture_bitmap;

	uint32_t texture_width;
	uint32_t texture_height;

	std::string current_font_path;
	uint8_t current_font_size;

	font_char allocated_font_char[256];
public:
	static void init();
	static void end_ft_library();

	void load(const std::string &font_path, uint8_t font_size);

	float get_text_width(const std::string &text);
	float get_text_height();

	void render(const std::string &text, float x, float y, const util::vec4f &color);
};

namespace font {
	extern font_renderer renderer;
};

#endif