#version 460 core

out vec4 fragColor;

struct Particle {
	vec4 position;
	vec4 velocity;
	vec4 acceleration;
	float density;
	float pressure;
};

in Particle particle;

void main() {
	fragColor = vec4(particle.velocity);
}