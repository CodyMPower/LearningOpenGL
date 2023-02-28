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
	this->bound = (GLfloat)20.0f;
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
	glm::vec2 facingVec = glm::vec2(0.0f, -1.0f);	// Create a temporary vector to store the information about what direction the player is facing
	
	_applyRotation(&facingVec, this->rotation_rad);	// Applied the rotation of the player to rotate the vector to the looking direction

	return facingVec;
}

void PlayerObject::playerVision(bool vision_arr[], int vision_size, glm::vec2 food_arr[], int food_size, double angle_rads, double food_radius)
{
	double max_angle = angle_rads / 2;
	int steps = vision_size / 2;
	double angle = 0;
	glm::vec2 vision_vec;

	for (int i = 0; i < vision_size; i++)						// For all the lines of vision, see if a piece of food can be seen
	{
		for (int j = 0; j < food_size; j++)						// For all the pieces of food, check if the line intersects the food
		{
			if (vision_arr[i] != true)							// Only check if the line hasn't seen the food yet
			{
				angle = -((max_angle / steps) * (i - steps));	// Calculate the angle of the line in refference to the current direction the creature is facing

				vision_vec = getLookingVector();				// Get the direction the player is facing
				_applyRotation(&vision_vec, angle);				// Rotate the looking vector to face in the same direction as the line

				vision_arr[i] = _seesFood(vision_vec, food_arr[j], food_radius);	// Check if the line intersects with the food
			}
		}
	}
}

bool PlayerObject::_seesFood(glm::vec2 v1, glm::vec2 p2, double radius)
{
	// Setup Variables to calculate
	glm::vec2 v2 = glm::vec2(-v1.y, v1.x);
	glm::vec2 p1 = glm::vec2(this->posVec.x, this->posVec.z);
	double denom = 0;
	double num = 0;
	double t1 = 0;
	double t2 = 0;

	if (v1.x >= 0.01)	// Values approach infinity as a vector becomes vertical
	{
		// t2 calculation according to vector math:
		num = ((v1.y * p2.x) / (v1.x * v2.y)) + (p1.y / v2.y) - ((v1.y * p1.x) / (v1.x * v2.y)) - (p2.y / v2.y);
		denom = 1 - ((v1.y * v2.x) / (v1.x * v2.y));
		t2 = num / denom;

		// t1 calculation according to vector math:
		num = v2.x * t2 + p2.x - p1.x;
		denom = v1.x;
		t1 = num / denom;
	}
	else
	{
		// t1 calculation according to vector math:
		num = ((v2.y * p1.x) / (v2.x * v1.y)) + (p2.y / v1.y) - ((v2.y * p2.x) / (v2.x * v1.y)) - (p1.y / v1.y);
		denom = 1 - ((v1.x * v2.y) / (v1.y * v2.x));
		t1 = num / denom;

		// t1 calculation according to vector math:
		num = v1.x * t1 + p1.x - p2.x;
		denom = v2.x;
		t2 = num / denom;
	}

	// Bounds for vision
	return (abs(t2) <= radius) && (t1 > 0);
}

void PlayerObject::updatePlayer(GLfloat deltaTime) {

	// Find the rotation angle based on time elapsed
	rotation_rad += velocity.x * deltaTime * rot_speed;
	velocity.x = 0;

	// Apply Rotation Matrix to Velocity to get correct direction to move
	glm::vec2 vel_mod = glm::vec2(velocity.x, velocity.z);
	_applyRotation(&vel_mod, this->rotation_rad);// = rot_matrix * vel_mod;
	getLookingVector();

	// Get the velocity modifier to move the player in the direction it is facing
	velocity.x = vel_mod.x;
	velocity.z = vel_mod.y;

	// Apply the gravity vector
	velocity += gravity * deltaTime;

	// Update player position based on velocity and time passed
	posVec += velocity * deltaTime * speed;
	
	
	// transformMatrix = glm::rotate(transformMatrix, this->rotationAngle, this->rotVec);

	// Set Player Bounds
	onGround = posVec.y <= -1;
	if (onGround)				// Entity cannot pass through the ground
		posVec.y = -1;

	// Keep entity confined to a specific area
	if (abs(posVec.x) > bound) {
		if (posVec.x > 0)
			posVec.x = bound;
		else
			posVec.x = -bound;
	}

	if (abs(posVec.z) > bound) {
		if (posVec.z > 0)
			posVec.z = bound;
		else
			posVec.z = -bound;
	}

	velocity.x = 0.0f;
	velocity.z = 0.0f;

	playerObject->setTransformMatrix(posVec, rotVec, rotation_rad, sizeVec);
}

void PlayerObject::keyControl(bool* keys) {
	if (keys[GLFW_KEY_LEFT_ALT])
		return;

	if (keys[GLFW_KEY_S])
		velocity.z = 1;		// Move Backwards

	if (keys[GLFW_KEY_W])
		velocity.z = -1;	// Move Forwards

	if (keys[GLFW_KEY_A])
		velocity.x = 1;		// Rotate Left

	if (keys[GLFW_KEY_D])
		velocity.x = -1;	// Rotate Right

	if (keys[GLFW_KEY_SPACE] && onGround)
		velocity.y = 5;		// Jump

	if (keys[GLFW_KEY_LEFT_SHIFT])	// Sprint
		speed = glm::vec3(10.0f, 1.0f, 10.0f);
	else
		speed = glm::vec3(5.0f, 1.0f, 5.0f);
}

void PlayerObject::modelCondrol(std::vector<bool> inputs)
{

	if (inputs.size() < 3)
		return;

	if (inputs.at(0))
		velocity.x = 1;		// Rotate Left

	if (inputs.at(2))
		velocity.x = -1;	// Rotate Right

	if (inputs.at(1))
		velocity.z = -1;		// Move Forwards
}

RenderedObject* PlayerObject::getPlayerObject() {
	return playerObject;
}

PlayerObject::~PlayerObject() {

}
