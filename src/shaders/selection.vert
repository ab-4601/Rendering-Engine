#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;

layout (std140, binding = 0) uniform CameraData {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.f);
}