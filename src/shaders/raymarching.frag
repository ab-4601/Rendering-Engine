#version 460 core

out vec4 fragColor;

uniform vec3 resolution;
uniform vec2 cursor;
uniform float time;
uniform float deltaTime;
uniform bool isMouseClicked;

in vertexData {
	vec2 fragCoord;
	vec2 texel;
} data_in;

mat2 rotate2D(float angle);
mat3 rotate3D(vec3 axis, float angle);
vec3 rotate3D(vec3 p, vec3 axis, float angle); // Rodrigues rotation formula;

vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d) { return a + b * cos(6.283185 * (c * t + d)); }

float sdSphere(vec3 p, float r);
float sdBox(vec3 p, vec3 b);
float sdPlane(vec3 p, vec3 n, float h);
float sdOctahedron(vec3 p, float s);
float sdMandelbulb(vec3 p);
float sdJulia(vec3 p);

float opUnion(float d1, float d2) { return min(d1, d2); };
float opIntersection(float d1, float d2) { return max(d1, d2); };

float opUnionSmooth(float d1, float d2, float k) {
	float h = max(k - abs(d1 - d2), 0.f) / k;
	return min(d1, d2) - pow(h, 3) * k * (1.f / 6.f);
}

void cameraControl(inout vec3 rayOrigin, inout vec3 rayDirection, vec2 m) {
	if(isMouseClicked) {
		rayOrigin.yz *= rotate2D(-m.y);
		rayDirection.yz *= rotate2D(-m.y);
		
		rayOrigin.xz *= rotate2D(-m.x);
		rayDirection.xz *= rotate2D(-m.x);
	}
}

float map(vec3 p) {
	//p.z += time * 0.5f; // Forward movement
	//p.xy = mod(p.xy, 1.f) - 0.5f; // Space repetition
	//p.z = mod(p.z, 0.25f) - 0.125f;

	//float d1 = sdOctahedron(p, 1.f);
	//float d2 = sdSphere(p, 0.5f);

	//return opUnionSmooth(d1, d2, 2.f);

	//float dist = sdJulia(p);
	float dist = sdMandelbulb(p);

	return dist;
}

vec4 mainImage(vec2 fragCoord) {
	vec2 uv = (fragCoord.xy * 2.f - resolution.xy) / resolution.y;
	vec2 m = (cursor * 2.f - resolution.xy) / resolution.y;
	//vec2 m = vec2(cos(time * 0.2f), sin(time * 0.2f));

	vec3 rayOrigin = vec3(0.f, 0.f, -2.f);
	vec3 rayDirection = normalize(vec3(uv, 1.f));
	vec3 color = vec3(0.f);
	float rStep = 0.f;

	// Camera control
	cameraControl(rayOrigin, rayDirection, m);

	bool rayhit = true;

	int i;
	float dist = 0.f;
	for(i = 0; i < 100; i++) {
		vec3 pos = rayOrigin + rayDirection * rStep;
		//pos.xy *= rotate2D(rStep * 0.2f * m.x);
		//pos.y += sin(rStep) * 0.35f;

		dist = map(pos);

		rStep += dist;

		if(dist <= 0.001f) {
			break;
		}

		if(rStep > 100.f) {
			rayhit = false;
			break;
		}
	}

	if(rayhit)
		color = palette(rStep * 0.04f + float(i) * 0.005f, vec3(0.5f), vec3(0.5f), vec3(1.f), vec3(0.263f, 0.416f, 0.557f));
	
	return vec4(color, 1.f);
}

void main() {
	fragColor = mainImage(gl_FragCoord.xy);
}

mat2 rotate2D(float angle) {
	float s = sin(angle);
	float c = cos(angle);
	return mat2(c, -s, s, c);
}

mat3 rotate3D(vec3 axis, float angle) {
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.f - c;

	return mat3(
		oc * axis.x * axis.x + c,
		oc * axis.x * axis.y - axis.z * s,
		oc * axis.z * axis.x + axis.y * s,
		oc * axis.x * axis.y + axis.z * s,
		oc * axis.y * axis.y + c,
		oc * axis.y * axis.z - axis.x * s,
		oc * axis.z * axis.x - axis.y * s,
		oc * axis.y * axis.z + axis.x * s,
		oc * axis.z * axis.z + c
	);
}

// Rodrigues rotation formula
vec3 rotate3D(vec3 p, vec3 axis, float angle) {
	return mix(dot(axis, p) * axis, p, cos(angle)) + cross(axis, p) * sin(angle);
}

float sdSphere(vec3 p, float r) {
	return length(p) - r;
}

float sdBox(vec3 p, vec3 b) {
	vec3 q = abs(p) - b;
	return length(max(q, 0.f)) + min(max(q.x, max(q.y, q.z)), 0.f);
}

float sdOctahedron(vec3 p, float s) {
	p = abs(p);
	return (p.x + p.y + p.z - s) * 0.57735027;
}

float sdPlane(vec3 p, vec3 n, float h) {
	return dot(p, n) + h;
}

float sdMandelbulb(vec3 p) {
    vec3 z = p;
    float dr = 1.f;
	float r = 0.f;
    float power = 8.f * abs(sin(time * 0.1f));  // Adjust for different fractal variations

    for (int i = 0; i < 8; i++) {
        r = length(z);
        if (r > 2.0) break;

        float theta = acos(z.z / r) * power;
        float phi = atan(z.y, z.x) * power;
        float zr = pow(r, power);
		
		dr = pow(r, power - 1.f) * power * dr + 1;

        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) + p;
    }

    return 0.5 * log(r) * r / dr;
}

float sdJulia(vec3 p) {
    vec3 z = p;
    float dr = 1.0;
    float r = 0.0;
    // Number of iterations: adjust for more detail at the cost of performance.
    const int iterations = 12;
    // Using a power of 2 for the Julia set (change this value for variations).
    const float power = 2.0;
    
    for (int i = 0; i < iterations; i++) {
        r = length(z);
        if (r > 2.0) break;
        
        // Update the derivative (for DE calculation)
        dr = 2.0 * r * dr + 1.0;
        
        // Convert to spherical coordinates
        float theta = acos(z.z / r);
        float phi = atan(z.y, z.x);
        // Scale the radius with the power
        float zr = pow(r, power);
        
        // Multiply the angles by the power
        theta *= power;
        phi   *= power;
        
        // Convert back to Cartesian coordinates
        z = zr * vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        // Add the constant Julia offset.
        z += abs(sin(time));
    }
    
    // Estimate distance from the fractal surface.
    return 0.5 * log(r) * r / dr;
}