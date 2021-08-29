#include "Light.h"

Light::Light() {
	colour = glm::vec3(1.0f, 1.0f, 1.0f);	// Default to wight light
	ambientIntensity = 1.0f;				// Default to full intensity

	direction = glm::vec3(0.0f, -1.0f, 0.0f);	// Light pointing straight down
	diffuseIntensity = 0.0f;					// No diffuse
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffuseIntensity) {
	colour = glm::vec3(red, green, blue);		// Set Colour to the specified R, G, B componetns
	this->ambientIntensity = ambientIntensity;	// Set ambient intensity

	direction = glm::vec3(xDir, yDir, zDir);
	this->diffuseIntensity = diffuseIntensity;
}

void Light::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation, GLuint diffuseIntensityLocation, GLuint directionLocation) {
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);	// Loads the colour values to the uniform ambientColour location
	glUniform1f(ambientIntensityLocation, ambientIntensity);			// Loads the intensity value to the uniform ambientIntensity location

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

Light::~Light() {

}
