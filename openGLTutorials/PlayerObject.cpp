#include "PlayerObject.h"

PlayerObject::PlayerObject() {

}

PlayerObject::PlayerObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 size, RenderedObject* playerObject) {
	this->posVec = position;
	this->rotVec = rotation;
	this->sizeVec = size;
	this->playerObject = playerObject;
	this->friction = glm::vec3(-0.1f, 0.0f, -0.01f);
	this->gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	this->velocity = glm::vec3(0.0f);
	this->bound = (GLfloat)10.0f;
	this->speed = glm::vec3(5.0f, 1.0f, 5.0f);
}

void PlayerObject::updatePlayer(GLfloat deltaTime) {

	// Update player position based on velocity
	posVec += velocity * deltaTime * speed;

	onGround = posVec.y <= 0;
	velocity += gravity * deltaTime;	// Apply gravity to the velocity
	//if (onGround)
		velocity *= glm::vec3(0.0f, 1.0f, 0.0f);
	
	
	printf("Player X Pos: %f\n", posVec.x);
	printf("Player Y Pos: %f\n", posVec.y);
	printf("Player Z Pos: %f\n", posVec.z);
	// Set Player Bounds
	if (abs(posVec.x) > bound) {
		if (posVec.x > 0)
			posVec.x = bound;
		else
			posVec.x = -bound;
		velocity.x = 0;
	}

	if (abs(posVec.z) > bound) {
		if (posVec.z > 0)
			posVec.z = bound;
		else
			posVec.z = -bound;
		velocity.z = 0;
	}

	if (posVec.y < -1) {
		posVec.y = -1;
		velocity.y = 0;
	}

	playerObject->setTransformMatrix(posVec, rotVec, 0.0f, sizeVec);
}

void PlayerObject::keyControl(bool* keys) {

	if (keys[GLFW_KEY_S])
		velocity.z = 1;

	if (keys[GLFW_KEY_W])
		velocity.z = -1;

	if (keys[GLFW_KEY_A])
		velocity.x = -1;

	if (keys[GLFW_KEY_D])
		velocity.x = 1;

	if (keys[GLFW_KEY_SPACE] && onGround)
		velocity.y = 5;

	if (keys[GLFW_KEY_LEFT_SHIFT])
		speed = glm::vec3(10.0f, 1.0f, 10.0f);
	else
		speed = glm::vec3(5.0f, 1.0f, 5.0f);
}

RenderedObject* PlayerObject::getPlayerObject() {
	return playerObject;
}

PlayerObject::~PlayerObject() {

}