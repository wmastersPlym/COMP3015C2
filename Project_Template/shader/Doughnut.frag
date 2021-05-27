#version 460

#define PI 3.14159265

uniform vec4 Color;

layout (binding = 4) uniform sampler2D NoiseTex;

//uniform vec4 ColourA = vec4(0.3,0.3,0.9,1.0);
//uniform vec4 ColourB = vec4(1.0,1.0,1.0,1.0);

vec4 ColourA;
vec4 ColourB;

uniform float CurrentTime;


in vec2 TexCoord;

layout ( location = 0) out vec4 FragColor;

float sinColour(float offset, float scale) {
	return (sin(CurrentTime*scale + offset)+1)/3;
}

void updateColours() {
	ColourA = vec4(sinColour(0.0,1.4),sinColour(0.0,1.8),sinColour(0.0,2.2), 1.0);
	ColourB = vec4(sinColour(1.0,1.4),sinColour(1.0,1.8),sinColour(1.0,2.2), 1.0);

}


void main() {
	updateColours();

	vec4 noise = texture(NoiseTex, TexCoord);
	float t = (cos (noise.a * PI) + 1.0) / 2.0;
	vec4 color = mix(ColourA, ColourB, t);

	FragColor = vec4(color.rgb, 1.0);
	//FragColor = vec4(noise.rgb, 1.0);
	//FragColor = vec4(ColourA.rgb, 1.0);
}
