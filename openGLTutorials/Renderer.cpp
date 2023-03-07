#include "Renderer.h"

Renderer::Renderer()
{
	this->renderedObjects.clear();
}

void Renderer::render(GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	for (RenderedObject* object : renderedObjects)
		object->renderObject(uniformModel, uniformSpecularIntensity, uniformShininess);
}
