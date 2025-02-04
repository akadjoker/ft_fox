#pragma once

#include "Chunk.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <thread>
#include <mutex>
#include "Core.hpp"
#include "PerlinNoise.hpp"

#define FOV 80.0f
#define VIEW_DISTANCE 160.0f * 1.4f + 32.0f

class World
{
public:
	World(Camera *camera,bool is_seed, unsigned int seed);
	~World();
	void MapHandler();
	void LoadChunks();
	std::map<std::string, Chunk *> enabledChunks;
	long getTotalVoxels() { return Voxels; }
	long getTotalCubes() { return Cubes; }
	long getTotalVertices() { return Vertices; }
	void clean();
private:
	long Vertices;
	long Cubes;
	long Voxels;
	Camera *camera;
	std::map<std::string, Chunk *> disabledChunks;
	PerlinNoise *mapgen;
	Chunk *GenerateChunk(int x, int y);
	void GenerateChunks();
	void AddChunkAtPos(int x, int y);
	bool CheckIfChunkAtPos(int x, int y);
	void DisableChunks();
	void RemoveFarChunks();
	Chunk *GetChunkAtPos(int x, int y);
	void CheckNeigbors(Chunk *chunk);
	void debugChunkHole();
	std::thread *t;
};
