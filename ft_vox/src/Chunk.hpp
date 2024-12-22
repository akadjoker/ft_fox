#pragma once
#include "Config.hpp"
#include "ChunkMesh.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
enum BlockType {
    BLOCK_AIR = 0,  
    BLOCK_GRASS = 1,
    BLOCK_DIRT = 2, 
    BLOCK_STONE = 3,
    BLOCK_SNOW = 4,
    BLOCK_SAND = 5,
    BLOCK_WOOD = 6,  // Tronco de árvore
    BLOCK_LEAVES = 7, // Folhas
    BLOCK_WATER = 8,
    BLOCK_LAVA = 9,
    BLOCK_GRAVEL = 10,
    BLOCK_ORE = 11   // Minério
};

// Estrutura simples para mapeamento de texturas
struct BlockTexture
{
    int top;    // índice x da textura para face superior
    int side;   // índice x da textura para faces laterais
    int bottom; // índice x da textura para face inferior
};

// Array com os mapeamentos de textura para cada tipo de bloco
const BlockTexture BLOCK_TEXTURES[] = {
      {0, 0, 0},      // BLOCK_AIR
    {0, 1, 2},      // BLOCK_GRASS (top verde, side grama, bottom terra)
    {2, 2, 2},      // BLOCK_DIRT
    {3, 3, 3},      // BLOCK_STONE
    {4, 4, 4},      // BLOCK_SNOW
    {5, 5, 5},      // BLOCK_SAND
    {6, 6, 6},      // BLOCK_WOOD
    {7, 7, 7},      // BLOCK_LEAVES
    {8, 8, 8},      // BLOCK_WATER
    {9, 9, 9},      // BLOCK_LAVA
    {10, 10, 10},   // BLOCK_GRAVEL
    {11, 11, 11}    // BLOCK_ORE
};

// Função auxiliar para obter o índice de textura correto
inline int GetBlockTextureIndex(BlockType type, bool isTop, bool isBottom)
{
    if (type == BLOCK_AIR)
        return 0;

    const BlockTexture &tex = BLOCK_TEXTURES[type];
    if (isTop)
        return tex.top;
    if (isBottom)
        return tex.bottom;
    return tex.side;
}
class Chunk
{
public:
    static const int CHUNK_SIZE = 16;    // X e Z
    static const int CHUNK_HEIGHT = 256; // Y
    static const int TOTAL_BLOCKS = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;

    enum Direction
    {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

private:
    uint8_t blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    ChunkMesh mesh;
    int posX, posZ;
    int worldX, worldZ;  
    bool needsUpdate;
    Chunk *neighbors[4];
    BoundingBox bounds;
    u32 voxels = 0;    

public:
    Chunk(int x, int z);
    ~Chunk();

    int getWorldX() const { return worldX; }
    int getWorldZ() const { return worldZ; }

    uint8_t getBlock(int x, int y, int z) const
    {
        if (!isInBounds(x, y, z))
            return 0;
        return blocks[x][y][z];
    }

    BoundingBox getBounds() const { return bounds; }

    void setNeighbor(Direction dir, Chunk *neighbor);

    Chunk *getNeighbor(Direction dir) const
    {
        return neighbors[dir];
    }

    u32 getVoxels() const { return voxels; }

    void setBlock(int x, int y, int z, uint8_t type);

    bool hasBlockAt(int x, int y, int z) const;

    void updateMesh();
    u32 render(Shader &shader);
    void debug(RenderBatch &batch);

    int getX() const { return posX; }
    int getZ() const { return posZ; }

    void destroy();

private:
    bool isInBounds(int x, int y, int z) const
    {
        return x >= 0 && x < CHUNK_SIZE &&
               y >= 0 && y < CHUNK_HEIGHT &&
               z >= 0 && z < CHUNK_SIZE;
    }
};