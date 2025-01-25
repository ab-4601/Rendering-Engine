#version 460 core

out vec4 fragColor;

in VS_OUT {
	vec3 color;
	vec2 texCoord1;
	vec2 texCoord2;
	float blend;
} fs_in;

uniform sampler2D textureSampler;

void main() {
	//fragColor = vec4(fs_in.color, 1.f);
	vec4 texColor1 = texture2D(textureSampler, fs_in.texCoord1);
	vec4 texColor2 = texture2D(textureSampler, fs_in.texCoord2);

	vec4 finalColor = mix(texColor1, texColor2, fs_in.blend);

	if(finalColor.a < 0.1)
		discard;

	fragColor = pow(finalColor * 3.f, vec4(2.2));
}