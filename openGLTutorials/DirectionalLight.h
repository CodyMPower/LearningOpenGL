#pragma once
#include "Light.h"

#include "Light.h"
class DirectionalLight :
    public Light
{
public:
    DirectionalLight();

    DirectionalLight(GLfloat shadowWidth, GLfloat shadowHeight,
                        GLfloat red, GLfloat green, GLfloat blue,
                        GLfloat ambientIntensity, GLfloat diffuseIntensity,
                        GLfloat xDir, GLfloat yDir, GLfloat zDir);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint directionLocation);

    glm::mat4 CalculateLightTransform();

    ~DirectionalLight();

private:
    glm::vec3 direction;	// Direction the light is coming from
};

