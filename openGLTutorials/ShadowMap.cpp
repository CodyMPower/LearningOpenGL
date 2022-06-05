#include "ShadowMap.h"

ShadowMap::ShadowMap() {
	FBO = 0;
	shadowMap = 0;
}

bool ShadowMap::Init(GLuint width, GLuint height) {
	shadowWidth = width;
	shadowHeight = height;

	glGenFramebuffers(1, &FBO);
	glGenTextures(1, &shadowMap);
	
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	// Depth Component will give the depth of a pixel from the given perspective as a normalized value 0 is furthest and 1 is nearest (near plane)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// Clamp to the border of the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);		
	float bColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColour);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Sets the minimum filtering to linearly interpolate the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Sets the magnification filtering to linearly interpolate the texture
	// Same as from Texture.cpp save for final parameters for the first 2 lines

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);	// GL_FRAMEBUFFER means the buffer can both be read and written to
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// GL_DEPTH_ATTACHMENT reffers to what part of the image is being buffered to the shadow map

	glDrawBuffer(GL_NONE);	// Don't draw or read from color portion
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer Error: %i\n", status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowMap::Write() {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void ShadowMap::Read(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}


ShadowMap::~ShadowMap() {
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}

	if (shadowMap) {
		glDeleteTextures(1, &shadowMap);
	}
}