#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include "helper/torus.h";
#include "helper/teapot.h"
#include "helper/plane.h";
#include "helper/random.h";
#include "helper/particleutils.h";


class SceneBasic_Uniform : public Scene
{
private:
    
    GLSLProgram prog, particleProg, flatProg;

    Random rand;

    GLuint initVel, startTime, particles, nParticles;

    //Grid grid;

    glm::vec3 emitterPos, emitterDir;

    float time, TTL;


    float angle;
    Teapot teapot;

    void initBuffers();
    float randFloat();

    void compile();
    void setMatrices();
    void setParticleMatrices();

    glm::mat4 rotationMatrix;

public:
    SceneBasic_Uniform();

    void initScene();
    void processInput(GLFWwindow * window);
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
