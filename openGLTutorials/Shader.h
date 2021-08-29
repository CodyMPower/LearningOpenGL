#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public:

	Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);		// Creates a shader program from char strings
	void CreateFromFile(const char* vertexLocation, const char* fragmentLocation);	// Creates a shader program from files

	std::string ReadFile(const char* fileLocation);	// Reads a file and coppies the file to a string

	GLuint GetProjectionLocation() { return uniformProjection; }				// Gets the location of the projection uniform
	GLuint GetModelLocation() { return uniformModel; }							// Gets the location of the model uniform
	GLuint GetViewLocation() { return uniformView; }							// Gets the location of the view uniform
	GLuint GetEyePositionLocation() { return uniformEyePosition; }				// Gets the location of the eyePosition uniform
	GLuint GetAmbientIntensityLocation() { return uniformAmbientIntensity; }	// Gets the location of the ambientIntensity uniform
	GLuint GetAmbientColourLocation() { return uniformAmbientColour; }			// Gets the location of the ambientColour uniform
	GLuint GetDiffuseIntensityLocation() { return uniformDiffuseIntensity; }	// Gets the location of the diffuseIntensity uniform
	GLuint GetDirectionLocation() { return uniformDirection; }					// Gets the location of the direction uniform
	GLuint GetSpecularIntensityLocation() { return uniformSpecularIntensity; }	// Gets the location of the specularIntensity uniform
	GLuint GetShininessLocation() { return uniformShininess; }					// Gets the location of the shininess uniform

	void UseShader();	// Uses the shader for rendering
	void ClearShader();	// Clears the shader program

	~Shader();

private:
	GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition,
		uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection,
		uniformSpecularIntensity, uniformShininess;

	void CompileShader(const char* vertexCode, const char* fragmentCode);				// Compiles the shader program and shaders
	void AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType);	// Adds a shader to the shader program
};

