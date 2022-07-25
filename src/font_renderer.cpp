#include "font_renderer.hpp"

void font_rendererer::load(const std::string &font_path) {

}

float font_rendererer::get_text_width(const std::string &text) {
	this->previous = 0;
	FT_Vector vec;

	float start_x = 0.5f;
	float render_x = 0.0f;
	float text_width = 0.0f;

	for (const char* i = text.c_str(); i *i; i++) {
		if (this->use_kerneking && this->previous && *i) {
			//FT_Get_Kerneking(this->face, );
		}
	}
}

float font_rendererer::get_text_height() {
	float h = static_cast<float>(this->texture_height);
	return h + (h / 8);
}

void font_rendererer::render(const std::string &text, float x, float y, const util::vec4f &color) {

}