#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonVals.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "CommonVals.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class Shader
{
public:

	Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);		// Creates a shader program from char strings
	void CreateFromFile(const char* vertexLocation, const char* fragmentLocation);	// Creates a shader program from files

	std::string ReadFile(const char* fileLocation);	// Reads a file and coppies the file to a string

	GLuint GetProjectionLocation()			{ return uniformProjection; }			// Gets the location of the projection uniform
	GLuint GetModelLocation()				{ return uniformModel; }				// Gets the location of the model uniform
	GLuint GetViewLocation()				{ return uniformView; }					// Gets the location of the view uniform
	GLuint GetEyePositionLocation()			{ return uniformEyePosition; }			// Gets the location of the eyePosition uniform
	GLuint GetAmbientIntensityLocation()	{ return uniformDirectionalLight.uniformAmbientIntensity; }
	GLuint GetAmbientColourLocation()		{ return uniformDirectionalLight.uniformColour; }
	GLuint GetDiffuseIntensityLocation()	{ return uniformDirectionalLight.uniformDiffuseIntensity; }
	GLuint GetDirectionLocation()			{ return uniformDirectionalLight.uniformDirection; }
	GLuint GetSpecularIntensityLocation()	{ return uniformSpecularIntensity; }	// Gets the location of the specularIntensity uniform
	GLuint GetShininessLocation()			{ return uniformShininess; }			// Gets the location of the shininess uniform

	void SetDirectionalLight(DirectionalLight *dLight);
	void SetPointLights(PointLight* pLight, unsigned int lightCount);
	void SetSpotLights(SpotLight* sLight, unsigned int lightCount);
	void SetTexture(GLuint textureUint);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lightTransform);

	void UseShader();	// Uses the shader for rendering
	void ClearShader();	// Clears the shader program

	~Shader();

private:

	int pointLightCount;
	int spotLightCount;

	GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition,
		uniformSpecularIntensity, uniformShininess,
		uniformTexture,
		uniformDirectionalLightTransform, uniformDirectionalShadowMap;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;
		GLuint uniformDirection;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;
		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformPointLightCount;

	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;
		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
		GLuint uniformDirection;
		GLuint uniformEdge;
	}uniformSpotLight[MAX_SPOT_LIGHTS];

	GLuint uniformSpotLightCount;

	void CompileShader(const char* vertexCode, const char* fragmentCode);				// Compiles the shader program and shaders
	void AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType);	// Adds a shader to the shader program
};

