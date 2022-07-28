#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "render.hpp"
#include <vector>
#include <SDL2/SDL.h>

class keyboard {
protected:
	bool should_accept_input;

	std::vector<const char*> key_char_list;
	std::vector<amogpu::rect> key_list;

	char* char_highlight;
	float offset;
public:
	amogpu::rect rect;
	
	void set_size(float width, float height);
	void set_pos(float x, float y);

	void init();
	void calculate_scale();
	void on_event(SDL_Event &sdl_event);
	void on_draw_reload();
};

#endif