#include "render.hpp"

dynamic_batching draw::batch;
font_renderer draw::font;
bool draw::refresh = true;

void draw::rectangle(float x, float y, float w, float h, const amogpu::vec4f &color) {
	// Call an instance of the global batch.
	draw::batch.instance(x, y);
	draw::batch.modal(w, h);

	// We need to reset the coords for the geometry mesh.
	x = 0.0f;
	y = 0.0f;
	w = 1.0f;
	h = 1.0f;

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