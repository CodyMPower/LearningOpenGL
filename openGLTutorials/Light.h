#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffuseIntensity);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	~Light();

private:
	glm::vec3 colour;
	GLfloat ambientIntensity;	// How much ambient light there is

	glm::vec3 direction;	// Direction the light is coming from
	GLfloat diffuseIntensity;	// How much directional light there is
};

