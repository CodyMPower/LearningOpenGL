#pragma once

#include <GL\glew.h>
#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
	Mesh();

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndecies);	// Creates a mesh
	void CreateMesh(std::vector<glm::vec3>& geometry, std::vector<glm::vec2>& texture, std::vector<glm::vec3>& normal);
	void RenderMesh();	// Renders the mesh
	void ClearMesh();	// Clears the VAO, VBO, and IBO values

	~Mesh();

private:
	GLuint VAO, VBO, IBO, geom_buff, texture_buff, normal_buff, vertex_count;	// Mesh Buffers
	GLsizei indexCount;		// Size integer, the second variable needed in the glDrawElements function
};

