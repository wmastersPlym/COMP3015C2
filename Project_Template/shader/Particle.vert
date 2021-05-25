#version 460

layout (location = 0) in vec3 VertexInitVel; // Initial velocity
layout (location = 1) in float BirthTime; // Time at which the particle was created

out float Opacity; // Opacity of particle
out vec2 TexCoord; // Texture coorinate

uniform float Time;									// Current time
uniform vec3 Gravity = vec3(0.0, -0.05, 0.0);	// Gravity vector
uniform float TTL;									// Paricle Time to Live
uniform float ParticleSize = 1.0;					// Paricle size
uniform vec3 EmittPos;								// Where the particle is emitted from

uniform mat4 MV;
uniform mat4 Proj;

const vec3 offsets[] = vec3[](vec3(-0.5, -0.5,0), vec3(0.5, -0.5,0), vec3(0.5, 0.5,0), 
							  vec3(-0.5, 0.5,0), vec3(0.5, 0.5,0), vec3(-0.5, 0.5,0));

const vec2 texCoords[] = vec2[](vec2(0,0),vec2(1,0),vec2(1,1),vec2(0,0),vec2(1,1),vec2(0,1));



void main() {
	vec3 cameraPos;
	
	float t = Time - BirthTime;
	if(t >=0 && t < TTL) {
		// Particle is still alive

		vec3 pos = EmittPos + VertexInitVel * t + Gravity * t * t;

		// Offset vertex based off ID
		cameraPos = (MV * vec4(pos,1)).xyz + (offsets[gl_VertexID] * ParticleSize);
		Opacity = mix(1,0, t/TTL);
	
	} else {
		// Particle has "died"
		cameraPos= vec3(0);
		Opacity = 0.0f;
	}

	TexCoord = texCoords[gl_VertexID];

	gl_Position = Proj * vec4(cameraPos, 1);
}
