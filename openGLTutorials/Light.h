#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShadowMap.h"

class Light
{
public:
	Light();
	Light(	GLfloat shadowWidth, GLfloat shadowHeight,
			GLfloat red, GLfloat green, GLfloat blue, 
			GLfloat ambientIntensity, GLfloat diffuseIntensity);

	ShadowMap* GetShadowMap() { return shadowMap; }


	~Light();

	// Protected only allows subclasses to access the information unlike in Java
protected:
	glm::vec3 colour;
	GLfloat ambientIntensity;	// How much ambient light there is
	GLfloat diffuseIntensity;	// How much directional light there is

	glm::mat4 lightProj;

	ShadowMap* shadowMap;
};

