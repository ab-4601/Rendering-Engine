#version 460 core

struct Particle {
	vec4 position;
	vec4 velocity;
	vec4 acceleration;
	float density;
	float pressure;
};

layout (std430, binding = 0) buffer Particles {
	Particle particles[];
};

layout (std140, binding = 0) uniform CameraData {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

out Particle particle;

void main() {
	Particle currParticle = particles[gl_VertexID];

	gl_Position = projection * view * currParticle.position;
	particle = currParticle;
}