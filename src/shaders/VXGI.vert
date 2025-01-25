#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexel;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

layout (binding = 0) uniform CameraData {
	mat4 projection;
	mat4 view;
	vec3 position;
};

uniform mat4 model;

out VS_OUT {
	vec4 fragPos;
	vec2 texel;
	vec3 tangent;
	vec3 normal;
} data_out;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.f);

	data_out.fragPos = vec4(aPos, 1.f);
	data_out.texel = aTexel;
	data_out.tangent = aTangent;
	data_out.normal = aNormal;
}