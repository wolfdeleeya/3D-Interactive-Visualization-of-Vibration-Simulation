#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

out vec3 frag_color, frag_normal;

uniform mat4 model = mat4(1), view = mat4(1), projection = mat4(1);

void main() {
	gl_Position = projection * view * model * vec4(position, 1);
	frag_color = color;
	frag_normal = normal;
}