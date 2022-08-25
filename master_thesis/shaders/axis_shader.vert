#version 330 core

layout (location = 0) in uint point_id;
layout (location = 1) in uint axis_id;

flat out vec3 color;

uniform vec3 point_arr[4] = { vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)};

uniform mat4 view, projection;

void main() {
	gl_Position = projection * view * vec4(point_arr[point_id], 1);

	//set color coordinate to 1 based on axis_id
	color = vec3(0);
	color[axis_id] = 1;
}