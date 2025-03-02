#version 460 core

out vec4 fragColor;

in vec2 texel;
in vec2 texOffset1;
in vec2 texOffset2;
in float blend;

uniform sampler2D particleTexture;
uniform int numRows;

void main() {
	vec2 textureCoords = gl_PointCoord;
	textureCoords /= numRows;

	vec4 texColor1 = texture2D(particleTexture, textureCoords + texOffset1);
	vec4 texColor2 = texture2D(particleTexture, textureCoords + texOffset2);

	vec4 finalColor = mix(texColor1, texColor2, blend);

	if(finalColor.a < 0.1f)
		discard;

	fragColor = pow(finalColor, vec4(2.2f));
}