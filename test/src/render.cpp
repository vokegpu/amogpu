#include "render.hpp"

dynamic_batching const draw::batch;
font_renderer const draw::font;
bool draw::refresh = true;

void draw::rectangle(float x, float y, float w, float h, const amogpu::vec4f &color) {
	// Clamp the sizes.
	w = w < 0 ? 1 : w;
	h = h < 0 ? 1 : h;

	// NOTE: the factor value in instance is to mark concurrent changes in geometry,
	// flagging it the gpu handler can alloc the new size buffer. 

	// Call an instance of the global batch.
	draw::batch.instance(x, y, w / h);

	// We need to reset the coords for the geometry mesh.
	x = 0;
	y = 0;

	// Fill with the color.
	draw::batch.fill(color);

	// First triangle part of rect.
	draw::batch.vertex(x, y);
	draw::batch.vertex(x, y + h);
	draw::batch.vertex(x + w, y + h);

	// Second triangle part of rect.
	draw::batch.vertex(x + w, y + h);
	draw::batch.vertex(x + w, y);
	draw::batch.vertex(x, y);

	// We do not need to set texture coords so set everything to 0.
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);
	draw::batch.coords(0.0f, 0.0f);

	// End the instance.
	draw::batch.next();
}