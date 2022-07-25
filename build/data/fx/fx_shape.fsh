#version 330 core

uniform vec4 u_vec_color;
uniform sampler2D u_sampler_texture_slot;
uniform bool u_bool_texture_active;

in vec2 varying_attrib_tex_coords;

void main() {
	vec4 frag_color = u_vec_color;

	if (u_bool_texture_active) {
		frag_color = texture(u_sampler_texture_slot, varying_attrib_tex_coords);
		frag_color = frag_color * u_vec_color;
	}

	gl_FragColor = frag_color;
}