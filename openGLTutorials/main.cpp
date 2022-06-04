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

#include "CommonVals.h"

#include "Mesh.h"
#include "Shader.h"
#include "GLWindow.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "RenderedObject.h"

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Shader directionalShadowShader;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

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

unsigned int spotLightCount = 0;
unsigned int pointLightCount = 0;

// Creates variables for uniform locations
GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0;	// Uniform IDs for the vertex shader

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

/*
Calculates the normals for each vertex

indices: a pointer to an indices array
indCount: how many indices are there?
vertices: a pointer to a vertices array
vertCount: how many vertice values are there total? (len of the array)
vLength: the size of each vertex information packet (how many coordinate values, tex coordinates, normal coordinates ect. values per vertex)
normalOffset: how far from the start of an information packet is the normal information stored?
*/
void calcAverageNormals(unsigned int* indices, unsigned int indCount, GLfloat *vertices, unsigned int vertCount, 
						unsigned int vLength, unsigned int normalOffset) {
	for (size_t i = 0; i < indCount; i += 3) {
		unsigned int ind0 = indices[i]	   * vLength;
		unsigned int ind1 = indices[i + 1] * vLength;
		unsigned int ind2 = indices[i + 2] * vLength;

		glm::vec3 v1(vertices[ind1] - vertices[ind0], vertices[ind1 + 1] - vertices[ind0 + 1], vertices[ind1 + 2] - vertices[ind0 + 2]);
		glm::vec3 v2(vertices[ind2] - vertices[ind0], vertices[ind2 + 1] - vertices[ind0 + 1], vertices[ind2 + 2] - vertices[ind0 + 2]);
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));	// Normal of the triangle

		vertices[ind0 + normalOffset + 0] += normal.x;
		vertices[ind1 + normalOffset + 0] += normal.x;
		vertices[ind2 + normalOffset + 0] += normal.x;

		vertices[ind0 + normalOffset + 1] += normal.y;
		vertices[ind1 + normalOffset + 1] += normal.y;
		vertices[ind2 + normalOffset + 1] += normal.y;

		vertices[ind0 + normalOffset + 2] += normal.z;
		vertices[ind1 + normalOffset + 2] += normal.z;
		vertices[ind2 + normalOffset + 2] += normal.z;
	}

	for (size_t i = 0; i < vertCount; i += vLength) {
		unsigned int relOffset = i + normalOffset;

		glm::vec3  vec(vertices[relOffset], vertices[relOffset + 1], vertices[relOffset + 2]);
		vec = glm::normalize(vec);

		vertices[relOffset + 0] = vec.x;
		vertices[relOffset + 1] = vec.y;
		vertices[relOffset + 2] = vec.z;

	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShader() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFile(vShader, fShader);
	shaderList.push_back(shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFile("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
}

void CreatePlane(int rows, int cols) {
	if (rows < 2 || cols < 2) {	// Does not allow the use of less than 4 vertecies (2x2)
		return;
	}

	int vertInfoSize = 8;

	// Creates offsets to center the plane relative to (0, 0)
	GLfloat xOff = (float) ((rows - 1) / 2);
	xOff += (rows % 2 == 0) ? 0.5f: 0.0f;
	GLfloat yOff = (float)((cols - 1) / 2);
	yOff += (cols % 2 == 0) ? 0.5f : 0.0f;

	// The size of the vertex and index buffers taking into account the 2 tex coords per 3 vert coords
	const int vertSize = rows * cols * vertInfoSize;
	const int indSize = (cols - 1) * (rows - 1) * 6;

	// Creates pointers to arrays of the above sizes
	GLfloat *vertices = new GLfloat[vertSize];
	unsigned int *indices = new unsigned int[indSize];
	
	// Calculates the coordinate positions of the vertex coordinates and sets tex coordinates to current relative vert position
	for (int X = 0; X < rows; X++) {

		for (int P = 0; P < cols; P++) {
			int offset = ((X * cols) + P) * vertInfoSize;
			vertices[offset + 0] = (GLfloat) (P - xOff);	// X coord
			vertices[offset + 1] = (GLfloat) (0.0f);	// Y coord
			vertices[offset + 2] = (GLfloat) (X - yOff);		// Z coord, placed at origin
			vertices[offset + 3] = (GLfloat) (P);			// S tex coord
			vertices[offset + 4] = (GLfloat) (X);			// T tex coord
			vertices[offset + 5] = (GLfloat) 0.0f;
			vertices[offset + 6] = (GLfloat) -1.0f;
			vertices[offset + 7] = (GLfloat) 0.0f;
		}

	}

	// Calculates the indice order to draw each triangle in the proper position and orientation
	for (int X = 0; X < rows - 1; X++) {

		for (int p = 0; p < cols - 1; p++) {											// One Square example:
			int newSquare[6];							// Temp array for storing data
			newSquare[0] = (X * cols) + p;				// Bottom left corner of triangle 1		(0)
			newSquare[1] = ((X + 1) * cols) + p;		// Bottom right corner of triangle 1	(1)
			newSquare[2] = (X * cols) + (p + 1);		// Top corner of triangle 1				(2)
			newSquare[3] = (X * cols) + (p + 1);		// Bottom corner of triangle 2			(1)
			newSquare[4] = ((X + 1) * cols) + p;		// Top right corner of triangle 2		(3)
			newSquare[5] = ((X + 1) * cols) + (p + 1);	// Top left corner of triangle 2		(2)

			for (int i = 0; i < 6; i++) {
				indices[((X * (cols - 1)) + p) * 6 + i] = newSquare[i];	// Transfers the data to the indices array
			}
			
		}

	}

	//calcAverageNormals(indices, indSize, vertices, vertSize, vertInfoSize, 5);

	// Create a mesh and store the mesh pointer to the mesh list
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, vertSize, indSize);
	meshList.push_back(obj1);	// Adds a new element to the end of the vector

}

void renderScene() {

	RenderedObject topTriangle(meshList[0], &brickTexture, &dullMaterial);
	topTriangle.setTransformMatrix(glm::vec3(0.0f, 4.0f, -2.5f), glm::vec3(0.0f, 1.0f, 0.0f),
		curAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));
	topTriangle.renderObject(uniformModel, uniformSpecularIntensity, uniformShininess);
	
	// Creates a model matrix
	glm::mat4 model(1.0f);											// Creates an identity matrix
	//model = topTriangle.getTransformMatrix();
	//model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));	// Applies a translation to the model matrix
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model:				The matrix to opperate on
	//curAngle * toRadians:	The angle, in radians, to rotate the matrix by
	//glm::vec3():			The rotation axis of the matrix (z axis)
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	//Scales the matrix

	// Assigns the matrices to their respective uniform variables
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	//uniformModel:				The matrix to pass though
	//1:						How many matrices are being passed (only 1, uniformModel)
	//GL_FALSE:					Should the matrix be transposed (flipped) (false)
	//glm::value_ptr(model):	A pointer to the model matrix

	brickTexture.UseTexture();	// Use texture you want before Rendering each mesh
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	//meshList[0]->RenderMesh();	// Calls the render function of the mesh at the first index

	model = glm::mat4(1.0f);												// Creates an identity matrix
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));	// Applies a translation to the model matrix
	//model = glm::rotate(model, -curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model:				The matrix to opperate on
	//curAngle * toRadians:	The angle, in radians, to rotate the matrix by
	//glm::vec3():			The rotation axis of the matrix (z axis)
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));	//Scales the matrix
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	dirtTexture.UseTexture();	// Use texture you want before Rendering each mesh
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();	// Calls the render function of the mesh at the first index

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0, 1.0, 2.0));
	//model = glm::rotate(model, -curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	meshList[2]->RenderMesh();
}

