#pragma once
#include "Shader.h"
#include "RenderedObject.h"
#include "Food.h"
#include "Renderer.h"
#include "PlayerObject.h"

#include <vector>

class Scene
{
public:
	Scene();

	virtual void generateScene();
	Renderer getRenderer() { return this->renderer; }
	std::vector<Food*> getFoodVec() { return this->food; }
	PlayerObject* getPlayer() { return this->player; }

	~Scene();

private:
	Renderer renderer;
	std::vector<Food*> food;
	PlayerObject* player;
};

