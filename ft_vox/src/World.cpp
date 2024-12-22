#include "World.hpp"

int World::GenerationThreadFunc(void *data)
{
    World *world = static_cast<World *>(data);
    return world->ProcessGenerationTasks();
}

int World::ProcessGenerationTasks()
{
    while (threadRunning)
    {
        isRunning = true;
        Vec3 currentPos;
        bool shouldUpdate = false;

        SDL_LockMutex(positionMutex);
        if (positionUpdated)
        {
            currentPos = playerPosition;
            positionUpdated = false;
            shouldUpdate = true;
        }
        SDL_UnlockMutex(positionMutex);

        if (shouldUpdate)
        {
            int playerChunkX = Floor(currentPos.x / (float)Chunk::CHUNK_SIZE);
            int playerChunkZ = Floor(currentPos.z / (float)Chunk::CHUNK_SIZE);

            for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++)
            {
                for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++)
                {
                    int chunkX = playerChunkX + x;
                    int chunkZ = playerChunkZ + z;

                    float dist = sqrt(x * x + z * z);
                    if (dist <= RENDER_DISTANCE)
                    {
                        SDL_LockMutex(chunksMutex);
                        if (!getChunk(chunkX, chunkZ))
                        {
                            createChunk(chunkX, chunkZ);
                        }
                        SDL_UnlockMutex(chunksMutex);
                    }
                }
            }
        }
        SDL_Delay(50);
    }

    LogInfo("Generation thread exiting.");
    destory();
    isRunning = false;
    return 0;
}

World::World(unsigned int seed) : noise(seed)
{
    removeChunks.reserve(100);
    threadRunning = true;
    positionUpdated = false;
    chunksMutex = SDL_CreateMutex();
    positionMutex = SDL_CreateMutex();
    lastCleanupTime = SDL_GetTicks();

    generationThread = SDL_CreateThread(GenerationThreadFunc, "ChunkGeneration", this);
}

World::~World()
{
}

void World::close()
{
    threadRunning = false;

    int status;
    SDL_WaitThread(generationThread, &status);

    SDL_DestroyMutex(chunksMutex);
    SDL_DestroyMutex(positionMutex);
}

bool World::IsRunning()
{
    return isRunning;
}

u32 World::getTotalVoxels()
{
    return voxelsTotal;
}

u32 World::getTotalChunks()
{
    return chunksTotal;
}

u32 World::getTotalTris()
{
    return trisTotal;
}

std::string World::generateChunkID(int x, int z)
{
    return std::to_string(x) + "_" + std::to_string(z);
}

Chunk *World::getChunkByID(const std::string &id)
{
    auto it = chunks.find(id);
    return (it != chunks.end()) ? it->second : nullptr;
}

Chunk *World::getChunk(int x, int z)
{
    return getChunkByID(generateChunkID(x, z));
}

void World::destory()
{
    removeChunks.clear();
    for (auto &pair : chunks)
    {
        Chunk *chunk = pair.second;
        chunk->destroy();
        delete chunk;
    }
    chunks.clear();
}

int World::getHeight(int worldX, int worldZ)
{
    return noise.generateHeight(worldX, worldZ);
}

