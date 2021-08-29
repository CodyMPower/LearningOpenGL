#version 330

in vec4 vCol;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 FragPos;	// Fragment coordinates in world space

out vec4 colour;

struct DirectionalLight {
	vec3 colour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

struct Material{
	float specularIntensity;
	float shininess;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform vec3 eyePosition;	// Camera Coordinates in world space

void main() {
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
	vec4 diffuseColour = vec4(directionalLight.colour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

	vec4 specularColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if (diffuseFactor > 0.0f) {	// Don't bother with specular if diffuse can't reach surface
		vec3 fragToEye = normalize(eyePosition - FragPos);																	// Direction of fragment from eye position
		vec3 reflected = normalize(reflect(directionalLight.direction, normalize(Normal)));	// Where the light hits the surface, reflect reflects the first vector around the second vector
		// Specular is more intense as reflected approaches fragToEye

		float specularFactor = dot(fragToEye, reflected);

		if (specularFactor > 0.0f) {
			specularFactor = pow(specularFactor, material.shininess);																						// True specular factor
			specularColour = vec4(directionalLight.colour * material.specularIntensity * specularFactor, 1.0f);	// Final Specular colour calculation
		}

	}

	colour = texture(theTexture, TexCoord0) * (ambientColour + diffuseColour + specularColour);
}
