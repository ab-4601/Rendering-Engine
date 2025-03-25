#version 460 core

const vec4 quad[4] = vec4[] (
	vec4(-1.f,  1.f, 0.f, 1.f),
	vec4(-1.f, -1.f, 0.f, 1.f),
	vec4( 1.f,  1.f, 0.f, 1.f),
	vec4( 1.f, -1.f, 0.f, 1.f)
);

out vertexData {
	vec2 fragCoord;
	vec2 texel;
} data_out;

void main() {
	gl_Position = quad[gl_VertexID];
	data_out.texel = 0.5f * (quad[gl_VertexID].xy + vec2(1.f));
}