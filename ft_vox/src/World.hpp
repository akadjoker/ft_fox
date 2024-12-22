#pragma once
#include "Config.hpp"
#include "Chunk.hpp"
#include "PerlinNoise.hpp"
#include "Batch.hpp"
#include <unordered_map>
#include <vector>
#include <string>

struct ChunkTask
{
    int x;
    int z;
    bool isGeneration;
};

class World
{
private:
    std::unordered_map<std::string, Chunk *> chunks;
    std::vector<Chunk *> removeChunks;
    PerlinNoise noise;

    u32 trisTotal = 0;
    u32 chunksTotal = 0;
    u32 voxelsTotal = 0;
    static const int RENDER_DISTANCE = 10;
    Vec3 lastPlayerPos;

    // Threading
    SDL_Thread *generationThread;
    SDL_mutex *chunksMutex;
    SDL_mutex *positionMutex;
    bool threadRunning;
    Vec3 playerPosition;
    bool positionUpdated;
    bool isRunning = true;

    Uint32 lastCleanupTime;
    static const Uint32 CLEANUP_INTERVAL = 5000;

    static int GenerationThreadFunc(void *data);
    int ProcessGenerationTasks();

public:
    World(unsigned int seed = 12345);
    ~World();

    void close();
    bool IsRunning();
    u32 getTotalVoxels();
    u32 getTotalChunks();
    u32 getTotalTris();
    std::string generateChunkID(int x, int z);
    Chunk *getChunkByID(const std::string &id);
    Chunk *getChunk(int x, int z);
    void destory();
    int getHeight(int worldX, int worldZ);
    void generateChunkTerrain(Chunk *chunk, int chunkX, int chunkZ);
    void createChunk(int x, int z);
    void connectChunkToNeighbors(Chunk *chunk, int x, int z);
    u32 render(const Vec3 &playerPos, Shader &shader, const Frustum &frustum);
    void update(const Vec3 &playerPos);
    void debug(RenderBatch &batch, const Frustum &frustum);
    void generateInitialChunks();
    bool isBlockSolid(int x, int y, int z);
    Chunk *getChunkAt(int x, int z);
    bool isColliding(const BoundingBox &playerBox);
    void Collision(Vec3 &playerPosition, Vec3 &velocity);
};

inline float RandFloat(float min, float max)
{
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

inline int RandInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

class CloudSystem
{
private:
    struct CloudInstance
    {
        Vec3 position;
    };

    static const int MAX_CLOUDS = 2000;
    GLuint instanceVBO;
    GLuint cubeVAO, cubeVBO, EBO;
    std::vector<CloudInstance> instances;
    std::vector<float> instanceData;
    PerlinNoise noise;
    float baseHeight = 120.0f;

    void setupCube()
    {
        float vertices[] = {
            -0.5f, -0.25f, 0.5f, // 0
            0.5f, -0.25f, 0.5f,  // 1
            0.5f, 0.25f, 0.5f,   // 2
            -0.5f, 0.25f, 0.5f,  // 3
            // Trás
            -0.5f, -0.25f, -0.5f, // 4
            0.5f, -0.25f, -0.5f,  // 5
            0.5f, 0.25f, -0.5f,   // 6
            -0.5f, 0.25f, -0.5f   // 7
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
            // Trás
            5, 4, 7,
            7, 6, 5,
            // Topo
            3, 2, 6,
            6, 7, 3,
            // Base
            4, 5, 1,
            1, 0, 4,
            // Direita
            1, 5, 6,
            6, 2, 1,
            // Esquerda
            4, 0, 3,
            3, 7, 4};
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &instanceVBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);
    }

    void updateInstanceData()
    {
        instanceData.clear();
        for (const auto &instance : instances)
        {
            instanceData.push_back(instance.position.x);
            instanceData.push_back(instance.position.y);
            instanceData.push_back(instance.position.z);
        }

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_DYNAMIC_DRAW);
     //   LogInfo("[CLOUD] Updated instance data %d", instanceData.size());
    }

