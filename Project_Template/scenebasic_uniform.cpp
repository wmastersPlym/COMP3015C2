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


//SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 30, 30) {}
SceneBasic_Uniform::SceneBasic_Uniform() : teapot(13, glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f))) {}

void SceneBasic_Uniform::initScene()
{
    // Initial setup

    compile();
    glEnable(GL_DEPTH_TEST);

    projection = mat4(1.0f);



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

    // Load wood texture file into channel 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wood);

    
    // Load moss texture file into channel 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, moss);
    
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/Blinn-Phong.vert");
		prog.compileShader("shader/Blinn-Phong.frag");
		prog.link();
		prog.use();
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
    prog.setUniform("CurrentTime", t);
}   

void SceneBasic_Uniform::render()
{
    //float angle = -30.0f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    


    view = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 4.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 0.0f, 1.0f));



    setMatrices();
    teapot.render();

    
    
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