#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

class RenderedObject
{
public:

	RenderedObject();
	RenderedObject(Mesh* objectMesh, Texture* objectTexture, Material* objectMaterial);

	glm::mat4 getTransformMatrix() { return transformMatrix; }
	Mesh* getMesh() { return objectMesh; }
	Texture* getTexture() { return objectTexture; }
	Material* getMaterial() { return objectMaterial; }

	void setTransformMatrix(glm::mat4 transformMatrix) { this->transformMatrix = transformMatrix; }
	void setTransformMatrix(glm::vec3 posVec, glm::vec3 rotVec, float rotationAngle, glm::vec3 sizeVec);

	void calculateTransformMatrix();

	void renderObject(GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess);

	~RenderedObject();

private:
	glm::mat4 transformMatrix;
	Mesh* objectMesh;
	Texture* objectTexture;
	Material* objectMaterial;

	float rotationAngle;
	glm::vec3 posVec, rotVec, sizeVec;

};

