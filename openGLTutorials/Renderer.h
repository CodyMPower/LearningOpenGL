#pragma once
#include "RenderedObject.h"

#include <vector>

#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer();

	void addObject(RenderedObject* object) { this->renderedObjects.push_back(object); }
	void clearObjects() { this->renderedObjects.clear(); }
	void render(GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess);

	~Renderer();

private:
	std::vector<RenderedObject*> renderedObjects;
};

