#include "keyboard.hpp"

void keyboard::set_size(float width, float height) {
	if (this->rect.w != width || this->rect.h != height) {
		this->rect.w = width;
		this->rect.h = height;
	}
}

void keyboard::set_pos(float x, float y) {
	if (this->rect.x != x || this->rect.y != y) {
		this->rect.x = x;
		this->rect.y = y;
		draw::refresh = true;
	}
}

void keyboard::init() {
	this->key_char_list = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Cc", "Z", "X", "C", "V", "B", "N", "M", " ", "  ", "   "};
	this->calculate_scale();
}

void keyboard::on_event(SDL_Event &sdl_event) {
	switch (sdl_event.type) {
		case SDL_MOUSEMOTION: {
			float mx = static_cast<float>(sdl_event.motion.x);
			float my = static_cast<float>(sdl_event.motion.y);

			char* concurrent_highlight = nullptr;

			if (this->rect.aabb_collide_with_point(mx, my)) {
				for (uint8_t i = 0; i < this->key_list.size(); i++) {
					util::rect &rekts = this->key_list.at(i);
					char* key = const_cast<char*>(this->key_char_list.at(i));

					// Add the rects.
					rekts += this->rect;

					if (rekts.aabb_collide_with_point(mx, my)) {
						concurrent_highlight = key;
					}

					// Subtract to old pos.
					rekts -= this->rect;
				}
			}

			if (this->char_highlight != concurrent_highlight) {
				this->char_highlight = concurrent_highlight;
				draw::refresh = true;
			}

			break;
		}
	}
}

void keyboard::calculate_scale() {
	float x = 0;
	float y = 0;

	float text_height = draw::font.get_text_height();
	float square_width = text_height + (text_height / 6);

	this->key_list.clear();
	this->offset = 1.0f;

	uint8_t step = 0;
	uint8_t steps_going_on = 0;
	float subset = 0;

	// NOTE: I created a formula to add opossite offset propety of keyboards.
	// (key_width / (10 - step * 2)) * step

	float w = square_width + this->offset;
	float h = text_height + this->offset;

	this->rect.w = 0;
	this->rect.h = 0;

	for (uint8_t i = 0; i < this->key_char_list.size(); i++) {
		++steps_going_on;

		if ((steps_going_on > 10 && step == 0 || steps_going_on > 9 && step == 1)) {
			step++;
			steps_going_on = 0;
			subset = (square_width / (10 - step * 2)) * step;

			y += h;
			x = subset;
		} else if (steps_going_on > 7 && step == 3) {
			break;
		}

		amogpu::rect rekt;

		rekt.x = x;
		rekt.y = y;
		rekt.w = square_width;
		rekt.h = text_height;

		x += w;
		this->key_list.push_back(rekt);

		if (x + w > this->rect.w) {
			this->rect.w = x + w;
		}

		if (y + h > this->rect.h) {
			this->rect.h = y + h;
		}
	}

	draw::refresh = true;
}

void keyboard::on_draw_reload() {
	amogpu::vec4f color(1.0f, 1.0f, 1.0f, 0.9f);
	amogpu::vec4f color_highlight(200.0f / 255, 86.0f / 255, 121.0f / 255, 0.5f);
	amogpu::vec4f color_str(0.0f, 0.0f, 0.0f, 1.0f);

	for (uint8_t i = 0; i < this->key_list.size(); i++) {
		amogpu::rect rekt = this->key_list.at(i);
		const char* key = this->key_char_list.at(i);

		draw::rectangle(this->rect.x + rekt.x, this->rect.y + rekt.y, rekt.w, rekt.h, key == this->char_highlight ? color_highlight : color);
		draw::font.render(std::string(key), this->rect.x + rekt.x + (rekt.w / 9), this->rect.y + rekt.y + (rekt.h / 9), color_str);
	}
}