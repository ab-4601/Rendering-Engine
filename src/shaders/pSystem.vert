#version 460 core

layout (location = 0) in vec3 pos_attrib;
layout (location = 1) in vec3 vel_attrib;
layout (location = 2) in float lifetime_attrib;
layout (location = 3) in float elapsed_time_attrib;

layout (xfb_buffer = 0) out;

layout (xfb_offset = 0, xfb_stride = 32) out vec3 pos_out;
layout (xfb_offset = 12, xfb_stride = 32) out vec3 vel_out;
layout (xfb_offset = 24, xfb_stride = 32) out float lifetime_out;
layout (xfb_offset = 28, xfb_stride = 32) out float elapsed_time_out;

out vec2 texOffset1;
out vec2 texOffset2;
out float blend;

layout (std140, binding = 0) uniform CameraData {
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
};

uniform mat4 model;
uniform int numRows;
uniform float time;
uniform float particleDelta;

vec3 v0(vec3 p) {
	return vec3(sin(p.y * 5.f), -sin(p.x * 7.f), + cos(1.2f * p.z));
}

vec3 v(vec3 p) {
	vec3 t = vec3(0.f, 0.5f * time, 0.f);
	return v0(p.xyz - t) + v0(p.yxz + t);
} 

vec3 velocity(vec3 p) {
	const int n = 5;
	vec3 octaves = vec3(0.f);
	float scale = 1.5f;

	for(int i = 0; i < n; i++) {
		octaves = octaves + v(scale * p) / scale;
		scale *= 1.5f;
	}

	return 0.5f * octaves.xzy;
}

const uint k_hash = 1103515245U;

vec3 uhash3( uvec3 x ) {
    x = ((x >> 8U)^x.yzx) * k_hash;
    x = ((x >> 8U)^x.yzx) * k_hash;
    x = ((x >> 8U)^x.yzx) * k_hash;
    
    return vec3(x) / float(0xffffffffU);
}

vec3 hash( vec3 f ) { 
    return uhash3(uvec3(floatBitsToUint(f.x), floatBitsToUint(f.y), floatBitsToUint(f.z)));
}

vec2 getTextureOffset(int index) {
	int col = index % numRows;
	int row = index / numRows;

	vec2 offset = vec2(0.f);
	offset.x = float(col) / float(numRows);
	offset.y = float(row) / float(numRows);

	return offset;
}

void main() {
	gl_Position = projection * view * model * vec4(pos_attrib, 1.f);
	gl_PointSize = (lifetime_attrib - elapsed_time_attrib) * 0.03f;

	float lifeFactor = elapsed_time_attrib / lifetime_attrib;
	int stageCount = numRows * numRows;
	float atlasProgression = lifeFactor * float(stageCount);

	int index1 = int(floor(atlasProgression));
	int index2 = index1 < stageCount - 1 ? index1 + 1 : index1;

	texOffset1 = getTextureOffset(index1);
	texOffset2 = getTextureOffset(index2);
	blend = lifeFactor;

	vel_out = velocity(pos_attrib);
	pos_out = pos_attrib + 0.003f * vel_out.xyz;
	lifetime_out = lifetime_attrib;
	elapsed_time_out = elapsed_time_attrib + particleDelta;

	if(elapsed_time_out >= lifetime_out) {
		vec3 seed = vec3(float(gl_VertexID), time / 10.f, gl_Position.x);
		vec3 rand = hash(seed);
		pos_out = vec3(2.f * rand.xy - vec2(1.f), -2.f).xzy;

		elapsed_time_out = 0.f;
		lifetime_out = 500.f + 1000.f * dot(rand, vec3(0.333f));
	}
}