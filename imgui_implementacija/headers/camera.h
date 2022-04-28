#pragma once
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

class Camera {
private:
	glm::vec3 m_target;
	glm::vec2 m_rotation;
	float m_distance;
	glm::mat4 m_rot_mat;

	glm::mat4 look_at(const glm::vec3& forward, const glm::vec3& w_up);
	void set_rot_mat();

public:
	static const glm::vec4 RIGHT, UP, FORWARD;

	Camera(float distance, const glm::vec3& target, const glm::vec2& rotation = glm::vec2(0)) : m_distance(distance), m_target(target), m_rotation(rotation) { set_rot_mat(); }

	Camera() : Camera(1, glm::vec3(0)) { set_rot_mat(); }

	void set_distance(float distance) { m_distance = distance < 0 ? 0 : distance; }

	void set_target(const glm::vec3& target) { m_target = target; }

	void set_rotation(const glm::vec2& rotation);

	void rotate(const glm::vec2& rot_delta);

	float distance() { return m_distance; }

	glm::vec3 position() { return m_target - forward() * distance(); }

	glm::vec2 rotation() { return m_rotation; }

	glm::vec3 forward() { return m_rot_mat * FORWARD; }

	glm::vec3 right() { return m_rot_mat * RIGHT; }

	glm::vec3 up() { return m_rot_mat * UP; }

	glm::mat4 view_mat() { return glm::lookAt(position(), m_target, up()); }
};