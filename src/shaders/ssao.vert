#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexel;

out vec2 texel;

void main() {
	texel = aTexel;
	gl_Position = vec4(aPos, 1.f);
}