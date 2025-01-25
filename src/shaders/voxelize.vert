#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexel;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
	vec4 fragPos;
	vec3 normal;
	vec3 tangent;
} data_out;

layout (std140, binding = 0) uniform CameraData {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

uniform mat4 model;

void main() {
	data_out.fragPos = model * vec4(aPos, 1.f);
	data_out.normal = mat3(transpose(inverse(model))) * aNormal;
	data_out.tangent = mat3(transpose(inverse(model))) * aTangent;

	gl_Position = projection * view * data_out.fragPos;
}