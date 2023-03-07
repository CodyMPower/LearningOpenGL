#pragma once

#include <stdio.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderedObject.h"

class PlayerObject
{
public:

	PlayerObject(RenderedObject* playerObject);
	PlayerObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 size, RenderedObject* playerObject);

	glm::vec2 getLookingVector();
	void playerVision(bool vision_arr[], int vision_size, glm::vec2 food_arr[], int food_size, double angle_rads, double food_radius);
	bool _seesFood(glm::vec2 v1, glm::vec2 p2, double radius);
	void updatePlayer(GLfloat deltaTime);
	void keyControl(bool* keys);
	void modelCondrol(std::vector<bool> inputs);

	void setGravity(glm::vec3 gravity) { this->gravity = gravity; }
	void setFriction(glm::vec3 friction) { this->friction = friction;  }

	RenderedObject* getPlayerObject();

	~PlayerObject();

private:

	void _setDefaults();
	void _applyRotation(glm::vec2* rotatedVec, double angle);

	glm::vec3 posVec, rotVec, sizeVec, gravity, velocity, friction, speed, facingVec;
	GLfloat rotation_rad, bound, rot_speed;
	RenderedObject* playerObject;
	bool onGround;

};