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

void PlayerObject::_applyRotation(glm::vec2* rotatedVector, double angle) {
	double sin_val = sin(angle);
	double cos_val = cos(angle);

	glm::mat2 rot_matrix = glm::mat2(glm::vec2(cos_val, sin_val),
										glm::vec2(-sin_val, cos_val));

	*rotatedVector = (* rotatedVector) * rot_matrix;
	 
}

glm::vec2 PlayerObject::getLookingVector() {
	glm::vec2 facingVec = glm::vec2(0.0f, -1.0f);
	
	_applyRotation(&facingVec, this->rotation_rad);

	//printf("Facing Direction x: %f\n", facingVec.x);
	//printf("Facing Direction z: %f\n\n", facingVec.y);

	return facingVec;
}

void PlayerObject::playerVision(bool vision_arr[], int vision_size, glm::vec2 food_arr[], int food_size, double angle_rads, double food_radius)
{
	double max_angle = angle_rads / 2;
	int steps = vision_size / 2;
	double angle = 0;
	glm::vec2 vision_vec;

	for (int i = 0; i < vision_size; i++)
	{
		for (int j = 0; j < food_size; j++)
		{
			if (vision_arr[i] != true)
			{
				angle = -((max_angle / steps) * (i - steps));

				vision_vec = getLookingVector();
				_applyRotation(&vision_vec, angle);

				vision_arr[i] = _seesFood(vision_vec, food_arr[j], food_radius);
			}
		}
	}
}



bool PlayerObject::_seesFood(glm::vec2 v1, glm::vec2 p2, double radius)
{
	glm::vec2 v2 = glm::vec2(-v1.y, v1.x);
	glm::vec2 p1 = glm::vec2(this->posVec.x, this->posVec.z);
	double denom = 0;
	double num = 0;
	double t1 = 0;
	double t2 = 0;
	//printf("V1.x: %f, V1.y: %f\n", v1.x, v1.y);
	//printf("V2.x: %f, V2.y: %f\n", v2.x, v2.y);
	//printf("P1.x: %f, P1.y: %f\n", p1.x, p1.y);
	//printf("P2.x: %f, P2.y: %f\n", p2.x, p2.y);
	if (v1.x >= 0.01)
	{
		// t2 calculation:
		denom = 1 - ((v1.y * v2.x) / (v1.x * v2.y));
		num = ((v1.y * p2.x) / (v1.x * v2.y)) + (p1.y / v2.y) - ((v1.y * p1.x) / (v1.x * v2.y)) - (p2.y / v2.y);
		t2 = num / denom;
		//printf("T2: %f\n", t2);

		num = v2.x * t2 + p2.x - p1.x;
		denom = v1.x;
		
		t1 = num / denom;

		return (abs(t2) <= radius) && (t1 > 0);
	}
	else
	{
		denom = 1 - ((v1.x * v2.y) / (v1.y * v2.x));
		num = ((v2.y * p1.x) / (v2.x * v1.y)) + (p2.y / v1.y) - ((v2.y * p2.x) / (v2.x * v1.y)) - (p1.y / v1.y);
		t1 = num / denom;

		num = v1.x * t1 + p1.x - p2.x;
		denom = v2.x;

		t2 = num / denom;

		//printf("T2: %f\n", t2);
		return (abs(t2) <= radius) && (t1 > 0);
	}
	return false;
}

bool PlayerObject::_tempFunc(glm::vec2 v1, glm::vec2 v2, glm::vec2 p1, glm::vec2 p2, double radius)
{
	double denom = 0;
	double num = 0;
	double t1 = 0;
	double t2 = 0;
	if (v1.x >= 0.01)
	{
		// t2 calculation:
		denom = 1 - ((v1.y * v2.x) / (v1.x * v2.y));
		num = ((v1.y * p2.x) / (v1.x * v2.y)) + (p1.y / v2.y) - ((v1.y * p1.x) / (v1.x * v2.y)) - (p2.y / v2.y);
		t2 = num / denom;
		printf("T2: %f\n", t2);

		num = v2.x * t2 + p2.x - p1.x;
		denom = v1.x;

		t1 = num / denom;

		return (abs(t2) <= radius) && (t1 > 0);
	}
	else
	{
		denom = 1 - ((v1.x * v2.y) / (v1.y * v2.x));
		num = ((v2.y * p1.x) / (v2.x * v1.y)) + (p2.y / v1.y) - ((v2.y * p2.x) / (v2.x * v1.y)) - (p1.y / v1.y);
		t1 = num / denom;

		num = v1.x * t1 + p1.x - p2.x;
		denom = v2.x;

		t2 = num / denom;

		printf("T2: %f\n", t2);
		return (abs(t2) <= radius) && (t1 > 0);
	}
	return false;
}

void PlayerObject::updatePlayer(GLfloat deltaTime) {

	rotation_rad += velocity.x * deltaTime * rot_speed;
	velocity.x = 0;

	// Apply Rotation Matrix to Velocity
	glm::vec2 vel_mod = glm::vec2(velocity.x, velocity.z);
	_applyRotation(&vel_mod, this->rotation_rad);// = rot_matrix * vel_mod;
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
