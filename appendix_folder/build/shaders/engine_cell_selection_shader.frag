#version 330 core

flat in uvec4 frag_color;

out vec4 color;

void main() {
	color = frag_color / 255.f;
}