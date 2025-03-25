#version 460 core

const vec4 quad[4] = vec4[] (
	vec4(-1.f,  1.f, 0.f, 1.f),
	vec4(-1.f, -1.f, 0.f, 1.f),
	vec4( 1.f,  1.f, 0.f, 1.f),
	vec4( 1.f, -1.f, 0.f, 1.f)
);

out vec2 texel;

void main() {
	gl_Position = vec4(quad[gl_VertexID]);
	texel = 0.5f * (quad[gl_VertexID].xy + vec2(1.f));
}