void DirectionalShadowMapPass(DirectionalLight* light) {
	
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	glm::mat4 lightTransform = light->CalculateLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

	renderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	// Uses the first shader in the shader list
	shaderList[0]->UseShader();

	// Stores the IDs of the uniform locations
	uniformModel = shaderList[0]->GetModelLocation();
	uniformProjection = shaderList[0]->GetProjectionLocation();
	uniformView = shaderList[0]->GetViewLocation();
	uniformEyePosition = shaderList[0]->GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0]->GetSpecularIntensityLocation();
	uniformShininess = shaderList[0]->GetShininessLocation();

	mainWindow.setViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Sets the clear color to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clears the screen color and depth, color cleared to specified clear color

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0]->SetDirectionalLight(&mainLight);
	shaderList[0]->SetPointLights(pointLights, pointLightCount);
	shaderList[0]->SetSpotLights(spotLights, spotLightCount);

	glm::mat4 lightTransform = mainLight.CalculateLightTransform();
	shaderList[0]->SetDirectionalLightTransform(&lightTransform);

	mainLight.GetShadowMap()->Read(GL_TEXTURE1);	// GL_TEXTURE0 is already being used for the texture of any given object
	shaderList[0]->SetTexture(0);					// Using Texture unit 0 for textures
	shaderList[0]->SetDirectionalShadowMap(1);		// Using Texture unit 1 for shadow maps

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y = -0.3f;
	spotLights[0].SetFlash(camera.getCameraPosition(), camera.getCameraDirection());

	renderScene();
	
}

