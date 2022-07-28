#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <amogpu/amogpu.hpp>

namespace draw {
	extern dynamic_batching batch;
	extern font_renderer font;
	extern bool refresh;

	void rectangle(float x, float y, float w, float h, const amogpu::vec4f &color);
}

#endif