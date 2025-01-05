#ifndef _PARTICLE_TEXTURE_H_
#define _PARTICLE_TEXTURE_H_

#include "../../Core Includes/Core.h"
#include "../Texture.h"

class ParticleTexture : public Texture {
private:
	float numOfRows;

public:
	ParticleTexture(std::string fileLocation = "", float numOfRows = 4.f)
		: Texture(fileLocation), numOfRows{ numOfRows } {
	}

	ParticleTexture(const ParticleTexture& source);
	ParticleTexture(ParticleTexture&& source) noexcept;

	ParticleTexture& operator=(const ParticleTexture& rhs);
	ParticleTexture& operator=(ParticleTexture&& rhs) noexcept;

	uint32_t getTextureID() const { return this->textureID; }
	float getNumberOfRows() const { return this->numOfRows; }

	~ParticleTexture() = default;
};

#endif