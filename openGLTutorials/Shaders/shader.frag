#version 330

in vec4 vCol;
in vec2 TexCoord0;
in vec3 Normal;
in vec3 FragPos;	// Fragment coordinates in world space
in vec4 DirectionalLightSpacePos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

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

struct SpotLight {
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;	// How many point lights are available
uniform int spotLightCount;		// How many spot lights are available

uniform DirectionalLight directionalLight;			// A directional light (sunlight) to calculate lighting
uniform PointLight pointLights[MAX_POINT_LIGHTS];	// A list of point lights to calculate lighting
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];		// A list of spotlights to calculate lighting

uniform sampler2D theTexture;			// Texture sample for the mesh
uniform sampler2D directionalShadowMap;	// Depth sample of the world for shadow calculations

uniform Material material;	// Material being used with the mesh

uniform vec3 eyePosition;	// Camera Coordinates in world space

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------<Code>-------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
float CalcDirectionalShadowFactor(DirectionalLight light) {
	
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w; // Normalizing directional light to "Normalized device coordinates" (-1 to 1)
	projCoords = (projCoords * 0.5f) + 0.5f; // Mapping from -1 to 1 over to 0 to 1
	
	float currentDepth = projCoords.z;
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.direction);
	
	float shadow = 0.0f;
	
	float bias = max(0.05f * (1 - dot(normal, lightDir)), 0.005);
	
	vec2 texelSize = 1.0f / textureSize(directionalShadowMap, 0);
	for (int x = -1; x <= 1; x ++){
		for (int y = -1; y <= 1; y ++) {
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; //Shadow Map is 2D, so we take the xy coords of the projection coordinates to get the closest point, then compare that to the point being looked at
			shadow += currentDepth - bias > pcfDepth ? 1.0f: 0.0f;
		}
	}
	
	shadow /= 9.0f;
	
	if(projCoords.z > 1.0f)
	{
		shadow = 0.0f;
	}
	
	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor){

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

	return (ambientColour + (1.0 - shadowFactor) * (diffuseColour + specularColour)); // Leaves only ambient color if point is in the shadow
}

vec4 CalcDirectionalLight(){
	float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight){
	vec3 direction = FragPos - pLight.position;
	float distance = length(direction);
	direction = normalize(direction);

	vec4 colour = CalcLightByDirection(pLight.base, direction, 0.0f);
	float attenuation = pLight.exponent * distance * distance +
											pLight.linear * distance +
											pLight.constant;

	return (colour / attenuation);
}

vec4 CalcSpotLight(SpotLight sLight){
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection, sLight.direction); // Angle between light ray and light direction

	if (slFactor > sLight.edge) {
		vec4 colour = CalcPointLight(sLight.base);

		return colour * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));
	} else {
		return vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

}

vec4 CalcPointLights(){

	vec4 totalColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < pointLightCount; i ++) {
		totalColour += CalcPointLight(pointLights[i]);
	}

	return totalColour;
}

vec4 CalcSpotLights(){

	vec4 totalColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < spotLightCount; i ++) {
		totalColour += CalcSpotLight(spotLights[i]);
	}

	return totalColour;
}

void main() {

	vec4 finalColour = CalcDirectionalLight();
	finalColour += CalcPointLights();
	finalColour += CalcSpotLights();

	colour = texture(theTexture, TexCoord0) * finalColour;
}
