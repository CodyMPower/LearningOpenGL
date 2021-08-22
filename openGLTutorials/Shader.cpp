#include "Shader.h"

Shader::Shader() {
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFile(const char* vertexLocation, const char* fragmentLocation) {
	std::string vertexString = ReadFile(vertexLocation);	// Coppies shader code from file to application
	const char* vertexCode = vertexString.c_str();			// Converts string to char string

	std::string fragmentString = ReadFile(fragmentLocation);	
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation) {
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read '%s', file does not exist or could not be found at the location", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);	// Coppies data line from file to the application
		content.append(line + "\n");
	}

	fileStream.close();
	return content;

}

void Shader::UseShader() {
	glUseProgram(shaderID);
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

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);	// Assigns a vertex shader to the shader program
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);	// Assigns a fragment shader to the shader program

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderID);								// Links the shader program
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);	// Gets the result of the linkage process

	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Link Program: '%s'\n", errorLog);
		return;
	}

	glValidateProgram(shaderID);								// Validates the shader program
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);	// Gets the result of the validation process

	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Validate Program: '%s'\n", errorLog);
		return;
	}

	uniformModel = glGetUniformLocation(shaderID, "model");				// Gets the location of the uniform variable "model"
	uniformProjection = glGetUniformLocation(shaderID, "projection");	// Gets the location of the uniform variable "model"
	uniformView = glGetUniformLocation(shaderID, "view");				// Gets the location of the unifrom variable "view"
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

	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Link '%d' Shader: '%s'\n", shaderType, errorLog);
		return;
	}

	glAttachShader(shaderProgram, theShader);	// Attaches the shader to the shader program
}