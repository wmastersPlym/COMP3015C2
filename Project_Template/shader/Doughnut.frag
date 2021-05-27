#version 460

#define PI 3.14159265

uniform vec4 Color;

layout (binding = 4) uniform sampler2D NoiseTex;

//uniform vec4 ColourA = vec4(0.3,0.3,0.9,1.0);
//uniform vec4 ColourB = vec4(1.0,1.0,1.0,1.0);

uniform vec4 ColourA = vec4(1.0,0.0,0.0,1.0);
uniform vec4 ColourB = vec4(0.0,1.0,1.0,1.0);



in vec2 TexCoord;

layout ( location = 0) out vec4 FragColor;


void main() {
	vec4 noise = texture(NoiseTex, TexCoord);
	float t = (cos (noise.a * PI) + 1.0) / 2.0;
	vec4 color = mix(ColourA, ColourB, t);

	FragColor = vec4(color.rgb, 1.0);
	//FragColor = vec4(noise.rgb, 1.0);
	//FragColor = vec4(ColourA.rgb, 1.0);
}
