#version 460 core

layout (triangles) in;
layout (points, max_vertices = 1) out;

uniform vec3 gridOrigin;
uniform float voxelSize;
uniform int gridResolution;

layout (binding = 1, r32ui) uniform uimage3D voxelGrid;

void main() {
    vec3 triCenter = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0;
    ivec3 voxelCoord = ivec3((triCenter - gridOrigin) / voxelSize);
    imageAtomicAdd(voxelGrid, voxelCoord, 1);
}