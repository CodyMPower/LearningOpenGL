#version 330

in vec4 vCol;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 FragPos;	// Fragment coordinates in world space

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;

struct Light {
vec3 colour;
float ambientIntensity;
float diffuseIntensity;
};

struct DirectionalLight {
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct Material{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform sampler2D theTexture;
uniform Material material;

uniform vec3 eyePosition;	// Camera Coordinates in world space

vec4 CalcLightByDirection(Light light, vec3 direction){

	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColour = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);

	vec4 specularColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if (diffuseFactor > 0.0f) {	// Don't bother with specular if diffuse can't reach surface
		vec3 fragToEye = normalize(eyePosition - FragPos);																	// Direction of fragment from eye position
		vec3 reflected = normalize(reflect(direction, normalize(Normal)));	// Where the light hits the surface, reflect reflects the first vector around the second vector
		// Specular is more intense as reflected approaches fragToEye

		float specularFactor = dot(fragToEye, reflected);

		if (specularFactor > 0.0f) {
			specularFactor = pow(specularFactor, material.shininess);																						// True specular factor
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);	// Final Specular colour calculation
		}

	}

	return (ambientColour + diffuseColour + specularColour);
}

vec4 CalcDirectionalLight(){
	return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLights(){

	vec4 totalColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < pointLightCount; i ++) {
		vec3 direction = FragPos - pointLights[i].position;
		float distance = length(direction);
		direction = normalize(direction);

		vec4 colour = CalcLightByDirection(pointLights[i].base, direction);
		float attenuation = pointLights[i].exponent * distance * distance +
												pointLights[i].linear * distance;
												pointLights[i].constant;

		totalColour += (colour / attenuation);
	}

	return totalColour;
}

void main() {
	vec4 finalColour = CalcDirectionalLight();
	finalColour += CalcPointLights();

	colour = texture(theTexture, TexCoord0) * finalColour;
}
