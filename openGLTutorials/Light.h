#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, 
			GLfloat ambientIntensity, GLfloat diffuseIntensity);

	~Light();

	// Protected only allows subclasses to access the information unlike in Java
protected:
	glm::vec3 colour;
	GLfloat ambientIntensity;	// How much ambient light there is
	GLfloat diffuseIntensity;	// How much directional light there is
};

