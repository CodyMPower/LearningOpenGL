#pragma once
#include <GL/glew.h>

#include "stb_image.h"

class Texture
{
public:

	Texture();				// Default constructor
	Texture(char* fileLoc);	// Constructor with file location

	void LoadTexture();		// Loads texture into GPU
	void UseTexture();		// Uses the texture in one of the texture units
	void ClearTexture();	// Deletes the texture from the GPU

	~Texture();	// Deletes texture from GPU and destroys the Texture

private:
	GLuint textureID;
	int width, height, bitDepth;

	char* fileLocation;
};

