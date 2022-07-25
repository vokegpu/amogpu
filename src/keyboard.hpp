#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "util.hpp"
#include <vector>

class keyboard {
protected:
	bool should_accept_input;
	util::rect rect;

	std::vector<const char*> key_char_list;
	std::vector<util::rect> key_list;
	float offset;
public:
	void set_size(float width, float height);
	void set_pos(float x, float y);

	void init();
	void calculate_scale();
	void on_draw_reload();
};

#endif