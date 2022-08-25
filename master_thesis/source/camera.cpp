#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"

const glm::vec4 Camera::UP = glm::vec4(0, 1, 0, 1);
const glm::vec4 Camera::FORWARD = glm::vec4(0, 0, -1, 1);
const glm::vec4 Camera::RIGHT = glm::vec4(1, 0, 0, 1);

glm::mat4 Camera::look_at(const glm::vec3& forward, const glm::vec3& w_up) {
	glm::vec3 f = -forward;
	glm::vec3 right(glm::cross(w_up, f));
	glm::vec3 up(glm::cross(f, right));
	glm::vec4 additional(0, 0, 0, 1);

	glm::mat4 rot(glm::vec4(right, 1),
		glm::vec4(up, 1),
		glm::vec4(f, 1),
		additional);
	rot = glm::transpose(rot);

	return rot;
}

void Camera::set_rot_mat() {
	m_rot_mat = glm::rotate(glm::mat4(1), glm::radians(m_rotation.y), (glm::vec3)UP);
	m_rot_mat = glm::rotate(m_rot_mat, glm::radians(m_rotation.x), (glm::vec3)RIGHT);
}

void Camera::set_rotation(const glm::vec2& rotation) {
	m_rotation = rotation;
	set_rot_mat();
}

void Camera::rotate(const glm::vec2& rot_delta) {
	m_rotation += rot_delta;
	set_rot_mat();
}