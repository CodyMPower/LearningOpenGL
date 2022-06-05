#include "Texture.h"

Texture::Texture() {
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = (char*) "";
}

Texture::Texture(char* fileLoc) {
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLoc;
}

bool Texture::LoadTexture() {
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);	// Gets the texture Data from a file

	if (!texData) {	// Check if texData recieved the texture data from the image
		printf("Failed to find: '%s'\n", fileLocation);
		return false;
	}

	glGenTextures(1, &textureID);				// Generates a Texture ID
	glBindTexture(GL_TEXTURE_2D, textureID);	// Binds the texture to bind the texture information

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		// Sets the x axis wrap to repeat the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		// Sets the y axis wrap to repeat the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Sets the minimum filtering to linearly interpolate the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Sets the magnification filtering to linearly interpolate the texture

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);	// Binds the texture information to the texture ID
	glGenerateMipmap(GL_TEXTURE_2D);																// Generates the Mipmap for the texture ID

	// Bind texture to GPU before unbinding the texture, doesn't work after unbinding

	glBindTexture(GL_TEXTURE_2D, 0);	// Unbinds the textureID

	stbi_image_free(texData);	// Frees the texture Data as the information is now buffered on the GPU

	return true;

}

bool Texture::LoadTextureA() {
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, STBI_rgb_alpha);	// Gets the texture Data from a file

	if (!texData) {	// Check if texData recieved the texture data from the image
		printf("Failed to find: '%s'\n", fileLocation);
		return false;
	}

	glGenTextures(1, &textureID);				// Generates a Texture ID
	glBindTexture(GL_TEXTURE_2D, textureID);	// Binds the texture to bind the texture information

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		// Sets the x axis wrap to repeat the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		// Sets the y axis wrap to repeat the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Sets the minimum filtering to linearly interpolate the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Sets the magnification filtering to linearly interpolate the texture

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);	// Binds the texture information to the texture ID
	glGenerateMipmap(GL_TEXTURE_2D);																// Generates the Mipmap for the texture ID

	// Bind texture to GPU before unbinding the texture, doesn't work after unbinding

	glBindTexture(GL_TEXTURE_2D, 0);	// Unbinds the textureID

	stbi_image_free(texData);	// Frees the texture Data as the information is now buffered on the GPU
	
	return true;

}

void Texture::UseTexture() {
	glActiveTexture(GL_TEXTURE0);	// Texture unit access, how the graphics card accesses textures, modern GPUs have 16 - 32 texture units
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture() {
	glDeleteTextures(1, &textureID);	// Deletes the texture from the GPU
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = (char*) "";
}

Texture::~Texture() {
	ClearTexture();
}