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


SceneBasic_Uniform::SceneBasic_Uniform() :  teapot(13, glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f))),
                                            torus(0.7f, 0.3f, 30, 30),
                                            angle(0.0f), drawBuff(1), time(0), deltaT(0), nParticles(2000), TTL(2.0f), emitterPos(2.9, 2.32, 0),
                                            emitterDir(2, 2, 0)                                                         //emitterPos(2.9, 2.3, 0)
{
}

void SceneBasic_Uniform::initScene()
{
    // Initial setup

    compile();
    
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);


    // Enables alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    
    // Setting particle effects shader

    // Setting up particle textures
    //model = mat4(1.0f);
    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("media/texture/smoke.png");

    glActiveTexture(GL_TEXTURE1);
    ParticleUtils::createRandomTex1D(nParticles * 3);
    
    initBuffers();
    
    particleProg.use();

    particleProg.setUniform("randomTex", 1);
    particleProg.setUniform("ParticleTex", 0);
    particleProg.setUniform("TTL", TTL);
    particleProg.setUniform("Acceleration", vec3(0.0f, 1.0f, 0.0f));
    particleProg.setUniform("ParticleSize", 0.05f);
    particleProg.setUniform("ParticleSizeMax", 1.0f);
    particleProg.setUniform("Emitter", emitterPos);
    particleProg.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));


    // Setting up blinn phong shader

    //projection = mat4(1.0f);
    
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

    // Load wood texture file into channel 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, wood);

    
    // Load moss texture file into channel 3
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, moss);



    // Setting up doughnut shader

    

    doughnutProg.use();

    glEnable(GL_DEPTH_TEST);

    projection = mat4(1.0f);

    glActiveTexture(GL_TEXTURE4);
    GLuint noiseTex = NoiseTex::generate2DTex(6.0f);

    
    glBindTexture(GL_TEXTURE_2D, noiseTex);

    
    
    
}

void SceneBasic_Uniform::compile()
{
	try {
        // Compliles, links and sets up the transform feedback for particle shader
        particleProg.compileShader("shader/Particle.vert");
        particleProg.compileShader("shader/Particle.frag");

        GLuint progHandle = particleProg.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);

        particleProg.link();

        // compiles and links blinn phong shader
		prog.compileShader("shader/Blinn-Phong.vert");
		prog.compileShader("shader/Blinn-Phong.frag");
		prog.link();

        // Compiles and links water shader
        doughnutProg.compileShader("shader/Doughnut.vert");
        doughnutProg.compileShader("shader/Doughnut.frag");
        doughnutProg.link();
		
        


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
    deltaT = t - time;
    time = t;
    //prog.setUniform("CurrentTime", t);
    //particleProg.setUniform("Time", t);
}   

void SceneBasic_Uniform::render()
{
    // Varies the cameras x position to get a constantly changing view of the scene
    float A = 2.0f; // Amplitude of wave
    float Lambda = 4.0f; // Wavelength
    float V = 0.5f; // wave velocity
    float camXPos = A * sin((2 * 3.1415926535897932384626433832795f / Lambda) * (V * time));

    // Sets camera
    view = mat4(1.0f);
    view = glm::lookAt(vec3(camXPos, 4.0f, 6.0f), vec3(0.0f, 2.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    // Sets up Teapot
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

    prog.use();
    prog.setUniform("CurrentTime", time);
    setMatrices();
    teapot.render();


    //  Sets up doughnut
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-2.0f, -2.0f, 0.0f));

    doughnutProg.use();

    setDoughnutMatrices();
    //teapot.render();
    torus.render();

    
    // Sets up particles
    model = mat4(1.0f);


    particleProg.use();
    particleProg.setUniform("Time", time);
    particleProg.setUniform("DeltaT", deltaT);

    
    // UpdatePass
    particleProg.setUniform("Pass", 1);
    
    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuff]);
    glBeginTransformFeedback(GL_POINTS);
    
    glBindVertexArray(particleArray[1 - drawBuff]);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);
    
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    

    // RenderPass
    
    particleProg.setUniform("Pass", 2);

    setParticleMatrices();

    glDepthMask(GL_FALSE);
    glBindVertexArray(particleArray[drawBuff]);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    // swap buffers
    drawBuff = 1 - drawBuff;

    
}

void SceneBasic_Uniform::initBuffers() {
    particleProg.use();
    
    // Creates the initial buffers 
    glGenBuffers(2, posBuf); // position buffer
    glGenBuffers(2, velBuf); // velocity buffer
    glGenBuffers(2, age);    // Age buffer

    // Allocates space for the buffers
    int size = nParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    // Fill the start time Buffer
    std::vector<GLfloat> tmpData(nParticles);
    float rate = TTL / nParticles;
    for (int i = 0; i < nParticles; i++) {
        tmpData[i] = rate * (i - nParticles);
    }

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), tmpData.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create Vertex arrays for eaach set of buffers
    glGenVertexArrays(2, particleArray);

    // set up particle array 0
    glBindVertexArray(particleArray[0]);

    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);


    // set up particle array 1
    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);


    glBindVertexArray(0);

    // Setup the feedback objects
    glGenTransformFeedbacks(2, feedback);

    // TransformFeedback 0
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

    // TransformFeedback 1
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    
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

void SceneBasic_Uniform::setDoughnutMatrices() {

    mat4 mv = view * model;
    doughnutProg.setUniform("MV", mv);
    doughnutProg.setUniform("MVP", projection * mv);
    doughnutProg.setUniform("Proj", projection);
}

float SceneBasic_Uniform::randFloat() {
    return Random().nextFloat();
}