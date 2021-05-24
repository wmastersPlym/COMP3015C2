#version 460

#define PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;



out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;





uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

uniform float CurrentTime;


float wiggleAnimY(float x, float time) {
    float A = 0.25f; // Amplitude of wave
    float Lambda = 2.0f; // Wavelength
    float V = 1.0f; // wave velocity

    return A * sin((2*PI/Lambda) * (x - V*time));
}

void main()
{

    Normal = normalize(NormalMatrix * VertexNormal);
    Position = ModelViewMatrix * vec4(VertexPosition, 1.0f);
    TexCoord = VertexTexCoord;

    
    
    vec4 offset = new vec4(0.0f,0.0f,0.0f,0.0f);
    offset[1] += wiggleAnimY(Position[0], CurrentTime);
    
    gl_Position = MVP * vec4(VertexPosition,1.0) + offset;

}

