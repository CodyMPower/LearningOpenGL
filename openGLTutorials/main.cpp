#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "GLWindow.h"

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

GLWindow mainWindow;

bool leftDirection = true;		// Is the triangle moving left?
float triOffset = 0.0f;			// The current offset
float triMaxOffset = 0.7f;		// The maximum offset of the triangle
float triIncrement = 0.001f;	// The increment value to the offset

float toRadians = 3.14159265f / 180.0f;
float curAngle = 0.0f;
float angleIncrement = 0.1f;

/*	Vertex Shader
	gl_Position is a predefined out vec4 variable
	model is where the object vertex is located in the world	(Local Space -> World Space)
	view is where the object is relative to the camera			(World Space -> View Space)
	projection is the frustrum of view that the camera will see	(View Space -> Clip space)
	vCol is the color based on the position of the vertex
*/
static const char* vShader = "Shaders/shader.vert";

/*	Fragment Shader
	any out vec4 will be assumed to be the colour of the pixel on the screen
	vCol is interpolated based on the location of the fragment shader within the triangle
*/
static const char* fShader = "Shaders/shader.frag";

void CreateObject() {
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,	// Point 0
		0.0f , -1.0f, 1.0f,	// Point 1
		1.0f , -1.0f, 0.0f,	// Point 2
		0.0f , 1.0f , 0.0f	// Point 3
	};

	unsigned int indices[] = {
		0, 3, 1,	// 
		1, 3, 2,	// 
		2, 3, 0,	// Front Facing Triangle (Original)
		0, 1, 2		// 
	};
	
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);	// Adds a new element to the end of the vector

}

void CreateShader() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFile(vShader, fShader);
	shaderList.push_back(shader1);
}


int main() {

	mainWindow = GLWindow(800, 600);
	if (mainWindow.Initialise() != 0) {
		return 1;
	}

	CreateObject();		// Creates a simple shape with 4 triangles
	CreateShader();	// Compiles a shader program with vShader and fShader

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)(mainWindow.getBufferWidth()/mainWindow.getBufferHeight()), 0.1f, 100.0f); // Only really need to create it once, unless you're manipulating the projection every frame

	while (!mainWindow.getShouldClose()) {
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
		if (curAngle >= 360.0f) {
			curAngle -= 360.0f;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Sets the clear color to black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clears the screen color and depth, color cleared to specified clear color

		shaderList[0]->UseShader();

		glm::mat4 model(1.0f);												// Creates an identity matrix
		model = glm::translate(model, glm::vec3(0.0f, triOffset, -2.5f));	// Applies a translation to the model matrix
		model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model:				The matrix to opperate on
		//curAngle * toRadians:	The angle, in radians, to rotate the matrix by
		//glm::vec3():			The rotation axis of the matrix (z axis)

		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));	//Scales the matrix

		GLuint uniformModel = 0, uniformProjection = 0;
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			//uniformModel:				The matrix to pass though
			//1:						How many matrices are being passed (only 1, uniformModel)
			//GL_FALSE:					Should the matrix be transposed (flipped) (false)
			//glm::value_ptr(model):	A pointer to the model matrix
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		meshList[0]->RenderMesh();	// Calls the render function of the mesh at the first index

		model = glm::mat4(1.0f);												// Creates an identity matrix
		model = glm::translate(model, glm::vec3(0.0f, -triOffset, -2.5f));	// Applies a translation to the model matrix
		model = glm::rotate(model, -curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model:				The matrix to opperate on
		//curAngle * toRadians:	The angle, in radians, to rotate the matrix by
		//glm::vec3():			The rotation axis of the matrix (z axis)

		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));	//Scales the matrix

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		meshList[0]->RenderMesh();	// Calls the render function of the mesh at the first index

		glUseProgram(0);							// Unbinds the shader program

		mainWindow.swapBuffers();	// Swaps the window's current buffer with the buffer the program was working on
	}

	mainWindow.~GLWindow();	// Destroys the window after use

	return 0;

}