#pragma once
#define RENDER_H
#define RENDER_H

#include <amogpu/amogpu.hpp>

struct draw {
	extern dynamic_batching const batch;
	extern font_renderer const font;
	extern bool refresh;

	void draw::rectangle(float x, float y, float w, float h, const amogpu::vec4f &color);
}

#endif