#pragma once
#include "Light.h"

class PointLight :
    public Light
{

public:
    PointLight();
    PointLight(GLfloat red, GLfloat green, GLfloat blue,
                GLfloat ambientIntensity, GLfloat diffuseIntensity,
                GLfloat xPos, GLfloat yPos, GLfloat zPos,
                GLfloat con, GLfloat lin, GLfloat exp);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                   GLuint diffuseIntensityLocation, GLuint positionLocation,
                   GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation);

    ~PointLight();

private:
    glm::vec3 position;
    GLfloat constant, linear, exponent;
};

