#version 460

#define PI 3.1415926535897932384626433832795
 
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexVelocity; 
layout (location = 2) in float VertexAge; 

// Render pass
uniform int Pass;

out vec3 Position;
out vec3 Velocity;
out float Age;


out float Opacity; // Opacity of particle
out vec2 TexCoord; // Texture coorinate

uniform float Time;					// Current time
uniform float DeltaT;				// Time since last frame
uniform vec3 Acceleration;			// Acceleration
uniform float TTL;					// Paricle Time to Live
uniform vec3 Emitter = vec3(0);				// Where the particle is emitted from
uniform mat3 EmitterBasis;			// Rotation that rotates y axis to the direction of emitter
uniform float ParticleSize;			// Paricle size
uniform float ParticleSizeMax;		// Maximum particleSize

uniform mat4 MV;
uniform mat4 Proj;

uniform sampler1D randomTex;

const vec3 offsets[] = vec3[](vec3(-0.5, -0.5,0), vec3(0.5, -0.5,0), vec3(0.5, 0.5,0), 
							  vec3(-0.5, -0.5,0), vec3(0.5, 0.5,0), vec3(-0.5, 0.5,0));

const vec2 texCoords[] = vec2[](vec2(0,0),vec2(1,0),vec2(1,1),vec2(0,0),vec2(1,1),vec2(0,1));

vec3 randomInitalVelocity() {
	float velocity = mix(0.1, 0.5, texelFetch(randomTex, 2* gl_VertexID, 0).r);
	return EmitterBasis * vec3(0, velocity, 0);

}

vec3 randomInitialPosition() {
	float offset = mix(-0.1, 0.1, texelFetch(randomTex, 2 * gl_VertexID + 1, 0).r);
	return Emitter + vec3(offset,0,0);
}


void update() {

	Age = VertexAge + DeltaT;

	if(VertexAge < 0 || VertexAge > TTL) {
		// Particle needs to be recycled
		Position = randomInitialPosition();
		Velocity = randomInitalVelocity();
		if(VertexAge > TTL) 
			Age = (VertexAge - TTL) + DeltaT;
	} else {
		Position = VertexPosition + VertexVelocity * DeltaT;
		Velocity = VertexVelocity + Acceleration * DeltaT;
		//Age = VertexAge + DeltaT;
	}
}

void render() {
	Opacity = 0.0;
	vec3 posCam = vec3(0.0);
	if(VertexAge >= 0.0) {
		float agePct = VertexAge / TTL;
		Opacity = clamp(1.0 - agePct, 0, 1);
		//posCam = (MV * vec4(VertexPosition,1)).xyz + offsets[gl_VertexID] * ParticleSize;
		posCam = (MV * vec4(VertexPosition,1)).xyz + offsets[gl_VertexID] * mix(ParticleSize, ParticleSizeMax, agePct);
		
	}
	TexCoord = texCoords[gl_VertexID];

	gl_Position = Proj * vec4(posCam,1);
}


void main() {
	
	if(Pass == 1) {
		update();
	} else {
		render();
	}
}
