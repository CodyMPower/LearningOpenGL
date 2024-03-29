#include "Mesh.h"

Mesh::Mesh() {
	VAO = 0;		// Set All buffers to the ID 0
	VBO = 0;		
	IBO = 0;		
	geom_buff = 0;
	normal_buff = 0;
	texture_buff = 0;
	vertex_count = 0;
	indexCount = 0;	
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndecies) {
	glGenVertexArrays(1, &VAO);	// Creates 1 VAO and returns the VAO ID
	glBindVertexArray(VAO);		// Binds the VAO to bind VBOs

	glGenBuffers(1, &VBO);				// Creates 1 VBO and returns the VBO ID
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Binds a VBO to the target buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);
	// buffers the data to the target VBO
	// GL_ARRAY_BUFFER:		The target to bind the data to
	// sizeof(vertices):	The size of the data being buffered
	// vertices:			The data being buffered
	// GL_STATIC_DRAW:		How the data will be used
// GL_STATIC_DRAW does not allow for the manipulation of the data inside the buffer
// while GL_DYNAMIC_DRAW does allow manipulation of the buffer data, it can be complecated
// however.

	glGenBuffers(1, &IBO);																				// Generates a buffer for the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);															// Binds the IBO to the element array buffer inside the VAO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndecies, indices, GL_STATIC_DRAW);	// Buffers the data of the indices order to the IBO's buffer

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
	// 0:						Location of the attribute pointer (location 0 as specified in the vertex shader)
	// 3:						The amount of data to pass though the attribute pointer (the x, y, and z values of the vertex)
	// GL_FLOAT:				The type of data being passed (float)
	// GL_FALSE:				Is the data normalized? (false)
	// sizeof(vertices[0]) * 8:	whe size of 1 chunk of data (skip 2 texture values and 3 normal values after the 3 coord values) (can be used if texture coordinates are included in the buffer)
	// 0:						The offset of where the data will start being read from (start at the start of the buffer)
	glEnableVertexAttribArray(0);																				// Enable the attrib array at location 0
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));	// Creates attribute pointer for location 1, texture attrib
	glEnableVertexAttribArray(1);																				// Enable attrib pointer 1
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));	// Creates attribute pointer for location 2, normal attrib
	glEnableVertexAttribArray(2);																				// Enable attrib pointer 2

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbinds the VBO at the array buffer location
	glBindVertexArray(0);				// Unbinds the VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// Unbind IBO after unbinding VAO
	// The IBO is unbound after the VAO as the VAO can only contain 1 IBO, thus unbinding beforehand will result in the removal of the IBO
	// The VBO is unbound before the VAO as the VAO can contain multiple VBOs, thus unbinding th VBO beforehand will not result in it's removal from the VAO

	indexCount = numOfIndecies;

}

void Mesh::CreateMesh(std::vector<glm::vec3>& geometry, std::vector<glm::vec2>& texture, std::vector<glm::vec3>& normal)
{
	glGenVertexArrays(1, &VAO);	// Creates 1 VAO and returns the VAO ID
	glBindVertexArray(VAO);		// Binds the VAO to bind VBOs

	glGenBuffers(1, &geom_buff);				// Creates 1 VBO and returns the VBO ID
	glBindBuffer(GL_ARRAY_BUFFER, geom_buff);	// Binds a VBO to the target buffer
	
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, geometry.size() * sizeof(glm::vec3), &geometry[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glGenBuffers(1, &texture_buff);				// Creates 1 VBO and returns the VBO ID
	glBindBuffer(GL_ARRAY_BUFFER, texture_buff);	// Binds a VBO to the target buffer

	glEnableVertexAttribArray(1);
	glBufferData(GL_ARRAY_BUFFER, texture.size() * sizeof(glm::vec2), &texture[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glGenBuffers(1, &normal_buff);				// Creates 1 VBO and returns the VBO ID
	glBindBuffer(GL_ARRAY_BUFFER, normal_buff);	// Binds a VBO to the target buffer

	glEnableVertexAttribArray(2);
	glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(glm::vec3), &normal[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbinds the VBO at the array buffer location
	glBindVertexArray(0);				// Unbinds the VAO

	vertex_count = geometry.size() * 3;
}

void Mesh::RenderMesh() {
	glBindVertexArray(VAO);						// Uses the triangle VAO

	if (IBO != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);	// Binds the active IBO

		// glDrawArrays(GL_TRIANGLES, 0, 3);	//Only use if not binding IBO
				// GL_TRIANGLES: what primitive is being drawn (triangles)
				// 0: where in the array to start (start at the start of the array)
				// 3: Amount of points to draw (3 points for a triangle)



		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);	// Draws the elements based on the IBO
			// GL_TRIANGLES:	What primitives are being drawn (triangles)
			// indexCount:		How many elements are there to draw (num of indices)
			// GL_UNSIGNED_INT:	The format of the data being used (can't have half or negative amounts of verteces, so uint)
			// 0:				The indices data (Already bound in the previous line, so no need to specify here)
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	}

	glBindVertexArray(0);						// Unbinds the triangle VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	// Unbinds the IBO, remember, unbind IBOs after their respective VAOs
}

void Mesh::ClearMesh() {
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);	// Deletes the buffered data from the GPU, no garbage collection, so must be cleared up
		IBO = 0;					// Removes buffer ID as the buffer has been deleted
	}

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);	// Deletes the buffered data from the GPU, no garbage collection, so must be cleared up
		VBO = 0;					// Removes buffer ID as the buffer has been deleted
	}

	if (geom_buff != 0) {
		glDeleteBuffers(1, &geom_buff);
		geom_buff = 0;
	}

	if (texture_buff != 0) {
		glDeleteBuffers(1, &texture_buff);
		texture_buff = 0;
	}

	if (normal_buff != 0) {
		glDeleteBuffers(1, &normal_buff);
		normal_buff = 0;
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);	// Deletes the array data from the GPU, no garbage collection, so must be cleared up
		VAO = 0;						// Removes array ID as the buffer has been deleted
	}

	vertex_count = 0;
	indexCount = 0;
}

Mesh::~Mesh() {
	ClearMesh();
}