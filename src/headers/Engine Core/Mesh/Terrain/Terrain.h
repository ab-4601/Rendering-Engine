#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#pragma once

#include "../Mesh.h"
#include "PerlinNoise.h"

class Terrain : public Mesh {
private:
	int rows, cols;
	float scale;

	PerlinNoise noise;

	int maxAmplitude, maxPersistence, maxFrequency;
	int frequencyDivisor, persistenceDivisor;
	int maxRandomSeed, maxOctaves;

	std::vector<std::vector<float>> heightMaps;

	void generateIndices();
	void generateTexCoords();

public:
	Terrain(int rows = 0, int cols = 0, int maxAmplitude = 50, int maxPersistence = 10,
		int maxFrequency = 10, int frequencyDivisor = 100, int persistenceDivisor = 100,
		int maxRandomSeed = 10, int maxOctaves = 10);

	void generateHeightMaps(int noOfMaps);
	void generateTerrain(float scale = 1.f);

	inline void setScale(float scale) { this->scale = scale; }

	void generateVertexNormals();

	~Terrain() = default;
};

#endif
