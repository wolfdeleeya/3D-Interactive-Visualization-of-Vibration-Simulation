#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in uint index;

flat out uvec4 frag_color;

uniform mat4 model = mat4(1), view = mat4(1), projection = mat4(1);

void main() {
	gl_Position = projection * view * model * vec4(position, 1);

	uint i = index;

	uint red = i / uint(256 * 256);
	i = uint(mod(i, 256 * 256));
	
	uint green = i / uint(256);
	i = uint(mod(i, 256));
	
	uint blue = i;

	frag_color = uvec4(red, green, blue, 255);
}