void World::generateChunkTerrain(Chunk *chunk, int chunkX, int chunkZ)
{
    for (int x = 0; x < Chunk::CHUNK_SIZE; x++)
    {
        for (int z = 0; z < Chunk::CHUNK_SIZE; z++)
        {
            int worldX = chunkX * Chunk::CHUNK_SIZE + x;
            int worldZ = chunkZ * Chunk::CHUNK_SIZE + z;

            float baseHeight = noise.noise(worldX * 0.02f, worldZ * 0.02f) * 20;
            float mountainNoise = noise.noise(worldX * 0.01f, worldZ * 0.01f) * 30;
            float detailNoise = noise.noise(worldX * 0.1f, worldZ * 0.1f) * 5;

            int height = 64 + (int)(baseHeight + mountainNoise + detailNoise);
            float temp = noise.noise(worldX * 0.005f, worldZ * 0.005f);

            for (int y = 0; y < height && y < Chunk::CHUNK_HEIGHT; y++)
            {
                BlockType blockType;

                if (y == 0)
                {
                    blockType = BLOCK_STONE;
                }
                else if (y == height - 1)
                {
                    if (height > 90)
                    {
                        blockType = BLOCK_SNOW;
                    }
                    else if (temp > 0.5f)
                    {
                        blockType = BLOCK_SAND;
                    }
                    else
                    {
                        blockType = BLOCK_GRASS;
                    }
                }
                else if (y > height - 4)
                {
                    if (height > 90)
                    {
                        blockType = BLOCK_STONE;
                    }
                    else if (temp > 0.5f)
                    {
                        blockType = BLOCK_SAND;
                    }
                    else
                    {
                        blockType = BLOCK_DIRT;
                    }
                }

                chunk->setBlock(x, y, z, blockType);
            }

            if (height < 60)
            {
                for (int y = height; y < 60; y++)
                {
                    chunk->setBlock(x, y, z, BLOCK_WATER);
                }
            }
        }
    }

    if (rand() % 100 < 20)
    {
        int treeX = rand() % Chunk::CHUNK_SIZE;
        int treeZ = rand() % Chunk::CHUNK_SIZE;
        int treeHeight = 5 + (rand() % 3);

        int groundY = 0;
        for (int y = Chunk::CHUNK_HEIGHT - 1; y >= 0; y--)
        {
            if (chunk->getBlock(treeX, y, treeZ) == BLOCK_GRASS)
            {
                groundY = y;
                break;
            }
        }

        for (int y = 1; y <= treeHeight; y++)
        {
            chunk->setBlock(treeX, groundY + y, treeZ, BLOCK_WOOD);
        }

        for (int y = 0; y < 3; y++)
        {
            for (int x = -2; x <= 2; x++)
            {
                for (int z = -2; z <= 2; z++)
                {
                    if (treeX + x >= 0 && treeX + x < Chunk::CHUNK_SIZE &&
                        treeZ + z >= 0 && treeZ + z < Chunk::CHUNK_SIZE)
                    {
                        chunk->setBlock(treeX + x, groundY + treeHeight + y - 1,
                                        treeZ + z, BLOCK_LEAVES);
                    }
                }
            }
        }
    }
}

void World::createChunk(int x, int z)
{
    std::string id = generateChunkID(x, z);
    if (chunks.find(id) != chunks.end())
        return;

    Chunk *chunk = new Chunk(x, z);
    chunks[id] = chunk;

    connectChunkToNeighbors(chunk, x, z);
    generateChunkTerrain(chunk, x, z);
}

void World::connectChunkToNeighbors(Chunk *chunk, int x, int z)
{
    if (Chunk *north = getChunk(x, z + 1))
    {
        chunk->setNeighbor(Chunk::NORTH, north);
        north->setNeighbor(Chunk::SOUTH, chunk);
    }

    if (Chunk *south = getChunk(x, z - 1))
    {
        chunk->setNeighbor(Chunk::SOUTH, south);
        south->setNeighbor(Chunk::NORTH, chunk);
    }

    if (Chunk *east = getChunk(x + 1, z))
    {
        chunk->setNeighbor(Chunk::EAST, east);
        east->setNeighbor(Chunk::WEST, chunk);
    }

    if (Chunk *west = getChunk(x - 1, z))
    {
        chunk->setNeighbor(Chunk::WEST, west);
        west->setNeighbor(Chunk::EAST, chunk);
    }
}

u32 World::render(const Vec3 &playerPos, Shader &shader, const Frustum &frustum)
{
    trisTotal = 0;
    chunksTotal = 0;
    voxelsTotal = 0;

    int playerChunkX = Floor(playerPos.x / (float)Chunk::CHUNK_SIZE);
    int playerChunkZ = Floor(playerPos.z / (float)Chunk::CHUNK_SIZE);

    SDL_LockMutex(chunksMutex);
    for (auto &pair : chunks)
    {
        Chunk *chunk = pair.second;
        BoundingBox bounds = chunk->getBounds();
        if (!frustum.isBoxInFrustum(bounds))
            continue;

        int dx = chunk->getX() - playerChunkX;
        int dz = chunk->getZ() - playerChunkZ;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist > RENDER_DISTANCE + 50)
        {
            removeChunks.push_back(chunk);
            continue;
        }

        voxelsTotal += chunk->getVoxels();
        chunksTotal++;
        trisTotal += chunk->render(shader);
    }
    SDL_UnlockMutex(chunksMutex);

    return trisTotal;
}

