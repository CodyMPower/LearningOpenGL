#pragma once
#include <GL/glew.h>

class Material
{
public:

	Material();
	Material(GLfloat specularIntensity, GLfloat shininess);

	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

	~Material();

private:
	GLfloat specularIntensity;
	GLfloat shininess;
};

