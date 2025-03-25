#ifndef _CORE_H_
#define _CORE_H_

const unsigned int MAX_POINT_LIGHTS = 3;
const unsigned int MAX_SPOT_LIGHTS = 3;
const unsigned int MAX_CASCADES = 8;

const float PI = 3.14159265f;
const float GRAVITY = -10.f;
const float toRadians = PI / 180;

extern float currTime;
extern float deltaTime;
extern float lastTime;
extern float prevTime;
extern float elapsedTime;

const float near_plane = 5.f;
const float far_plane = 7000.f;

#endif