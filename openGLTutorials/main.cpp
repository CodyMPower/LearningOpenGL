#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <filesystem>

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
#include "PlayerObject.h"
#include "Food.h"
#include "FileMan.h"
#include "FileParser.h"
#include "MatlabHandler.h"
#include "Piping.h"

#define PLAYER_SIZE 1
#define BOARD_SIZE 40

#define NO_MODE 0
#define MATL_MODE 1
#define FILE_MODE 2
#define FPGA_MODE 3
#define MAX_MODE 3

Piping* fpgaPipe;
MatlabHandler* model;
FileMan manager;
FileParser parser;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
std::vector<RenderedObject*> objectVector;
std::vector<Food*> foodVector;
std::vector<std::vector<std::string>> modelOutput;

PlayerObject* player;
Shader directionalShadowShader;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture faceTexture;
Texture cheeseTexture;
Texture mouseTexture;
Texture skyTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLWindow mainWindow;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

int seed = 0;
int functionalMode = 0;
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

void loadOBJ(const std::string &file, std::vector<glm::vec3>& geometry, std::vector<glm::vec2>& texture, std::vector<glm::vec3>& normal)
{
	std::filesystem::path path = std::filesystem::current_path();
	path /= "OBJ_Files";

	manager.setFilePath(path.string());
	manager.setFileName(file);
	manager.setMode(std::fstream::in | std::fstream::out);

	std::vector<std::string> fileData = manager.readFile();
	
	std::vector<std::vector<double>> geometry_temp;
	std::vector<std::vector<double>> texture_temp;
	std::vector<std::vector<double>> normal_temp;

	parser.parseOBJ(fileData, geometry_temp, texture_temp, normal_temp);

	glm::vec3 temp_vec3;
	glm::vec2 temp_vec2;

	for (std::vector<double> data : geometry_temp)
	{
		temp_vec3.x = data.at(0);
		temp_vec3.y = data.at(1);
		temp_vec3.z = data.at(2);
		geometry.push_back(temp_vec3);
	}

	for (std::vector<double> data : normal_temp)
	{
		temp_vec3.x = -data.at(0);
		temp_vec3.y = -data.at(1);
		temp_vec3.z = -data.at(2);
		normal.push_back(temp_vec3);
	}

	for (std::vector<double> data : texture_temp)
	{
		temp_vec2.x = data.at(0);
		temp_vec2.y = 1-data.at(1);
		texture.push_back(temp_vec2);
	}
}

void setSkyboxNormal(std::vector<glm::vec3> &normals, glm::vec3 &dir_light_vec)
{
	for (int i = 0; i < normals.size(); i++)
		normals.at(i) = dir_light_vec;
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
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	std::vector<glm::vec3> geometry;
	std::vector<glm::vec2> texture;
	std::vector<glm::vec3> normal;
	std::string file = "cheese.obj";

	loadOBJ(file, geometry, texture, normal);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(geometry, texture, normal);
	meshList.push_back(obj3);

	geometry.clear();
	texture.clear();
	normal.clear();
	file = "mouse.obj";

	loadOBJ(file, geometry, texture, normal);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(geometry, texture, normal);
	meshList.push_back(obj4);

	//0.0, -1.0f, -1.0f
	geometry.clear();
	texture.clear();
	normal.clear();
	file = "sphere.obj";

	glm::vec3 dir_light = glm::vec3(0.0, -1.0, -1.0);

	loadOBJ(file, geometry, texture, normal);
	setSkyboxNormal(normal, dir_light);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(geometry, texture, normal);
	meshList.push_back(obj5);
}

