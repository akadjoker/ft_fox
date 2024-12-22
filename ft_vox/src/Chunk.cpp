
#include "Chunk.hpp"
#include "PerlinNoise.hpp"
#include "Math.hpp"
#include <iostream>

Chunk::Chunk(int x, int z)
    : posX(x), posZ(z),
      worldX(x * CHUNK_SIZE),
      worldZ(z * CHUNK_SIZE), needsUpdate(true)
{
    memset(blocks, 0, sizeof(blocks));
    neighbors[NORTH] = nullptr;
    neighbors[SOUTH] = nullptr;
    neighbors[EAST] = nullptr;
    neighbors[WEST] = nullptr;


    bounds = BoundingBox(
        Vec3(worldX, 0, worldZ),
        Vec3(worldX + CHUNK_SIZE, 1, worldZ + CHUNK_SIZE));
}

Chunk::~Chunk()
{
   
}
void Chunk::updateMesh()
{
    if (!needsUpdate)
        return;
    mesh.clear();

    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                BlockType blockType = (BlockType)blocks[x][y][z];
                if (blockType == BLOCK_AIR)
                    continue;

                // TOP face (Y+1)
                if (y == CHUNK_HEIGHT - 1 || !hasBlockAt(x, y + 1, z))
                {
                    mesh.addTopFace(x, y, z, blockType - 1, 0);
                }

                // BOTTOM face (Y-1)
                if (y == 0 || !hasBlockAt(x, y - 1, z))
                {
                    mesh.addBottomFace(x, y, z, blockType - 1, 0);
                }

                // FRONT face (Z+1)
                if (z == CHUNK_SIZE - 1)
                {
                    // Estamos na borda frontal, verificar chunk vizinho
                    if (!neighbors[NORTH] || neighbors[NORTH]->getBlock(x, y, 0) == BLOCK_AIR)
                    {
                        mesh.addFrontFace(x, y, z, blockType - 1, 0);
                    }
                }
                else if (!hasBlockAt(x, y, z + 1))
                {
                    mesh.addFrontFace(x, y, z, blockType - 1, 0);
                }

                // BACK face (Z-1)
                if (z == 0)
                {
                    // Estamos na borda traseira, verificar chunk vizinho
                    if (!neighbors[SOUTH] || neighbors[SOUTH]->getBlock(x, y, CHUNK_SIZE - 1) == BLOCK_AIR)
                    {
                        mesh.addBackFace(x, y, z, blockType - 1, 0);
                    }
                }
                else if (!hasBlockAt(x, y, z - 1))
                {
                    mesh.addBackFace(x, y, z, blockType - 1, 0);
                }

                // RIGHT face (X+1)
                if (x == CHUNK_SIZE - 1)
                {
                    // Estamos na borda direita, verificar chunk vizinho
                    if (!neighbors[EAST] || neighbors[EAST]->getBlock(0, y, z) == BLOCK_AIR)
                    {
                        mesh.addRightFace(x, y, z, blockType - 1, 0);
                    }
                }
                else if (!hasBlockAt(x + 1, y, z))
                {
                    mesh.addRightFace(x, y, z, blockType - 1, 0);
                }

                // LEFT face (X-1)
                if (x == 0)
                {
                    // Estamos na borda esquerda, verificar chunk vizinho
                    if (!neighbors[WEST] || neighbors[WEST]->getBlock(CHUNK_SIZE - 1, y, z) == BLOCK_AIR)
                    {
                        mesh.addLeftFace(x, y, z, blockType - 1, 0);
                    }
                }
                else if (!hasBlockAt(x - 1, y, z))
                {
                    mesh.addLeftFace(x, y, z, blockType - 1, 0);
                }
            }
        }
    }

    mesh.update();
    needsUpdate = false;
}

u32 Chunk::render(Shader &shader)
{
    updateMesh();
    Mat4 model = Mat4::Translate(Vec3(worldX, 0, worldZ));
    shader.SetMatrix4("model", model.m);
    mesh.render();
    return mesh.count();
}

void Chunk::debug(RenderBatch &batch)
{
    batch.Box(bounds);
}

void Chunk::destroy()
{
    mesh.destroy();
}

void Chunk::setNeighbor(Direction dir, Chunk *neighbor)
{
    // std::cout << "Chunk em " << worldX << "," << worldZ << " defenido vizinho "
    //           << (dir == EAST ? "EAST" : "WEST") << std::endl;
    neighbors[dir] = neighbor;
    needsUpdate = true;
}

void Chunk::setBlock(int x, int y, int z, uint8_t type)
{
    if (!isInBounds(x, y, z))
        return;

    if (type != BLOCK_AIR)
        bounds.merge((Vec3(worldX + x, y + 1, worldZ + z)));

    if (type != BLOCK_AIR)
        voxels++;
    blocks[x][y][z] = type;
    needsUpdate = true;
}
bool Chunk::hasBlockAt(int x, int y, int z) const
{
    if (y < 0 || y >= CHUNK_HEIGHT)
    {
        return false;
    }

    if (x >= 0 && x < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE)
    {
        return blocks[x][y][z] != BLOCK_AIR;
    }

    if (x >= CHUNK_SIZE && neighbors[EAST])
    {
        return neighbors[EAST]->getBlock(0, y, z) != BLOCK_AIR;
    }

    if (x < 0 && neighbors[WEST])
    {
        return neighbors[WEST]->getBlock(CHUNK_SIZE - 1, y, z) != BLOCK_AIR;
    }

    return false;
}
