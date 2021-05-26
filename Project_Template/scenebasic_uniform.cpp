#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <sstream>

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;



#include "helper/glutils.h"
#include "helper/texture.h"


#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;


SceneBasic_Uniform::SceneBasic_Uniform() : teapot(13, glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f))),
                                           time(0), TTL(5.5f), nParticles(8000), emitterPos(0,0,0), emitterDir(0,1,0) {
    angle = 0;
}

void SceneBasic_Uniform::initScene()
{
    // Initial setup

    compile();
    

    // Enables alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    
    // Setting particle effects
    

    initBuffers();


    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("media/texture/bluewater.png");

    
    particleProg.use();
    particleProg.setUniform("ParticleTex", 0);
    
    particleProg.setUniform("Time", float(glfwGetTime()));
    particleProg.setUniform("TTL", TTL);
    particleProg.setUniform("ParticleSize", 0.05f);
    particleProg.setUniform("Gravity", vec3(0.0f, 1.0f, 0.0f));
    particleProg.setUniform("EmittPos", emitterPos);

    


    projection = mat4(1.0f);

    prog.use();

    // Setting up lights

    // Setting x and z position for each of the 3 lights
    float x, z;
    for (int i = 0; i < 3; i++) {
        std::stringstream name;
        name << "Lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        //prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
        prog.setUniform(name.str().c_str(), glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
        std::cout << "Light( " + name.str() + ") -> X: " + std::to_string(x) + ", Z: " + std::to_string(z) + "." << std::endl;
    }

    // Setting variables for lights
    prog.setUniform("lights[0].L", vec3(0.0f, 0.0f, 0.8f));
    prog.setUniform("lights[1].L", vec3(0.0f, 0.8f, 0.0f));
    prog.setUniform("lights[2].L", vec3(0.8f, 0.0f, 0.0f));

    prog.setUniform("lights[0].La", vec3(0.0f, 0.0f, 0.2f));
    prog.setUniform("lights[1].La", vec3(0.0f, 0.2f, 0.0f));
    prog.setUniform("lights[2].La", vec3(0.2f, 0.0f, 0.0f));
    


    // Setting up Model 

    // Setting material properties for teapot
    prog.setUniform("Material.Kd", 0.4f, 0.2f, 0.2f);
    prog.setUniform("Material.Ks", 0.9f, 0.45f, 0.45f);
    prog.setUniform("Material.Ka", 0.5f, 0.25f, 0.25f);
    prog.setUniform("Material.Shininess", 10.0f);

    // Loading teaxtures
    GLuint wood = Texture::loadTexture("media/texture/hardwood2_diffuse.jpg");
    GLuint moss = Texture::loadTexture("media/texture/moss.png");

    // Load wood texture file into channel 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wood);

    
    // Load moss texture file into channel 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, moss);
    
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/Blinn-Phong.vert");
		prog.compileShader("shader/Blinn-Phong.frag");
		prog.link();
		

        particleProg.compileShader("shader/Particle.vert");
        particleProg.compileShader("shader/Particle.frag");
        particleProg.link();


	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::processInput(GLFWwindow * window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        std::cout << "W PRESSED" << std::endl;
    }
}

void SceneBasic_Uniform::update( float t )
{
    // Updating the angle used to rotate the teapot
    angle += 0.2f;
    if (angle > 360.0f) {
        angle -= 360.0f;
    }

    // Passing the current time to the vertex shader for the wiggle animation
    time = t;
    //prog.setUniform("CurrentTime", t);
    //particleProg.setUniform("Time", t);
}   

void SceneBasic_Uniform::render()
{
    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    // Sets camera
    view = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    // Sets up Teapot
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 0.0f, 1.0f));

    prog.use();
    prog.setUniform("CurrentTime", time);
    setMatrices();
    //teapot.render();

    model = mat4(1.0f);
    // Sets up particles

    particleProg.use();
    setParticleMatrices();

    glDepthMask(GL_FALSE);
    particleProg.use();
    particleProg.setUniform("Time", float(glfwGetTime()));
    glBindVertexArray(particles);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    
}

void SceneBasic_Uniform::initBuffers() {
    particleProg.use();
    // Creates the initial buffers 
    glGenBuffers(1, &initVel);
    glGenBuffers(1, &startTime);

    // Allocates space for the buffers
    int size = nParticles * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

    // Fills velocity buffer with random velocities 
    glm::mat3 emitterBasis = ParticleUtils::makeArbitraryBasis(emitterDir);
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);
    for (uint32_t i = 0; i < nParticles; i++) {
        // Pick the direction of the velocity
        theta = glm::mix(0.0f, glm::pi<float>() / 20.0f, randFloat());
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

        v.x = sinf(theta) * cosf(phi);
        v.y = sinf(theta);
        v.z = sinf(theta) * sinf(phi);

        // Scale to set the magnitude of the velocity
        velocity = glm::mix(0.5f, 1.5f, randFloat());
        v = glm::normalize(emitterBasis * v) * velocity;

        data[3 * i] = v.x;
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;

    }

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, data.data());

    // Fill the start time Buffer

    float rate = TTL / nParticles;
    for (int i = 0; i < nParticles; i++) {
        data[i] = rate * i;
    }

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &particles);
    glBindVertexArray(particles);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);
    
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0,0,w,h);

    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);


}

void SceneBasic_Uniform::setMatrices() {
    mat4 mv = view * model;

    //prog.setUniform("cameraPos", vec3(0.0f, 0.0f, 2.0f));
    prog.setUniform("cameraPos", vec3(0.5f, 0.75f, 2.0f));
    
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);

    
}

void SceneBasic_Uniform::setParticleMatrices() {

    mat4 mv = view * model;

    particleProg.setUniform("MV", mv);
    particleProg.setUniform("Proj", projection);
}

float SceneBasic_Uniform::randFloat() {
    return Random().nextFloat();
}