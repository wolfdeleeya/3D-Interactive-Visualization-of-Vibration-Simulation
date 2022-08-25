#version 330 core

flat in vec3 color;

out vec3 frag_color;

void main() {
	frag_color = color;
}