#include "app.h"
#include <imgui_impl_glfw.h>

const char* DEFAULT_VERT_SHADER = "./Shaders/engine_shader.vert";
const char* DEFAULT_FRAG_SHADER = "./Shaders/engine_shader.frag";

App* app;

void resize_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_moved_callback(GLFWwindow* window, double x_pos, double y_pos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {
	app = new App(500, 500, DEFAULT_VERT_SHADER, DEFAULT_FRAG_SHADER);

	glfwSetWindowSizeCallback(app->window(), resize_callback);
	glfwSetKeyCallback(app->window(), key_callback);
	glfwSetScrollCallback(app->window(), scroll_callback);
	glfwSetCursorPosCallback(app->window(), mouse_moved_callback);
	glfwSetMouseButtonCallback(app->window(), mouse_button_callback);

	while (!app->should_close())
		app->update();

	delete app;
}

void resize_callback(GLFWwindow* window, int width, int height) {
	app->resize_callback(width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		app->close();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	app->scroll_callback(xoffset, yoffset);
}

void mouse_moved_callback(GLFWwindow* window, double x_pos, double y_pos) {
	app->mouse_moved_callback(x_pos, y_pos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	app->mouse_button_callback(button, action == GLFW_PRESS);
}

//SINGLETON REWORK