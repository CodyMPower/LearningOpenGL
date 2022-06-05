#include "RenderedObject.h"

RenderedObject::RenderedObject()
{
	calculateTransformMatrix();
}

RenderedObject::RenderedObject(Mesh* objectMesh, Texture* objectTexture, Material* objectMaterial)
{
	calculateTransformMatrix();
	this->objectMesh = objectMesh;
	this->objectTexture = objectTexture;
	this->objectMaterial = objectMaterial;
}

void RenderedObject::setTransformMatrix(glm::vec3 posVec, glm::vec3 rotVec, float rotationAngle, glm::vec3 sizeVec) {
	this->posVec = posVec;
	this->rotVec = rotVec;
	this->sizeVec = sizeVec;
	this->rotationAngle = rotationAngle;
}

void RenderedObject::calculateTransformMatrix() {
	transformMatrix = glm::mat4(1.0f);
	transformMatrix = glm::translate(transformMatrix, this->posVec);
	transformMatrix = glm::rotate(transformMatrix, this->rotationAngle, this->rotVec);
	transformMatrix = glm::scale(transformMatrix, this->sizeVec);
}

void RenderedObject::renderObject(GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess) {
	calculateTransformMatrix();
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(transformMatrix));
	objectTexture->UseTexture();
	objectMaterial->UseMaterial(uniformSpecularIntensity, uniformShininess);
	objectMesh->RenderMesh();
}

RenderedObject::~RenderedObject() {

}