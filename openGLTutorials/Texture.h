#pragma once
#include <GL/glew.h>

#include "stb_image.h"

class Texture
{
public:

	Texture();				// Default constructor
	Texture(char* fileLoc);	// Constructor with file location

	bool LoadTexture();		// Loads texture into GPU
	bool LoadTextureA();	// Loads a texture with an alpha chanel

	void UseTexture();		// Uses the texture in one of the texture units
	void ClearTexture();	// Deletes the texture from the GPU

	~Texture();	// Deletes texture from GPU and destroys the Texture

private:
	GLuint textureID;
	int width, height, bitDepth;

	char* fileLocation;
};

