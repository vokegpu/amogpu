#pragma once

#include <ft2build.h>
#define FT_LIBRARY_H

#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include "util.hpp"

struct font_char {
	float x = 0;
	float texture_x = 0;

	float w = 0;
	float h = 0;

	float top = 0;
	float left = 0;
}

class font_renderer {
protected:
	FT_Library ft_library;
	FT_Face ft_face;
	FT_GlyphSlot f_glyph_slot;
	FT_Bool use_kerneking;
	FT_UInt previous;
	FT_Vector_ previous_char_vec;

	GLuint texture_bitmap;

	uint32_t texture_width;
	uint32_t texture_height;
public:
	void load(const std::string font_path);

	float get_text_width(const std::string &text);
	float get_text_height();

	void render(const std::string &text, float x, float y, const util::vec4f &color);
}

#endif