#version 330 core

in vec3 frag_color, frag_normal;

out vec4 color;

void main() {
	color = vec4(frag_color, 1);
}