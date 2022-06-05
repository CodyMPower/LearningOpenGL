#include "Shader.h"

Shader::Shader() {
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;

	pointLightCount = 0;
	spotLightCount = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFile(const char* vertexLocation, const char* fragmentLocation) {
	std::string vertexString = ReadFile(vertexLocation);	// Coppies vertex shader code from file to application
	const char* vertexCode = vertexString.c_str();			// Converts string to char string

	std::string fragmentString = ReadFile(fragmentLocation);	// Coppies fragment shader code from file to application
	const char* fragmentCode = fragmentString.c_str();			// Converts string to char string
	
	CompileShader(vertexCode, fragmentCode);	// Compiles the shader using coppied shader code
}

std::string Shader::ReadFile(const char* fileLocation) {
	std::string content;									// Local string to contain the content of the file
	std::ifstream fileStream(fileLocation, std::ios::in);	// (BufferedInputStream?) stream from the input file

	if (!fileStream.is_open()) {	// Checks if the file could be located
		printf("Failed to read '%s', file does not exist or could not be found at the location", fileLocation);
		return "";
	}

	std::string line = "";				// Temp variable to copy line information
	while (!fileStream.eof()) {			// Loops while the file stream has not reached eof (end of file)
		std::getline(fileStream, line);	// Coppies data line from file to the application
		content.append(line + "\n");	// Appends the line to the content string with a newline
	}

	fileStream.close();	// Close the fileStream after use
	return content;		// Returns the content of the file

}

void Shader::SetDirectionalLight(DirectionalLight* dLight) {
	dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColour,
					uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight* pLight, unsigned int lightCount) {
	if (lightCount > MAX_POINT_LIGHTS) {
		lightCount = MAX_POINT_LIGHTS;
	}

	glUniform1i(uniformPointLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i ++) {
		pLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColour,
							uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition,
							uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);
	}

}

void Shader::SetSpotLights(SpotLight* sLight, unsigned int lightCount) {
	if (lightCount > MAX_SPOT_LIGHTS) {
		lightCount = MAX_SPOT_LIGHTS;
	}

	glUniform1i(uniformSpotLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {
		sLight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformColour,
			uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformConstant, uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformEdge);
	}

}

void Shader::SetTexture(GLuint textureUint)
{
	glUniform1i(uniformTexture, textureUint);
}

void Shader::SetDirectionalShadowMap(GLuint textureUnit)
{
	glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::SetDirectionalLightTransform(glm::mat4* lightTransform)
{
	glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lightTransform));
}

void Shader::UseShader() {
	glUseProgram(shaderID);	// Uses the shader program in the GPU
}

void Shader::ClearShader() {
	if (shaderID != 0) {
		glDeleteProgram(shaderID);	// Deletes the shader program from the GPU
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

Shader::~Shader() {
	ClearShader();
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode) {
	shaderID = glCreateProgram();	// Creates a shader program ID

	if (!shaderID) {	// Check if the shader program was created successfully
		printf("Failed to Create Shader Program");
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);		// Assigns a vertex shader to the shader program
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);	// Assigns a fragment shader to the shader program

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderID);							// Links the shader program
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);	// Gets the result of the linkage process

	if (!result) {	// Checks if the program linking process passed
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Link Program: '%s'\n", errorLog);
		return;
	}

	glValidateProgram(shaderID);							// Validates the shader program
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);	// Gets the result of the validation process

	if (!result) {	// Checks if the program validation passed
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Validate Program: '%s'\n", errorLog);
		return;
	}

	uniformModel				= glGetUniformLocation(shaderID, "model");								// Gets the location of the uniform variable "model"
	uniformProjection			= glGetUniformLocation(shaderID, "projection");							// Gets the location of the uniform variable "model"
	uniformView					= glGetUniformLocation(shaderID, "view");								// Gets the location of the unifrom variable "view"
	uniformEyePosition			= glGetUniformLocation(shaderID, "eyePosition");						// Gets the location of the unifrom variable "eyePosition"
	uniformDirectionalLight.uniformAmbientIntensity		= glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");	// Gets the location of the uniform variable "ambientIntensity" inside the uniform struct "directionalLight"
	uniformDirectionalLight.uniformColour				= glGetUniformLocation(shaderID, "directionalLight.base.colour");			// Gets the location of the unifrom variable "colour" inside the uniform struct "directionalLight"
	uniformDirectionalLight.uniformDiffuseIntensity		= glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");	// Gets the location of the uniform variable "diffuseIntensity" inside the uniform struct "directionalLight"
	uniformDirectionalLight.uniformDirection			= glGetUniformLocation(shaderID, "directionalLight.direction");				// Gets the location of the unifrom variable "direction" inside the uniform struct "directionalLight"
	uniformSpecularIntensity	= glGetUniformLocation(shaderID, "material.specularIntensity");			// Gets the location of the uniform variable "specularIntensity" inside the uniform struct "material"
	uniformShininess			= glGetUniformLocation(shaderID, "material.shininess");					// Gets the location of the unifrom variable "shininess" inside the uniform struct "material"
	uniformPointLightCount		= glGetUniformLocation(shaderID, "pointLightCount");
	uniformSpotLightCount		= glGetUniformLocation(shaderID, "spotLightCount");

	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++) {
		char locBuff[100] = {'\0'};	// All characters are null termination characters

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.colour", i);	// snprintf prints to a buffer at the locBuff location, .base.colour will be important
		uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);	// snprintf prints to a buffer at the locBuff location, .base.colour will be important
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);	// snprintf prints to a buffer at the locBuff location, .base.colour will be important
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);	// snprintf prints to a buffer at the locBuff location, .base.colour will be important
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);	// snprintf prints to a buffer at the locBuff location, .base.colour will be important
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);	// snprintf prints to a buffer at the locBuff location, .base.colour will be important
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);	// snprintf prints to a buffer at the locBuff location, .base.colour will be important
		uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

	}

	// Same function as above loop exept initilizing spotlights
	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.colour", i);
		uniformSpotLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);

	}

	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
	uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
	uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");

}

void Shader::AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);	// Creates an ID for the shader type specified

	const GLchar* theCode[1];	// A char pointer in an array to point to the shader code
	theCode[0] = shaderCode;	// Pointer points to the shader code

	GLint codeLength[1];				// A GLint in an array to store the length of the shader code
	codeLength[0] = strlen(shaderCode);	// Stores the length of the shader code

	glShaderSource(theShader, 1, theCode, codeLength);
	// Passes the shader code to the created shader
	// theShader: the shader to pass the code to
	// 1: how many shader code segments are being passed
	// theCode: an array of pointer(s) that point to the code of the shader
	// codeLength: an array of GLint(s) that specify how long each code segment is

	glCompileShader(theShader);	// Compiles the shader

	GLint result = 0;				// Sets up a variable to store the shader status results
	GLchar errorLog[1024] = { 0 };	// Sets up a char array to stor any errors that may have occured

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);	// Gets the result of the Compilation process

	if (!result) {	// Checks if the shader linked to the shader program
		glGetShaderInfoLog(theShader, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Link '%d' Shader: '%s'\n", shaderType, errorLog);
		return;
	}

	glAttachShader(shaderProgram, theShader);	// Attaches the shader to the shader program
}