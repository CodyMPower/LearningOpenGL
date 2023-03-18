#include "Camera.h"

Camera::Camera() {
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = 0;
	pitch = 0;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = 0;
	turnSpeed = 0;

	update();
}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed) {
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::update() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));	// Finds the "front" camera vector coordinates using the pitch and yaw values
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);									// Normalizes the vector (magnitude = 1)

	right = glm::normalize(glm::cross(front, worldUp));	// Normalized crossproduct of front x worldUp, order matters
	up = glm::normalize(glm::cross(right, front));		// Order also matters here, right x front

}

void Camera::keyControl(bool* keys, GLfloat deltaTime, bool lock) {
	if (lock && !keys[GLFW_KEY_LEFT_ALT])
		return;

	GLfloat velocity = moveSpeed * deltaTime;

	// Check if any given key is currently being activated
	if (keys[GLFW_KEY_W])	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S]) {
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A]) {
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D]) {
		position += right * velocity;
	}

	if (keys[GLFW_KEY_SPACE]) {
		position += up * velocity;
	}

	if (keys[GLFW_KEY_LEFT_CONTROL]) {
		position -= up * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange) {
	xChange *= turnSpeed;	// Relate axis changes to the speed of the turn
	yChange *= turnSpeed;

	yaw += xChange;		//	Change the yaw (L/R) position based on change in the x position of the mouse
	pitch += yChange;	//	Change the pitch (U/D) position based on the change in y position of the mouse

	if (pitch > 89.0f) {	// Can't allow pitch to equal 90 deg as this can cause problems with finding up and right vectors
		pitch = 89.0f;
	}

	if (pitch < -89.0f) {	//Same thing but looking down
		pitch = -89.0f;
	}

	update();	// Applies changes to the camera coordinates

}

glm::mat4 Camera::calculateViewMatrix() {
	return glm::lookAt(position, front + position, up);	// Returns a view matrix based on the camera's position, front, and world up vectors
}

Camera::~Camera() {

}