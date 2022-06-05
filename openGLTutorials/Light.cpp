#include "Light.h"

Light::Light() {
	colour = glm::vec3(1.0f, 1.0f, 1.0f);	// Default to wight light
	ambientIntensity = 1.0f;				// Default to full intensity
	diffuseIntensity = 0.0f;					// No diffuse
}

Light::Light(GLfloat shadowWidth, GLfloat shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat diffuseIntensity) {
	shadowMap = new ShadowMap();
	shadowMap->Init(shadowWidth, shadowHeight);
	
	colour = glm::vec3(red, green, blue);		// Set Colour to the specified R, G, B componetns
	this->ambientIntensity = ambientIntensity;	// Set ambient intensity
	this->diffuseIntensity = diffuseIntensity;
}

Light::~Light() {

}
