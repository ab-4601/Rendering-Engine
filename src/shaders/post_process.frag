#version 460 core

out vec4 fragColor;

in vec2 texel;

layout (binding = 0) uniform sampler2D hdrBuffer;
layout (binding = 1) uniform sampler2D bloomBuffer;
layout (binding = 2) uniform sampler2D blendWeightTexture;

uniform ivec2 screenResolution;
uniform float exposure;
uniform float knee;
uniform float time;
uniform bool enableBloom;

const float gamma_const = 2.2f;
const float FXAA_REDUCE_MUL = 1.f / 8.f;
const float FXAA_REDUCE_MIN = 1.f / 128.f;
const vec2 FXAA_SPOIL = vec2(1.f);

mat3 aces_input_matrix = mat3(
	vec3(0.59719f, 0.35458f, 0.04823f),
    vec3(0.07600f, 0.90834f, 0.01566f),
    vec3(0.02840f, 0.13383f, 0.83777f)
);

mat3 aces_output_matrix = mat3(
	vec3( 1.60475f, -0.53108f, -0.07367f),
    vec3(-0.10208f,  1.10813f, -0.00605f),
    vec3(-0.00327f, -0.07276f,  1.07602f)
);

vec3 FXAA();
vec3 SMAA();
vec3 gamma(vec3 color) { return pow(color, vec3(1.f / gamma_const)); }
vec3 aces(vec3 color);
vec4 sobel_filter();
vec4 blur();
vec4 vignette(vec4 color);
vec4 glitch();

void main() {
	vec3 hdrColor = texture2D(hdrBuffer, texel).rgb;
	vec3 bloomColor = texture2D(bloomBuffer, texel).rgb;

	if(enableBloom)
		hdrColor = mix(hdrColor, bloomColor, knee);

	vec3 mapped = aces(hdrColor);

	mapped = gamma(mapped) * exposure;

	fragColor = vec4(mapped, 1.f);
}

vec3 mul(const mat3 m, vec3 v) {
	float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
    float y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
    float z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];

    return vec3(x, y, z);
}

vec3 rtt_and_odt_fit(vec3 v) {
	vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 FXAA() {
	vec3 color = texture2D(hdrBuffer, texel).rgb;
	vec3 luma = vec3(0.299f, 0.587f, 0.114f);

	vec2 texelOffset = 1.f / vec2(textureSize(hdrBuffer, 0));

	vec2 posM1 = texel - texelOffset;
	vec2 posP1 = texel + texelOffset;
	vec2 posM0 = texel - vec2(texelOffset.x, 0.f);
	vec2 posP0 = texel + vec2(texelOffset.x, 0.f);
	vec2 pos0M = texel - vec2(0.f, texelOffset.y);
	vec2 pos0P = texel + vec2(0.f, texelOffset.y);

	float lumaM1 = dot(texture2D(hdrBuffer, posM1).rgb, luma);
	float lumaP1 = dot(texture2D(hdrBuffer, posP1).rgb, luma);
	float lumaM0 = dot(texture2D(hdrBuffer, posM0).rgb, luma);
	float lumaP0 = dot(texture2D(hdrBuffer, posP0).rgb, luma);
	float luma0M = dot(texture2D(hdrBuffer, pos0M).rgb, luma);
	float luma0P = dot(texture2D(hdrBuffer, pos0P).rgb, luma);

	float lumaCenter = dot(color, luma);

	float edgeHorizontal = abs(lumaM1 + lumaP1 - 2.f * lumaCenter);
	float edgeVertical = abs(luma0M + luma0P - 2.f * lumaCenter);
	float edgeStrength = max(edgeHorizontal, edgeVertical);

	float blendFactor = clamp(edgeStrength, 0.f, 1.f);
	color = mix(color, (texture2D(hdrBuffer, posM1).rgb + texture2D(hdrBuffer, posP1).rgb) * 0.5f, blendFactor);

	return color;
}

vec3 SMAA() {
	vec3 color = texture2D(hdrBuffer, texel).rgb;
	vec4 weights = texture2D(blendWeightTexture, texel);

	vec3 blendedColor = mix(vec4(color, 1.f), weights, weights.a).rgb;

	return blendedColor;
}

vec3 aces(vec3 color) {
	color = mul(aces_input_matrix, color);
    color = rtt_and_odt_fit(color);
    return mul(aces_output_matrix, color);
}

vec4 sobel_filter() {
	vec4 north = texelFetch(hdrBuffer, ivec2(gl_FragCoord) + ivec2( 0,  1), 0);
	vec4 south = texelFetch(hdrBuffer, ivec2(gl_FragCoord) + ivec2( 0, -1), 0);
	vec4 east = texelFetch(hdrBuffer, ivec2(gl_FragCoord) + ivec2( 1,  0), 0);
	vec4 west = texelFetch(hdrBuffer, ivec2(gl_FragCoord) + ivec2(-1,  0), 0);

	vec4 vertical_diff = abs(north - south);
	vec4 horizontal_diff = abs(east - west);

	vec4 edge = vec4(length(vertical_diff) + length(horizontal_diff));
	return edge;
}

vec4 blur() {
	int hw = 5;
	float n = 0.f;
	vec4 blur = vec4(0.f);

	for(int i = -hw; i <= hw; i++) {
		for(int j = -hw; j <= hw; j++) {
			blur += texelFetch(hdrBuffer, ivec2(gl_FragCoord) + ivec2(i, j), 0);
			n += 1.f;
		}
	}

	blur /= n;
	return blur;
}

vec4 vignette(vec4 color) {
	vec2 coord = texel - vec2(0.5f);
	float darken = smoothstep(1.f, 0.3f, length(coord));
	return darken * color;
}

vec4 glitch() {
	float flicker = step(0.f, fract(0.25f * time) - 0.92) * abs(fract(sin(15656.f * time)));

	vec4 color = vec4(1.f);
	color.r = texture2D(hdrBuffer, texel + vec2(0.25f * flicker, 0.f)).r;
	color.g = texture2D(hdrBuffer, texel + vec2(0.011f * fract(time * 1000.f + 1500.f * texel.y), 0.f)).g;
	color.b = texture2D(hdrBuffer, texel + vec2(-0.001f, 0.005f)).b;

	float lines = (mod(gl_FragCoord.y, 2.5f) + 0.75f) / 2.f;
	return lines * color * (1.f - flicker);
}