void CreateScene() {
	//RenderedObject* topTriangle = new RenderedObject(meshList[0], &faceTexture, &dullMaterial);				// Sets the mesh, texture, and material of the object
	//topTriangle->setTransformMatrix(glm::vec3(0.0f, 4.0f, -2.5f), glm::vec3(0.0f, 1.0f, 0.0f),
	//	curAngle * toRadians, glm::vec3(1.0f, 1.0f, 1.0f));								// Sets position, rotation, and size of the object
	//objectVector.push_back(topTriangle);

	//RenderedObject* bottomTriangle = new RenderedObject(meshList[1], &faceTexture, &shinyMaterial);
	//bottomTriangle->setTransformMatrix(glm::vec3(2.0f, 1.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f),
	//	0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	//objectVector.push_back(bottomTriangle);

	RenderedObject* groundPlane = new RenderedObject(meshList[5], &dirtTexture, &shinyMaterial);
	groundPlane->setTransformMatrix(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		0.0f, glm::vec3(1.0707, 1.0, 1.0707));
	objectVector.push_back(groundPlane);

	RenderedObject* playerTriangle = new RenderedObject(meshList[3], &mouseTexture, &dullMaterial);
	playerTriangle->setTransformMatrix(glm::vec3(2.0f, -1.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	player = new PlayerObject(glm::vec3(5.0f, -1.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		playerTriangle);
	objectVector.push_back(playerTriangle);

	Food* food = new Food(meshList[2], &cheeseTexture, &shinyMaterial);
	food->setTransformMatrix(glm::vec3(0.0f, -1.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	food->setSize(1.0f);

	foodVector.push_back(food);
	objectVector.push_back(food);

	RenderedObject* skybox = new RenderedObject(meshList[4], &skyTexture, &shinyMaterial);
	skybox->setTransformMatrix(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		0.0f, glm::vec3(100.0f, 100.0f, 100.0f));
	objectVector.push_back(skybox);
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

	for (int i = 0; i < objectVector.size(); i++) {
		objectVector[i]->renderObject(uniformModel, uniformSpecularIntensity, uniformShininess);	// Render the object with the specified materials to the shader
	}

	//RenderedObject* renderedObject = player->getPlayerObject();
	//renderedObject->renderObject(uniformModel, uniformSpecularIntensity, uniformShininess);

}

void DirectionalShadowMapPass(DirectionalLight* light) {
	
	directionalShadowShader.UseShader();									// Set up the shadow shader to be the next shader used

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(),				// Gets the dimentions of the light source
			light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();											// Sets up light source shadow map to be written to by shader
	glClear(GL_DEPTH_BUFFER_BIT);											// Clears the depth buffer for the new shadow map

	uniformModel = directionalShadowShader.GetModelLocation();				// Sets the uniformModel location to the shadow shader
	glm::mat4 lightTransform = light->CalculateLightTransform();			// Calculates the transform matrix from the light
	directionalShadowShader.SetDirectionalLightTransform(&lightTransform);	// Links the light transform matrix to the shadow shader

	renderScene();															// Render the current sceen with the current objects

	glBindFramebuffer(GL_FRAMEBUFFER, 0);									// Unbinds the frame buffer from the shaders
}

void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	
	shaderList[0]->UseShader();													// Uses the vertex shader from the shader list

	// Stores the IDs of the uniform locations
	uniformModel = shaderList[0]->GetModelLocation();							// Gets the model location in the shader				( uniform mat4 model )
	uniformProjection = shaderList[0]->GetProjectionLocation();					// Gets the progection location in the shader			( uniform mat4 projection )
	uniformView = shaderList[0]->GetViewLocation();								// Gets the view location in the shader					( unifrom mat4 view )
	uniformEyePosition = shaderList[0]->GetEyePositionLocation();				// Gets the eye position location in the shader			(  )
	uniformSpecularIntensity = shaderList[0]->GetSpecularIntensityLocation();	// Gets the specular intesnity location in the shader	(  )
	uniformShininess = shaderList[0]->GetShininessLocation();					// Gets the shininess location in the shader			(  )

	mainWindow.setViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// Sets the clear color to black
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

double genRand(double min, double max)
{
	double val = (double)rand() / RAND_MAX;
	return min + val * (max - min);
}

int checkForCollisions()
{
	glm::vec3 playerPos = player->getPlayerObject()->getPos();
	glm::vec3 foodPos;
	glm::vec3 distance;

	for (Food* currFood : foodVector)
	{
		foodPos = currFood->getPos();
		foodPos.y = -1;
		distance = foodPos - playerPos;

		if (glm::length(distance) <= currFood->getSize() + PLAYER_SIZE)
		{
			int x = genRand(-BOARD_SIZE / 2, BOARD_SIZE / 2);
			int z = genRand(-BOARD_SIZE / 2, BOARD_SIZE / 2);
			currFood->setPosVec(glm::vec3(x, -1.0, z));
			currFood->calculateTransformMatrix();
		}
	}
	return 0;
}

std::vector<bool> getData(std::vector<std::string> recordingData)
{
	std::vector<bool> output;
	output.clear();

	if (recordingData.empty())
		return output;

	for (std::string token : recordingData)
		output.push_back(std::stoi(token) ? true : false);

	return output;
}

std::vector<bool> getStoredData(std::vector<std::vector<std::string>> recording, int time_val)
{
	std::vector<bool> output;
	output.clear();

	if (time_val >= recording.size() - 1)
		return output;

	output = getData(recording.at(time_val + 1));
	return output;
}

#define FPGA_LOAD_INPUT_FORMAT "LD_INPUT,%d,%d\0"
#define FPGA_EXECUTE_FORMAT "RUN_EXECUTION"

bool RWStatus = false;

std::vector<bool> getFPGAData(std::vector<int> input)
{
	char text[1024] = "";
	std::string data;
	std::vector<bool> output;

	if (!RWStatus) {
		for (int i = 0; i < input.size(); i++)
		{
			sprintf_s(text, FPGA_LOAD_INPUT_FORMAT, i, input.at(i));
			data = text;
			fpgaPipe->writeToPipe(data);
		}
		sprintf_s(text, FPGA_EXECUTE_FORMAT);
		data = text;
		fpgaPipe->writeToPipe(data);
		RWStatus = !RWStatus;
	}
	else {
		data = fpgaPipe->readFromPipe();
		if (!data.empty())
		{
			for (int i = 0; i < 3; i++)
				output.push_back(false);

			output.at(0) = (data == "LEFT");
			output.at(1) = (data == "CENTER");
			output.at(1) = (data == "RIGHT");

			if (data == "ERROR")
				output.push_back(true);
		}
	}

	return output;
}

std::vector<bool> getModelOutput(std::vector<int> input, double time_val)
{
	switch (functionalMode)
	{
	case NO_MODE:
		break;
	case MATL_MODE:
		return model->getModelResults(input);
	case FILE_MODE:
		return getStoredData(modelOutput, time_val);
	case FPGA_MODE:
		return getFPGAData(input);
	default:
		return std::vector<bool>();
	}

	return std::vector<bool>();
}

std::vector<std::string> outputToString(std::vector<bool> modelOutput) {
	std::vector<std::string> returnVar;
	returnVar.clear();

	if (modelOutput.empty())
		return returnVar;

	for (bool data : modelOutput)
		returnVar.push_back(std::to_string(data));

	return returnVar;
}

void appendInputToRecords(std::vector<bool> modelData)
{
	if (modelData.empty())
		return;

	if (functionalMode == NO_MODE || functionalMode == FILE_MODE)
		return;

	modelOutput.push_back(outputToString(modelData));
}

void getMode() {
	int modeObtained = 0;
	std::string input;

	while (!functionalMode)
	{
		std::cout << "What Mode Would You Like To Run (MATL_MODE = 1, FILE_MODE = 2, FPGA_MODE = 3):";
		std::cin >> input;
		modeObtained = std::stoi(input);

		if (modeObtained < NO_MODE || modeObtained > MAX_MODE)
			std::cout << "Mode " << input << " Is Not A Valid Input Mode\n";
		else
		{
			functionalMode = modeObtained;
			return;
		}
	}
}

void fileSetup()
{
	bool finished = false;
	std::string file;
	std::vector<std::string> fileData;

	while (!finished)
	{
		std::cout << "What Recording File Would You Like To Read From: ";
		std::cin >> file;

		manager.setFileName(file);
		manager.setMode(std::fstream::in | std::fstream::out);
		fileData = manager.readFile();

		if (fileData.empty())
			std::cout << "File " << file << " Is Empty, Please Choose Another File\n";
		else
		{
			modelOutput = parser.parseCSV(fileData);
			seed = std::stoi(modelOutput.at(0).at(0));
			std::cout << seed << "\n";
			srand(seed);
			finished = true;
		}
	}
}

void fileSave()
{
	if (functionalMode == NO_MODE || functionalMode == FILE_MODE)
		return;
	
	std::string input;
	std::cout << "What Would You Like The Recording To Be Called (\"no file\" to not save): ";
	std::cin >> input;

	std::transform(input.begin(), input.end(), input.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (input.at(0) == '\n' || input.compare("no file") == 0)
		return;

	manager.setFileName(input);
	std::vector<std::string> fileData = parser.formatCSV(modelOutput);
	manager.setMode(std::fstream::in | std::fstream::out);
	manager.writeFile(fileData);
}

void matlabSetup()
{
	model = new MatlabHandler();

	seed = rand();
	srand(seed);

	std::string seed_str = std::to_string(seed);
	std::vector<std::string> line;
	line.push_back(seed_str);
	modelOutput.push_back(line);
}

void modeSetup()
{
	switch (functionalMode)
	{
	case NO_MODE:
		return;
	case MATL_MODE:
		matlabSetup();
		return;
	case FILE_MODE:
		fileSetup();
		return;
	case FPGA_MODE:
		fpgaPipe = new Piping();
		return;
	default:
		return;
	}
}

int main() {
	std::filesystem::path path = std::filesystem::current_path();
	path /= "CSV_Files";
	manager.setFilePath(path.string());

	std::cout << path.string() << "\n";

	getMode();
	modeSetup();

	// Creates a window and checks if the window was initalizes correctly
	mainWindow = GLWindow(1366, 768);
	if (mainWindow.Initialise() != 0) {
		return 1;
	}

	// Create objects and store the mesh pointers in meshList
	CreateObjects();				// Creates a simple shape with 4 triangles
	CreateShader();					// Compiles a shader program with vShader and fShader
	CreatePlane(40, 40);	// Creates a plane object of width and height of 10
	//CreateObject();

	camera = Camera(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -45.0f, 5.0f, 0.5f);	// Sets up camera properties

	// Create and load textures from designated directory
	brickTexture = Texture((char*)"Textures/brick.jpg");
	brickTexture.LoadTextureA();
	dirtTexture = Texture((char*)"Textures/dirt.jpg");
	dirtTexture.LoadTextureA();
	faceTexture = Texture((char*)"Textures/Face.jpg");
	faceTexture.LoadTextureA();
	cheeseTexture = Texture((char*)"Textures/cheese.jpg");
	cheeseTexture.LoadTextureA();
	mouseTexture = Texture((char*)"Textures/mouse.jpg");
	mouseTexture.LoadTextureA();
	skyTexture = Texture((char*)"Textures/sky.jpg");
	skyTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 256);	// Full intensity, higher powers of 2 indicate more shininess
	dullMaterial = Material(0.3f, 4);	// Low intensity, lower powers of 2 indicates less shininess

								// Shadow Buffer size
	mainLight = DirectionalLight(1024, 1024,
		// rgb values
		1.0f, 1.0f, 1.0f,
		//amb, dif
		0.7f, 0.3f,
		//x, y	  , z
		0.0, -1.0f, -1.0f);

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



	if (functionalMode != NO_MODE) {
		spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
			0.0f, 2.0f,
			0.0f, 0.0f, 0.0f,
			-100.0f, -1.0f, 0.0f,
			0.3f, 0.2f, 0.1f,
			20.0f);
		spotLightCount++;
	}

	// Creates a projection (perspective) matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f); // Only really need to create it once, unless you're manipulating the projection every frame

	CreateScene();	// Sets the scene up to render

	glm::vec3 playerPos;
	glm::vec2 v1 = glm::normalize(glm::vec2(1.0f, 0.0f));
	glm::vec2 v2 = glm::vec2(-v1.y, v1.x);
	glm::vec2 p1 = glm::vec2(0.0f, 0.0f);
	glm::vec2 p2 = glm::vec2(5.0f, 0.0f);
	double radius = 1.0f;
	double originalHeight = objectVector.at(2)->getPos().y;

	const int max_vision = 23;
	glm::vec2 food_arr[1];
	bool vision_arr[max_vision];

	std::vector<int> input;
	input.clear();

	for (int i = 0; i < max_vision; i++)
		input.push_back(0);

	std::vector<bool> output;

	int time = 0;

	while (!mainWindow.getShouldClose()) {

		GLfloat now = glfwGetTime();	// (SDL_GetPerformanceCounter(), not returned in seconds)	Gets the current time
		deltaTime = now - lastTime;		// ((now - lastTime) * 1000 / SDL_GetPerformanveFrequency)	Gets the change in ime from last itteration to current
		lastTime = now;					// Sets last time to current time for next loop itteration

		food_arr[0] = glm::vec2(foodVector.at(0)->getPos().x, foodVector.at(0)->getPos().z);
		for (int i = 0; i < max_vision; i++)
		{
			vision_arr[i] = false;
		}

		player->playerVision(vision_arr, max_vision, food_arr, 1, 1.0f, 1.0f);
		for (int i = 0; i < max_vision; i++)
			input.at(i) = vision_arr[i] * 15;

		output = getModelOutput(input, time);

		if (!output.empty())
		{
			if (output.size() == 4)
			{
				std::cout << "Error in model, ending simulation\n";
				break;
			}

			printf("Model Inputs:  ");
			for (int i = 0; i < max_vision; i++)
			{
				input.at(i) = vision_arr[i] * 15;
				if (vision_arr[i])
					printf("\033[0;32m%d\033[0;37m", vision_arr[i]);
				else
					printf("%d", vision_arr[i]);
			}
			printf("\n");
			printf("Model Results: ");
			for (int i = 0; i < 3; i++)
			{
				if (output.at(i))
					printf("\033[0;32m%d\033[0;37m", 1);
				else
					printf("%d", 0);
			}
			printf("\n");
		}

		glfwPollEvents();	// Gets user input events
		
		camera.keyControl(mainWindow.getKeys(), deltaTime, functionalMode == NO_MODE);						// Updates camera location based on keyboard input
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());	// Updates camera rotations based on changes in cursor locations

		if (!output.empty() && functionalMode != NO_MODE)
			player->modelCondrol(output);
		else if (functionalMode == NO_MODE)
		{
			player->keyControl(mainWindow.getKeys());
		}
			
		player->updatePlayer(0.03333);//15 steps / second sim time
		checkForCollisions();

		if (functionalMode == NO_MODE)
		{
			playerPos = player->getPos();
			playerPos.y += 2;
			playerPos += (camera.getCameraDirection() * 2.0f);

			player->setRotAng((camera.getCameraYaw() - 270) * (-M_PI / 180));
			camera.setCameraPosition(playerPos);
		}

		objectVector.at(3)->setPosVec(camera.getCameraPosition());
		objectVector.at(3)->setRotAng(objectVector.at(3)->getRotAng() + deltaTime * 0.01);

		glm::vec3 offsetPos = objectVector.at(2)->getPos();
		offsetPos.y = originalHeight + sin(now) * 0.2;
		objectVector.at(2)->setPosVec(offsetPos);
		objectVector.at(2)->setRotAng(objectVector.at(2)->getRotAng() + deltaTime * 0.3);
		
		DirectionalShadowMapPass(&mainLight);
		renderPass(projection, camera.calculateViewMatrix());

		glUseProgram(0);							// Unbinds the shader program
		//
		mainWindow.swapBuffers();	// Swaps the window's current buffer with the buffer the program was working on
		time++;

		appendInputToRecords(output);
	}

	std::cout << modelOutput.size() << "\n";
	mainWindow.~GLWindow();	// Destroys the window after use
	fileSave();

	if (model != nullptr)
		model->~MatlabHandler();

	if (fpgaPipe != nullptr)
		fpgaPipe->~Piping();

	return 0;

}
