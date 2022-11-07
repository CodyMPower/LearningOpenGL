#include "PlayerObject.h"

PlayerObject::PlayerObject(RenderedObject* playerObject) {
	this->posVec = glm::vec3(0.0f, 1.0f, 0.0f);
	this->rotVec = glm::vec3(0.0f, 1.0f, 0.0f);
	this->sizeVec = glm::vec3(1.0f, 1.0f, 1.0f);
	this->playerObject = playerObject;
	_setDefaults();
}

PlayerObject::PlayerObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 size, RenderedObject* playerObject) {
	this->posVec = position;
	this->rotVec = rotation;
	this->sizeVec = size;
	this->playerObject = playerObject;
	_setDefaults();
}

void PlayerObject::_setDefaults()
{
	this->friction = glm::vec3(-0.1f, 0.0f, -0.01f);
	this->gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	this->velocity = glm::vec3(0.0f);
	this->bound = (GLfloat)10.0f;
	this->speed = glm::vec3(5.0f, 1.0f, 5.0f);
	this->rotation_rad = 0.0f;
	this->rot_speed = 2.0f;
}

void PlayerObject::_applyRotation(glm::vec2* rotatedVector) {
	double sin_val = sin(this->rotation_rad);
	double cos_val = cos(this->rotation_rad);

	glm::mat2 rot_matrix = glm::mat2(glm::vec2(cos_val, sin_val),
										glm::vec2(-sin_val, cos_val));

	*rotatedVector = (* rotatedVector) * rot_matrix;
	 
}

glm::vec2 PlayerObject::getLookingVector() {
	glm::vec2 facingVec = glm::vec2(0.0f, -1.0f);
	
	_applyRotation(&facingVec);

	printf("Facing Direction x: %f\n", facingVec.x);
	printf("Facing Direction z: %f\n\n", facingVec.y);

	return facingVec;
}

void PlayerObject::updatePlayer(GLfloat deltaTime) {

	rotation_rad += velocity.x * deltaTime * rot_speed;
	velocity.x = 0;

	// Apply Rotation Matrix to Velocity
	glm::vec2 vel_mod = glm::vec2(velocity.x, velocity.z);
	_applyRotation(&vel_mod);// = rot_matrix * vel_mod;
	getLookingVector();

	velocity.x = vel_mod.x;
	velocity.z = vel_mod.y;

	// Update player position based on velocity
	posVec += velocity * deltaTime * speed;

	onGround = posVec.y <= -1;
	velocity += gravity * deltaTime;	// Apply gravity to the velocity
	//if (onGround)
		velocity *= glm::vec3(0.0f, 1.0f, 0.0f);
	
	// transformMatrix = glm::rotate(transformMatrix, this->rotationAngle, this->rotVec);

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

	if (onGround) {
		posVec.y = -1;
		velocity.y = 0;
	}

	playerObject->setTransformMatrix(posVec, rotVec, rotation_rad, sizeVec);
}

void PlayerObject::keyControl(bool* keys) {
	if (keys[GLFW_KEY_LEFT_ALT])
		return;

	if (keys[GLFW_KEY_S])
		velocity.z = 1;

	if (keys[GLFW_KEY_W])
		velocity.z = -1;

	if (keys[GLFW_KEY_A])
		velocity.x = 1;

	if (keys[GLFW_KEY_D])
		velocity.x = -1;

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