public:
    CloudSystem(unsigned int seed) : noise(seed)
    {
        setupCube();

        instanceData.reserve(1000);
        instances.reserve(1000);
    }

    void render()
    {
        glBindVertexArray(cubeVAO);
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instances.size());
    }

    void destroy()
    {
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &instanceVBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &cubeVAO);
    }
    void update(const Vec3 &playerPos)
    {
        for (int i = instances.size() - 1; i >= 0; i--)
        {
            if (Vec3::Distance(instances[i].position, playerPos) > 500.0f)
            {
                instances.erase(instances.begin() + i);
            }
        }

        int regionX = Floor(playerPos.x / (float)Chunk::CHUNK_SIZE);
        int regionZ = Floor(playerPos.z / (float)Chunk::CHUNK_SIZE);

        for (int x = -8; x <= 8; ++x)
        {
            for (int z = -8; z <= 8; ++z)
            {
                float worldX = (regionX + x) * 16.0f;
                float worldZ = (regionZ + z) * 16.0f;
                float density = noise.noise(worldX * 0.1f, worldZ * 0.1f);

                int per = RandInt(0, 100);
                if (per > 80)
                {

                    if (density > 0.1f && instances.size() < MAX_CLOUDS) // Threshold reduzido
                    {

                        CloudInstance centerCloud;
                        centerCloud.position = Vec3(worldX, baseHeight, worldZ);
                        instances.push_back(centerCloud);

                        int count = RandInt(0, 4);
                        switch (count)
                        {
                        case 0:
                        {
                            instances.push_back(CloudInstance{Vec3(worldX + 2.0f, baseHeight, worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX + 1.0f, baseHeight, worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX - 1.0f, baseHeight, worldZ - 1.0f)});
                            break;
                        }
                        case 1:
                        {
                            instances.push_back(CloudInstance{Vec3(worldX + 2.0f, baseHeight, worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX + 1.0f, baseHeight, worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX - 1.0f, baseHeight, worldZ - 1.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX - 2.0f, baseHeight, worldZ)});
                            break;
                        }

                        case 2:
                        {
                            instances.push_back(CloudInstance{Vec3(worldX + 3.0f, baseHeight, worldZ - 2.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX + 2.0f, baseHeight, worldZ - 2.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX + 1.0f, baseHeight, worldZ - 2.0f)});

                            instances.push_back(CloudInstance{Vec3(worldX + 3.0f, baseHeight, worldZ - 1.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX + 2.0f, baseHeight, worldZ + 1.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX + 1.0f, baseHeight, worldZ - 1.0f)});

                            instances.push_back(CloudInstance{Vec3(worldX + 3.0f, baseHeight, worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX + 2.0f, baseHeight, worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX + 1.0f, baseHeight, worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX - 1.0f, baseHeight, worldZ - 1.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX - 2.0f, baseHeight, worldZ - 1.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX - 3.0f, baseHeight, worldZ - 1.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX - 4.0f, baseHeight, worldZ)});
                            break;
                        }
                        case 3: // Nuvem em forma de U
                        {
                             const int gridSize = 5;
                            for (int dx = -gridSize/2; dx <= gridSize/2; ++dx)
                            {
                                for (int dz = -gridSize/2; dz <= gridSize/2; ++dz)
                                {
                                   float spreadVar = RandFloat(-0.8f, 0.8f);
                                    
                                    instances.push_back(CloudInstance{
                                        Vec3(
                                            worldX + dx * 2.0f * spreadVar, 
                                            baseHeight , 
                                            worldZ + dz * 2.0f * spreadVar
                                        )
                                    });
                                }
                            }
                            break;
                        }
                        case 4: // Nuvem em diagonal
                        {
                            instances.push_back(CloudInstance{Vec3(worldX + 2.0f, baseHeight + RandFloat(0.5f, 1.5f), worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX - 2.0f, baseHeight + RandFloat(0.5f, 1.5f), worldZ)});
                            instances.push_back(CloudInstance{Vec3(worldX, baseHeight + RandFloat(1.0f, 2.0f), worldZ + 2.0f)});
                            instances.push_back(CloudInstance{Vec3(worldX, baseHeight + RandFloat(1.0f, 2.0f), worldZ - 2.0f)});
                            
                            // Bordas mais dispersas
                            for (int i = 0; i < 6; ++i)
                            {
                                float angle = RandFloat(0, 6.28f); // 2π
                                float radius = RandFloat(3.0f, 5.0f);
                                instances.push_back(CloudInstance{
                                    Vec3(
                                        worldX + radius * cos(angle), 
                                        baseHeight + RandFloat(0.0f, 1.0f), 
                                        worldZ + radius * sin(angle)
                                    )
                                });
                            }
                            break;
                        }
                  
                        }
                    }
                }
            }
        }

        updateInstanceData();
    }
};
