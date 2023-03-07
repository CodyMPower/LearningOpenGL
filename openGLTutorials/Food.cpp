#include "Food.h"

Food::Food() : RenderedObject()
{
	this->size = 0;
}

Food::Food(Mesh* objectMesh, Texture* objectTexture, Material* objectMaterial) : RenderedObject(objectMesh, objectTexture, objectMaterial)
{
	this->size = 0;
}

Food::~Food()
{
}
