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

	int strips = 0, verticesPerStrip = 0;

	std::vector<std::vector<float>> heightMaps;

	void generateIndices();
	void generateTexCoords();

public:
	Terrain(int rows = 0, int cols = 0, int maxAmplitude = 50, int maxPersistence = 10,
		int maxFrequency = 10, int frequencyDivisor = 100, int persistenceDivisor = 100,
		int maxRandomSeed = 10, int maxOctaves = 10);

	void generateHeightMaps(int noOfMaps);
	void genearteTerrainFromHeightMap(const std::string& filePath, float yScale, float yShift);
	void generateTerrain(float scale = 1.f);

	void generateVertexNormals();

	~Terrain() = default;
};

#endif
