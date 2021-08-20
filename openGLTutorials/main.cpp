#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;	// Window Dimensions

GLuint VAO, VBO, shader, uniformModel;	// Shader variables

bool leftDirection = true;		// Is the triangle moving left?
float triOffset = 0.0f;			// The current offset
float triMaxOffset = 0.7f;		// The maximum offset of the triangle
float triIncrement = 0.005f;	// The increment value to the offset

float toRadians = 3.14159265f / 180.0f;
float curAngle = 0.0f;
float angleIncrement = 0.01;

/*	Vertex Shader
	gl_Position is a predefined out vec4 variable
*/
static const char* vShader = "								\n\
#version 330												\n\
															\n\
layout (location = 0) in vec3 pos;							\n\
															\n\
uniform mat4 model;											\n\
															\n\
void main(){												\n\
	gl_Position = model * vec4(pos, 1.0f);					\n\
}															\n\
";

/*	Fragment Shader
	any out vec4 will be assumed to be the colour of the pixel on the screen
*/
static const char* fShader = "								\n\
#version 330												\n\
															\n\
out vec4 colour;											\n\
															\n\
void main(){												\n\
	colour = vec4(1.0, 0.0, 0.0, 1.0);						\n\
}															\n\
";

void CreateTriangle() {
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f , -1.0f, 0.0f,
		0.0f , 1.0f , 0.0f
	};

	glGenVertexArrays(1, &VAO);	// Creates 1 VAO and returns the VAO ID
	glBindVertexArray(VAO);		// Binds the VAO to bind VBOs

	glGenBuffers(1, &VBO);				// Creates 1 VBO and returns the VBO ID
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Binds a VBO to the target buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// buffers the data to the target VBO
		// GL_ARRAY_BUFFER:		The target to bind the data to
		// sizeof(vertices):	The size of the data being buffered
		// vertices:			The data being buffered
		// GL_STATIC_DRAW:		How the data will be used
	// GL_STATIC_DRAW does not allow for the manipulation of the data inside the buffer
	// while GL_DYNAMIC_DRAW does allow manipulation of the buffer data, it can be complecated
	// however.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		// 0:			Location of the attribute pointer (location 0 as specified in the vertex shader)
		// 3:			The amount of data to pass though the attribute pointer (the x, y, and z values of the vertex)
		// GL_FLOAT:	The type of data being passed (float)
		// GL_FALSE:	Is the data normalized? (false)
		// 0:			How much data is skipped between each data chunk used (don't skip any data) (can be used if texture coordinates are included in the buffer)
		// 0:			The offset of where the data will start being read from (start at the start of the buffer)

	glEnableVertexAttribArray(0);	// Enable the attrib array at location 0

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbinds the VBO at the array buffer location
	glBindVertexArray(0);				// Unbinds the VAO

}

void AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType) {
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

void CompileShaders() {
	shader = glCreateProgram();	// Creates a shader program ID
	
	if (!shader) {	// Check if the shader program was created successfully
		printf("Failed to Create Shader Program");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);	// Assigns a vertex shader to the shader program
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);	// Assigns a fragment shader to the shader program

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shader);								// Links the shader program
	glGetProgramiv(shader, GL_LINK_STATUS, &result);	// Gets the result of the linkage process

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Link Program: '%s'\n", errorLog);
		return;
	}

	glValidateProgram(shader);								// Validates the shader program
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);	// Gets the result of the validation process

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(errorLog), NULL, errorLog);
		printf("Failed to Validate Program: '%s'\n", errorLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");	// Gets the location of the uniform variable "model"

}

int main() {
	// Initialise GLFW
	if (!glfwInit()) {	// Check if glfw initialised correctly
		printf("Failed to Initialise GLFW");
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// Major version 3 of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// Minor version 3 of OpenGL
													// Final version 3.3 of OpenGL

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Disallows backwards compatability
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// Allows forwards compatability

	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
		// Creates a new window
		// WIDTH:	width of the new window
		// HEIGHT:	height of the new window
		// "Test Window":	Name of the window
		// NULL:	What monitor to display to
		// NULL:	?

	if (!mainWindow) {	// Check if the window was created correctly
		printf("Window Creation Failed");
		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;										// Store the dimensions of the window buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);	// Gets the dimensions of the buffer
	glfwMakeContextCurrent(mainWindow);									// Set the main window to the current context (window being used)

	glewExperimental = GL_TRUE;	// Enables experimental features

	if (glewInit() != GLEW_OK) {	// Check if glew initialised correctly
		printf("Failed to Initalise GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glViewport(0, 0, bufferWidth, bufferHeight);	// Sets the view port inside the window (not including the window boarder)

	CreateTriangle();	// Creates a simple triangle primitive
	CompileShaders();	// Compiles a shader program with vShader and fShader

	while (!glfwWindowShouldClose(mainWindow)) {
		glfwPollEvents();	// Gets user input events

		if (leftDirection) {
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset) {
			leftDirection = !leftDirection;
		}

		curAngle += angleIncrement;
		if (curAngle >= 360) {
			curAngle - 360;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// Sets the clear color to black
		glClear(GL_COLOR_BUFFER_BIT);			// Clears the screen to the clear color

		glUseProgram(shader);	// Uses the designated shader

		glm::mat4 model(1.0f);												// Creates an identity matrix
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));	// Applies a translation to the model matrix
		model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			//model:						The matrix to opperate on
			//curAngle * toRadians:			The angle, in radians, to rotate the matrix by
			//glm::vec3(0.0f, 0.0f, 1.0f):	The rotation axis of the matrix (z axis)

		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));	//Scales the matrix

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			//uniformModel:				The matrix to pass though
			//1:						How many matrices are being passed (only 1, uniformModel)
			//GL_FALSE:					Should the matrix be transposed (flipped) (false)
			//glm::value_ptr(model):	A pointer to the model matrix

		glBindVertexArray(VAO);	// Uses the triangle VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);
			// GL_TRIANGLES: what primitive is being drawn (triangles)
			// 0: where in the array to start (start at the start of the array)
			// 3: Amount of points to draw (3 points for a triangle)

		glBindVertexArray(0);	// Unbinds the triangle VAO
		glUseProgram(0);		// Unbinds the shader program

		glfwSwapBuffers(mainWindow);	// Swaps the window's current buffer with the buffer the program was working on
	}

	glfwDestroyWindow(mainWindow);	// Destroys the window after use
	glfwTerminate();				// Terminates GLFW after use

	return 0;

}