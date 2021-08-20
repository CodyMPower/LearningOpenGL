#pragma once

#include <GL\glew.h>

class Mesh
{
public:
	Mesh();

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndecies);
	void RenderMesh();
	void ClearMesh();

	~Mesh();

private:
	GLuint VAO, VBO, IBO;	// Mesh Buffers
	GLsizei indexCount;		// Size integer, the second variable needed in the glDrawElements function
};

