#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "../Camera.h"
#include "ParticleTexture.h"

using namespace glm;

class CParticle {
private:
	vec3 position;
	vec3 velocity;
	vec3 color;
	float forceEffect;
	float rotation;
	float scale;
	float lifeTime;
	float distance;

	float elapsedTime = 0.f;
	float delta = 0.f;

	vec2 texOffset1;
	vec2 texOffset2;
	float blend;

	void updateTextureCoordInfo() const {
		float lifeFactor = elapsedTime / lifeTime;
	}

public:
	CParticle(vec3 position = { 0.f, 0.f, 0.f }, vec3 velocity = { 0.f, 0.f, 0.f },
		vec3 color = { 0.1f, 0.5f, 0.5f }, float forceEffect = 0.f, float rotation = 0.f,
		float scale = 1.f, float lifeTime = 1.f);

	vec3 getPosition() const { return this->position; }
	float getRotation() const { return this->rotation; }
	float getScale() const { return this->scale; }
	vec3 getColor() const { return this->color; }
	float getElapsedTime() const { return this->elapsedTime; }
	float getLifeTime() const { return this->lifeTime; }
	void updateDelta(float delta) { this->delta = delta; }
	float getDistance() const { return this->distance; }
	vec3 getVelocity() const { return this->velocity; }
	float getForceEffect() const { return this->forceEffect; }

	float getBlend() const { return this->blend; }
	vec2 getTexOffset1() const { return this->texOffset1; }
	vec2 getTexOffset2() const { return this->texOffset2; }
	bool update(vec3 cameraPosition);

	~CParticle() = default;
};

#endif