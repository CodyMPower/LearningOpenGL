#include "DirectionalLight.h"
// : Light() calls the light constructor, don't need to call super() like in Java
DirectionalLight::DirectionalLight() : Light(){

	direction = glm::vec3(0.0f, -1.0f, 0.0f);	// Light pointing straight down
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
								GLfloat ambientIntensity, GLfloat diffuseIntensity,
								GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(red, green, blue, ambientIntensity, diffuseIntensity) {

	direction = glm::vec3(xDir, yDir, zDir);
}

void DirectionalLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation, GLuint diffuseIntensityLocation, GLuint directionLocation) {
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);	// Loads the colour values to the uniform ambientColour location
	glUniform1f(ambientIntensityLocation, ambientIntensity);			// Loads the intensity value to the uniform ambientIntensity location
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

DirectionalLight::~DirectionalLight() {

}