#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model = mat4(1), view = mat4(1), projection = mat4(1);

void main() {
	gl_Position = projection * view * model * vec4(position, 1);
}