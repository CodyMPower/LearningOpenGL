#pragma once
#include "RenderedObject.h"
class Food :
    public RenderedObject
{
public:
    Food();
    Food(Mesh* objectMesh, Texture* objectTexture, Material* objectMaterial);

    void setSize(double size) { this->size = size; }
    double getSize() { return this->size; }

    ~Food();

private:
    double size;
};

