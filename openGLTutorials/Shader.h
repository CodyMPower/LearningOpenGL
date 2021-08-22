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

	GLuint GetProjectionLocation() { return uniformProjection; }	// Gets the location of the projection uniform
	GLuint GetModelLocation() { return uniformModel; }				// Gets the location of the model uniform
	GLuint GetViewLocation() { return uniformView; }				// Gets the location of the model uniform

	void UseShader();	// Uses the shader for rendering
	void ClearShader();	// Clears the shader program

	~Shader();

private:
	GLuint shaderID, uniformProjection, uniformModel, uniformView;

	void CompileShader(const char* vertexCode, const char* fragmentCode);				// Compiles the shader program and shaders
	void AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType);	// Adds a shader to the shader program
};

