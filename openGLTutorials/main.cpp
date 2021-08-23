#define STB_IMAGE_IMPLEMENTATION

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
#include "Camera.h"
#include "Texture.h"

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

GLWindow mainWindow;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

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
		0.0f , 0.0f ,	// Tex Coord 0

		0.0f , -1.0f, 1.0f,	// Point 1
		0.5f , 0.0f ,	// Tex Coord 1

		1.0f , -1.0f, 0.0f,	// Point 2
		1.0f , 0.0f ,	// Tex Coord 2

		0.0f , 1.0f , 0.0f,	// Point 3
		0.5f , 1.0f		// Tex Coord 3
	};

	unsigned int indices[] = {
		0, 3, 1,	// 
		1, 3, 2,	// 
		2, 3, 0,	// Front Facing Triangle (Original)
		0, 1, 2		// 
	};
	
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 20, 12);
	meshList.push_back(obj1);	// Adds a new element to the end of the vector

}

void CreateShader() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFile(vShader, fShader);
	shaderList.push_back(shader1);
}

/*
void CreatePlane(int rows, int cols) {
	if (rows < 2 || cols < 2) {
		return;
	}

	GLfloat xOff = ((float)rows) / 2;
	GLfloat yOff = ((float)cols) / 2;

	const int vertSize = rows * cols * 3;
	const int indSize = (cols - 1) * (rows - 1) * 6;

	GLfloat vertices[vertSize];
	unsigned int indices[indSize];

	for (GLint X = 0; X < rows; X++) {

		for (GLint P = 0; P < cols; P++) {
			vertices[((X * cols) + P) * 3]		= (GLfloat) (P - xOff);
			vertices[((X * cols) + P) * 3 + 1]	= (GLfloat) (X - yOff);
			vertices[((X * cols) + P) * 3 + 2]	= (GLfloat) (-1.0f);
		}

	}

	for (int X = 0; X < rows - 1; X++) {

		for (int P = 0; P < cols - 1; P++) {
			int newSquare[6];
			newSquare[0] = X * cols + P;
			newSquare[1] = X * cols + (P + 1);
			newSquare[2] = (X + 1) * cols + P;
			newSquare[3] = X * cols + (P + 1);
			newSquare[4] = (X + 1) * cols + (P + 1);
			newSquare[5] = (X + 1) * cols + P;

			for (int i = 0; i < 6; i++) {
				indices[((X * cols - 1) + P) * 6 + i] = newSquare[i];
			}

		}

	}

	printf("Vertices values: Size: %d\n", vertSize);

	for (int X = 0; X < rows; X++) {
		printf("Row: %d\n", X);

		for (int P = 0; P < cols; P++) {
			printf("X coord: '%.6f' " , (float) vertices[((X * cols) + P) * 3]);
			printf("Y coord: '%.6f' " , (float) vertices[((X * cols) + P) * 3 + 1]);
			printf("Z coord: '%.6f'\n", (float) vertices[((X * cols) + P) * 3 + 2]);
		}

	}

	printf("\n");
	printf("Indices values: Size: %d\n", indSize);

	for (int X = 0; X < rows - 1; X++) {
		printf("Row: %d\n", X);

		for (int P = 0; P < cols - 1; P++) {
			printf("X coord: '%u' " , indices[((X * cols - 1) + P) * 6]);
			printf("Y coord: '%u' " , indices[((X * cols - 1) + P) * 6 + 1]);
			printf("Z coord: '%u' " , indices[((X * cols - 1) + P) * 6 + 2]);
			printf("Z coord: '%u' " , indices[((X * cols - 1) + P) * 6 + 3]);
			printf("Z coord: '%u' " , indices[((X * cols - 1) + P) * 6 + 4]);
			printf("Z coord: '%u'\n", indices[((X * cols - 1) + P) * 6 + 5]);
		}

	}

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, vertSize, indSize);
	meshList.push_back(obj1);	// Adds a new element to the end of the vector

}

*/

int main() {

	mainWindow = GLWindow(800, 600);
	if (mainWindow.Initialise() != 0) {
		return 1;
	}

	CreateObject();	// Creates a simple shape with 4 triangles
	CreateShader();	// Compiles a shader program with vShader and fShader
	//CreatePlane(5, 5);
	CreateObject();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);	// Sets up camera properties

	brickTexture = Texture((char*) "Textures/brick.jpg");
	brickTexture.LoadTexture();
	dirtTexture = Texture((char*) "Textures/dirt.jpg");
	dirtTexture.LoadTexture();

	GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;	// Uniform IDs for the vertex shader
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)(mainWindow.getBufferWidth()/mainWindow.getBufferHeight()), 0.1f, 100.0f); // Only really need to create it once, unless you're manipulating the projection every frame

	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime();	// (SDL_GetPerformanceCounter(), not returned in seconds)	Gets the current time
		deltaTime = now - lastTime;		// ((now - lastTime) * 1000 / SDL_GetPerformanveFrequency)	Gets the change in ime from last itteration to current
		lastTime = now;					// Sets last time to current time for next loop itteration

		glfwPollEvents();	// Gets user input events
		camera.keyControl(mainWindow.getKeys(), deltaTime);						// Updates camera location based on keyboard input
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());	// Updates camera rotations based on changes in cursor locations

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
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.5f));	// Applies a translation to the model matrix
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model:				The matrix to opperate on
		//curAngle * toRadians:	The angle, in radians, to rotate the matrix by
		//glm::vec3():			The rotation axis of the matrix (z axis)
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));	//Scales the matrix

		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();
		uniformView = shaderList[0]->GetViewLocation();

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			//uniformModel:				The matrix to pass though
			//1:						How many matrices are being passed (only 1, uniformModel)
			//GL_FALSE:					Should the matrix be transposed (flipped) (false)
			//glm::value_ptr(model):	A pointer to the model matrix
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		brickTexture.UseTexture();	// Use texture you want before Rendering each mesh
		meshList[0]->RenderMesh();	// Calls the render function of the mesh at the first index

		model = glm::mat4(1.0f);												// Creates an identity matrix
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -2.5f));	// Applies a translation to the model matrix
		//model = glm::rotate(model, -curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model:				The matrix to opperate on
		//curAngle * toRadians:	The angle, in radians, to rotate the matrix by
		//glm::vec3():			The rotation axis of the matrix (z axis)
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));	//Scales the matrix
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		dirtTexture.UseTexture();	// Use texture you want before Rendering each mesh
		meshList[0]->RenderMesh();	// Calls the render function of the mesh at the first index

		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//meshList[1]->RenderMesh();

		glUseProgram(0);							// Unbinds the shader program

		mainWindow.swapBuffers();	// Swaps the window's current buffer with the buffer the program was working on
	}

	mainWindow.~GLWindow();	// Destroys the window after use

	return 0;

}