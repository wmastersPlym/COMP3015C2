#version 460

#define PI 3.14159265

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec2 VertexTexCoord;

out vec2 TexCoord;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 Proj;

uniform float CurrentTime;


// Local variables used for sin waves
float A = 0.2f; // Amplitude of wave
float Lambda = 2.0f; // Wavelength
float V = 1.0f; // wave velocity

float wiggleAnimY(float x, float time) {
    return A * sin((2*PI/Lambda) * (x - V*time));
}

void main()
{
    TexCoord = VertexTexCoord;
    
    vec4 Position = MVP *vec4(VertexPosition,1.0);

	vec4 offset = vec4(0.0f,0.0f,0.0f,0.0f);
    offset[1] += wiggleAnimY(Position[0], CurrentTime);
    
    
    gl_Position = MVP *vec4(VertexPosition,1.0) + offset ;


    
}