int main() {

	// Creates a window and checks if the window was initalizes correctly
	mainWindow = GLWindow(1366, 768);
	if (mainWindow.Initialise() != 0) {
		return 1;
	}

	// Create objects and store the mesh pointers in meshList
	CreateObjects();	// Creates a simple shape with 4 triangles
	CreateShader();		// Compiles a shader program with vShader and fShader
	CreatePlane(10, 10);
	//CreateObject();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);	// Sets up camera properties

	// Create and load textures from designated directory
	brickTexture = Texture((char*) "Textures/brick.jpg");
	brickTexture.LoadTextureA();
	dirtTexture = Texture((char*) "Textures/dirt.jpg");
	dirtTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 256);	// Full intensity, higher powers of 2 indicate more shininess
	dullMaterial = Material(0.3f, 4);	// Low intensity, lower powers of 2 indicates less shininess

							    // Shadow Buffer size
	mainLight = DirectionalLight(1024, 1024,
								// rgb values
								1.0f, 1.0f, 1.0f, 
								//amb, dif
								0.1f, 0.3f,
								//x, y	  , z
								0.0, -7.0f, -1.0f);

	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
								0.0f, 0.0f,
								-4.0f, 0.0f, 0.0f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,
								0.0f, 0.0f,
								4.0f, 0.0f, 0.0f,
								0.3f, 0.1f, 0.1f);
	pointLightCount++;


	
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
								0.0f, 2.0f,
								0.0f, 0.0f, 0.0f,
								-100.0f, -1.0f, 0.0f,
								0.3f, 0.2f, 0.1f,
								20.0f);
	spotLightCount++;

	// Creates a projection (perspective) matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth()/mainWindow.getBufferHeight(), 0.1f, 100.0f); // Only really need to create it once, unless you're manipulating the projection every frame

	while (!mainWindow.getShouldClose()) {

		GLfloat now = glfwGetTime();	// (SDL_GetPerformanceCounter(), not returned in seconds)	Gets the current time
		deltaTime = now - lastTime;		// ((now - lastTime) * 1000 / SDL_GetPerformanveFrequency)	Gets the change in ime from last itteration to current
		lastTime = now;					// Sets last time to current time for next loop itteration

		glfwPollEvents();	// Gets user input events
		camera.keyControl(mainWindow.getKeys(), deltaTime);						// Updates camera location based on keyboard input
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());	// Updates camera rotations based on changes in cursor locations
		
		DirectionalShadowMapPass(&mainLight);
		renderPass(projection, camera.calculateViewMatrix());

		glUseProgram(0);							// Unbinds the shader program

		mainWindow.swapBuffers();	// Swaps the window's current buffer with the buffer the program was working on
	}

	mainWindow.~GLWindow();	// Destroys the window after use

	return 0;

}
