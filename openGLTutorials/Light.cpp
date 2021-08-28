#include "Light.h"

Light::Light() {
	colour = glm::vec3(1.0f, 1.0f, 1.0f);	// Default to wight light
	ambientIntensity = 1.0f;				// Default to full intensity
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity) {
	colour = glm::vec3(red, green, blue);		// Set Colour to the specified R, G, B componetns
	this->ambientIntensity = ambientIntensity;	// Set ambient intensity
}

void Light::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation) {
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);	// Loads the colour values to the uniform ambientColour location
	glUniform1f(ambientIntensityLocation, ambientIntensity);			// Loads the intensity value to the uniform ambientIntensity location
}

Light::~Light() {

}
