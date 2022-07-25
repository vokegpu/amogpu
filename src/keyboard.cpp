#include "keyboard.hpp"
#include "font_renderer.hpp"
#include "gpu_handler.hpp"

void keyboard::set_size(float width, float height) {
	if (this->rect.w != width || this->rect.h != height) {
		this->rect.w = width;
		this->rect.h = height;
	}
}

void keyboard::set_pos(float x, float y) {
	this->rect.x = x;
	this->rect.y = y;
}

void keyboard::init() {
	this->key_char_list = {"Q", "W", "R", "R", "T", "Y", "U", "I", "O", "P", "A", "S", "D", "F", "G", "H", "J", "K", "L", "C", "Z", "X", "C", "V", "B", "N", "M", " ", " ", " "};
	this->calculate_scale();
}

void keyboard::calculate_scale() {
	float x = this->rect.x;
	float y = this->rect.y;

	float text_height = font::renderer.get_text_height();
	float square_width = text_height + (text_height / 6);

	this->key_list.clear();
	this->offset = 1.0f;

	uint8_t step = 0;
	uint8_t steps_going_on = 0;
	float subset = 0;

	for (uint8_t i = 0; i < this->key_char_list.size(); i++) {
		++steps_going_on;

		if ((steps_going_on > 10 && step == 0 || steps_going_on > 9 && step == 1)) {
			step++;
			steps_going_on = 0;
			subset = (square_width / (10 - step * 2)) * step;

			y += text_height + this->offset;
			x = this->rect.x + subset;
		} else if (steps_going_on > 7 && step == 3) {
			break;
		}

		util::rect rekt;

		rekt.x = x;
		rekt.y = y;
		rekt.w = square_width;
		rekt.h = text_height;

		x += square_width + this->offset;

		this->key_list.push_back(rekt);
	}
}

void keyboard::on_draw_reload() {
	util::vec4f color(1.0f, 1.0f, 1.0f, 0.9f);
	util::vec4f color_str(0.0f, 0.0f, 0.0f, 1.0f);

	for (uint8_t i = 0; i < this->key_list.size(); i++) {
		util::rect rekt = this->key_list.at(i);
		const char* key = (const char*) this->key_char_list.at(i);

		draw::rectangle(rekt.x, rekt.y, rekt.w, rekt.h, color);
		font::renderer.render(std::string(key), rekt.x + (rekt.w / 9), rekt.y + (rekt.h / 9), color_str);
	}
}