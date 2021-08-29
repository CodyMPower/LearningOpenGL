#include "Material.h"

Material::Material() {
	specularIntensity = 0;
	shininess = 0;
}

Material::Material(GLfloat specularIntensity, GLfloat shininess) {
	this->specularIntensity = specularIntensity;
	this->shininess = shininess;
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) {
	glUniform1f(specularIntensityLocation, specularIntensity);	// Set specularIntensity uniform in frag shader
	glUniform1f(shininessLocation, shininess);					// Set shininess uniform in frag shader
}

Material::~Material() {

}