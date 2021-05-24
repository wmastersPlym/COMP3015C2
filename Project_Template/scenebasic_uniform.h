#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helper/torus.h";
#include "helper/teapot.h"
#include "helper/plane.h";

class SceneBasic_Uniform : public Scene
{
private:
    float angle;
    //Plane plane;

    //Torus torus;
    Teapot teapot;
    GLSLProgram prog;

    void compile();
    void setMatrices();

    glm::mat4 rotationMatrix;

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