void World::update(const Vec3 &playerPos)
{
    SDL_LockMutex(positionMutex);
    playerPosition = playerPos;
    positionUpdated = true;
    SDL_UnlockMutex(positionMutex);

    Uint32 currentTime = SDL_GetTicks();
    lastPlayerPos = playerPos;

    u32 count = 0;
    if (currentTime - lastCleanupTime > CLEANUP_INTERVAL)
    {
        lastCleanupTime = currentTime;

        if (removeChunks.size() > 5)
        {
            if (count < 5)
            {
                Chunk *chunk = removeChunks.back();
                std::string id = generateChunkID(chunk->getX(), chunk->getZ());
                chunks.erase(id);
                removeChunks.pop_back();
                count++;
                chunk->destroy();
                delete chunk;
            }
        }
    }
}

void World::debug(RenderBatch &batch, const Frustum &frustum)
{
    for (auto &pair : chunks)
    {
        Chunk *chunk = pair.second;
        BoundingBox bounds = chunk->getBounds();
        if (!frustum.isBoxInFrustum(bounds))
            continue;
        chunk->debug(batch);
    }
}

void World::generateInitialChunks()
{
    for (int x = -4; x <= 4; x++)
    {
        for (int z = -4; z <= 4; z++)
        {
            createChunk(x, z);
        }
    }
}

Chunk *World::getChunkAt(int x, int z)
{
    std::string id = generateChunkID(x, z);
    Chunk *chunk = nullptr;
    if (chunks.find(id) != chunks.end())
    {
        LogInfo("ID: %s", id.c_str());
        chunk = chunks[id];
    }
    return chunk;
}

bool World::isBlockSolid(int x, int y, int z)
{
    Chunk *chunk = getChunkAt(x, z);
    if (!chunk)
        return false;

    u8 block = (u8)chunk->getBlock(x & (Chunk::CHUNK_SIZE - 1), y, z & (Chunk::CHUNK_SIZE - 1));
    LogInfo("Block: %d", block);
    return block != 0 && block != 7;
}

bool World::isColliding(const BoundingBox &playerBox)
{
    // Determinar os blocos que precisam ser verificados
    int minX = Floor(playerBox.min.x);
    int maxX = Floor(playerBox.max.x);
    int minY = Floor(playerBox.min.y);
    int maxY = Floor(playerBox.max.y);
    int minZ = Floor(playerBox.min.z);
    int maxZ = Floor(playerBox.max.z);

    BoundingBox blockBox;

    for (int x = minX; x <= maxX; ++x)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            for (int z = minZ; z <= maxZ; ++z)
            {
                Chunk *chunk = getChunk(x / Chunk::CHUNK_SIZE, z / Chunk::CHUNK_SIZE);
                if (chunk)
                {
                    BlockType block = (BlockType)chunk->getBlock(x % Chunk::CHUNK_SIZE, y, z % Chunk::CHUNK_SIZE);
                    if (block != BLOCK_AIR)
                    {
                        // Criar a bounding box do bloco
                        blockBox.set(Vec3(x, y, z), Vec3(x + 1, y + 1, z + 1));
                        if (playerBox.intersects(blockBox))
                        {
                            return true; // Colisão detectada
                        }
                    }
                }
            }
        }
    }

    return false; // Sem colisão
}

void World::Collision(Vec3 &playerPosition, Vec3 &velocity)
{
    BoundingBox playerBox(
        playerPosition + Vec3(-0.4f, 0.0f, -0.4f), // Offset da bounding box
        playerPosition + Vec3(0.4f, 1.8f, 0.4f));  // Altura  (1.8m)
    

    Vec3 nextPosition = playerPosition + velocity; // Próxima posição com base na velocidade
    BoundingBox nextBox (nextPosition + Vec3(-0.4f, 0.0f, -0.4f),nextPosition + Vec3(0.4f, 1.8f, 0.4f));

    // Verificar colisão no eixo X
    if (isColliding(BoundingBox(nextBox.min.x, playerBox.min.y, playerBox.min.z,
                    nextBox.max.x, playerBox.max.y, playerBox.max.z)))
    {
        velocity.x = 0; // Bloquear movimento no eixo X
    }

    // Verificar colisão no eixo Y
    if (isColliding(BoundingBox(playerBox.min.x, nextBox.min.y, playerBox.min.z,
                    playerBox.max.x, nextBox.max.y, playerBox.max.z)))
    {
        velocity.y = 0; // Bloquear movimento no eixo Y
    }

    // Verificar colisão no eixo Z
    if (isColliding(BoundingBox(playerBox.min.x, playerBox.min.y, nextBox.min.z,
                    playerBox.max.x, playerBox.max.y, nextBox.max.z)))
    {
        velocity.z = 0; // Bloquear movimento no eixo Z
    }

    playerPosition += velocity;
}
