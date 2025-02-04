#pragma once

#include <cstdlib>
#include "Core.hpp"

#include "PerlinNoise.hpp"

#include <mutex>

#define CHUNK_XY 16
#define CHUNK_Z 256
#define CHUNK_SIZE CHUNK_XY *CHUNK_XY *CHUNK_Z
#define AIR 0
#define GRASS 1
#define STONE 2
#define SAND 3
#define WATER 4

class Chunk
{
public:
	Chunk(Vec2 position, int id);
	~Chunk();
	Vec2 GetPos();
	int GetNbInstances();
	GLuint GetVBOID();
	GLuint GetIBOID();
	GLuint GetTID();
	GLuint GetCID();
	bool isEnabled();
	void Enable();
	void Disable();
	
	void generateDefault(PerlinNoise **mapgen);
	void generateCity(PerlinNoise **mapgen);

	bool isGenerated();
	bool isUsable();
	void Build();
	unsigned char GetCube(int x, int y, int z);
	bool HasFourNeigbors();
	Chunk *left;
	Chunk *right;
	Chunk *front;
	Chunk *back;
	int GetID();
	BoundingBox &getBoundingBox() { return box; }
	bool isCubeVisible(int x, int y, int z);
	void setCube(int x, int y, int z, unsigned char cube);
	bool rayIntersectsVoxel(RenderBatch *batch, const Vec3 &rayOrigin, const Vec3 &rayDir, int &outX, int &outY, int &outZ, float &outDistance);
	bool getVoxelFromScreen(float windowWidth, float windowHeight, int mouseX, int mouseY, const Mat4 &projMatrix, const Mat4 &viewMatrix, int &outX, int &outY, int &outZ);
	bool rayAABBIntersection(const Vec3 &rayOrigin, const Vec3 &rayDir, const Vec3 &boxMin, const Vec3 &boxMax, float &outDistance);
	void reloadChunk();

private:
	BoundingBox box;
	int id;
	bool usable;
	bool reloaded;
	bool generated;
	bool enabled;
	GLuint vboID;
	GLuint iboID;
	GLuint translationsID;
	GLuint cubeID;
	int nbInstances;
	float *vertices;
	unsigned int *indices;
	GLfloat *translations;
	GLuint *cubes;
	const Vec2 position;
	PerlinNoise *mapgen;
	int nb;
	unsigned char ***chunk;
	void calcVertices();
	void setVBO();
	void setIBO();
	void addCubeVertices();
	void addIndices();
	void setTranslationsO();
	void setCubeO(int nb);
};
