#pragma once

#include "Chunk.hpp"
#include <vector>
#include <map>
#include <iostream>
#include <thread>
#include <mutex>
#include "Core.hpp"
#include "PerlinNoise.hpp"

    //"In the open, minimal distance render will be 160 cubes."
#define FOV 80.0f
#define VIEW_DISTANCE 160.0f * 2.0f// 1.4f + 32.0f

struct RayPickResult
{
	Chunk *chunk;
	int voxelX;
	int voxelY;
	int voxelZ;
	float distance;
	bool found;

	RayPickResult() : chunk(nullptr), voxelX(0), voxelY(0), voxelZ(0), distance(0), found(false) {}
};

class World
{
public:
	World(Camera *camera, bool is_seed, unsigned int seed);
	~World();
	void MapHandler();
	void LoadChunks();
	Chunk *GetChunkAtPos(int x, int y);
	std::map<std::string, Chunk *> enabledChunks;
	long getTotalVoxels() { return Voxels; }
	long getTotalCubes() { return Cubes; }
	long getTotalVertices() { return Vertices; }
	void clean();
	void switchMode() { isCity = !isCity; }
	bool isCityMode() { return isCity; }
    RayPickResult getChunkAndVoxelFromScreen(RenderBatch *batch,  float windowWidth, float windowHeight,int mouseX, int mouseY);
private:
	long Vertices;
	long Cubes;
	long Voxels;
	Camera *camera;
	bool isCity;

	std::map<std::string, Chunk *> disabledChunks;
	PerlinNoise *mapgen;
	 Vec3 screenToWorldRay(RenderBatch *batch,float windowWidth, float windowHeight,int mouseX, int mouseY);
	Chunk *GenerateChunk(int x, int y);
	void GenerateChunks();
	void AddChunkAtPos(int x, int y);
	bool CheckIfChunkAtPos(int x, int y);
	void DisableChunks();
	void RemoveFarChunks();
	void CheckNeigbors(Chunk *chunk);
	void debugChunkHole();
};
