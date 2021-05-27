#version 460

in float Opacity;
in vec2 TexCoord;

uniform sampler2D ParticleTex;


layout ( location = 0 ) out vec4 FragColor;

void main() {
	FragColor = texture(ParticleTex, TexCoord);

	// Mixes white colour as particle gets older
	FragColor = vec4(mix(vec3(1,1,1), FragColor.xyz, Opacity), FragColor.a);

	FragColor.a *= Opacity;

}