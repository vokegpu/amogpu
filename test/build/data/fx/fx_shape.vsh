#version 330 core

layout (location = 0) in vec2 attrib_vertexes;
layout (location = 1) in vec2 attrib_tex_coords;

uniform mat4 u_mat_projection;
uniform vec4 u_vec_rect;
uniform float u_float_zdepth;

out vec2 varying_attrib_tex_coords;

void main() {
	if (u_vec_rect.z != 0 || u_vec_rect.w != 0) {
		gl_Position = u_mat_projection * vec4((attrib_vertexes * u_vec_rect.zw) + u_vec_rect.xy, (u_float_zdepth / 100000), 1.0f);
	} else {
		gl_Position = u_mat_projection * vec4(attrib_vertexes + u_vec_rect.xy, (u_float_zdepth / 100000), 1.0f);
	}

	varying_attrib_tex_coords = attrib_tex_coords;
}