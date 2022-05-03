#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in uint cell_index;
layout (location = 3) in vec3 color;

out vec3 frag_color, frag_normal;

uniform mat4 model = mat4(1), view = mat4(1), projection = mat4(1);
uniform uint selected_index;

void main() {
	gl_Position = projection * view * model * vec4(position, 1);
	
	int is_selected = int(cell_index == selected_index);

	frag_color = is_selected * (vec3(1) - color) + (1 - is_selected) * color;
	frag_normal = normal;
}