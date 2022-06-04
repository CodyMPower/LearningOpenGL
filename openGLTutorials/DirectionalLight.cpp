#include "DirectionalLight.h"
// : Light() calls the light constructor, don't need to call super() like in Java
DirectionalLight::DirectionalLight() : Light(){

	direction = glm::vec3(0.0f, -1.0f, 0.0f);	// Light pointing straight down
	lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

DirectionalLight::DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight, 
									GLfloat red, GLfloat green, GLfloat blue,
									GLfloat ambientIntensity, GLfloat diffuseIntensity,
									GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(shadowWidth, shadowHeight, red, green, blue, ambientIntensity, diffuseIntensity) {

	direction = glm::vec3(xDir, yDir, zDir);
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

void DirectionalLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation, GLuint diffuseIntensityLocation, GLuint directionLocation) {
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);	// Loads the colour values to the uniform ambientColour location
	glUniform1f(ambientIntensityLocation, ambientIntensity);			// Loads the intensity value to the uniform ambientIntensity location
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	return lightProj * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

DirectionalLight::~DirectionalLight() {

}