#pragma once
#include "Math.hpp"
#include "World.hpp"
#include "Camera.hpp"

struct CollisionResult
{
    bool collided;
    Vec3 position;     // Posição da colisão
    Vec3 normal;       // Normal da superfície de colisão
    float penetration; // Profundidade da penetração
};
struct SphereCollisionResult
{
    bool collided;
    Vec3 normal;
    float penetration;
};

class CollisionSystem
{
private:
    World *world;
    float sphereRadius = 1.5f; // Raio da esfera do jogador

    bool checkSphereVoxelCollision(const Vec3 &center, const Vec3 &voxelPos, SphereCollisionResult &result)
    {
        // Encontra o ponto mais próximo da esfera na box do voxel
        Vec3 closest;
        closest.x = std::max(voxelPos.x, std::min(center.x, voxelPos.x + 1.0f));
        closest.y = std::max(voxelPos.y, std::min(center.y, voxelPos.y + 1.0f));
        closest.z = std::max(voxelPos.z, std::min(center.z, voxelPos.z + 1.0f));

        // Calcula a distância entre o centro da esfera e o ponto mais próximo
        Vec3 diff = center - closest;
        float distSqr = diff.lengthSquared();

        // Verifica colisão
        if (distSqr < sphereRadius * sphereRadius)
        {
            float dist = sqrt(distSqr);
            result.collided = true;
            result.normal = Vec3(diff.x / dist, diff.y / dist, diff.z / dist);
            result.penetration = sphereRadius - dist;
            return true;
        }
        return false;
    }

public:
    CollisionSystem(World *world) : world(world) {}

    // bool checkAndResolveCollision(Camera &camera)
    // {
    //     Vec3 position = camera.getPosition();
    //     Vec3 velocity = camera.getVelocity();
    //     bool hadCollision = false;

    //     // Área de teste
    //     int minX = Floor(position.x - sphereRadius) - 1;
    //     int minY = Floor(position.y - sphereRadius) - 1;
    //     int minZ = Floor(position.z - sphereRadius) - 1;
    //     int maxX = Floor(position.x + sphereRadius) + 1;
    //     int maxY = Floor(position.y + sphereRadius) + 1;
    //     int maxZ = Floor(position.z + sphereRadius) + 1;

    //     // Testa colisão com voxels próximos
    //     for (int y = minY; y <= maxY; y++)
    //     {
    //         for (int x = minX; x <= maxX; x++)
    //         {
    //             for (int z = minZ; z <= maxZ; z++)
    //             {
    //                 int chunkX = x >> 4;
    //                 int chunkZ = z >> 4;

    //                 Chunk *chunk = world->getChunk(chunkX, chunkZ);
    //                 if (!chunk)
    //                     continue;

    //                 int localX = x & 15;
    //                 int localZ = z & 15;

    //                 if (chunk->getBlock(localX, y, localZ) != BLOCK_AIR)
    //                 {
    //                     SphereCollisionResult result;
    //                     if (checkSphereVoxelCollision(position, Vec3(x, y, z), result))
    //                     {
    //                         Vec3 correction = result.normal * result.penetration;
    //                         position = position + correction;

    //                         float dotProduct = velocity.dot(result.normal);
    //                         if (dotProduct < 0)
    //                         {
    //                             velocity = velocity - result.normal * dotProduct;
    //                         }

    //                         // LogInfo("COLLISION: %f %f %f", correction.x, correction.y, correction.z );
    //                         // LogInfo("NORMAL: %f %f %f", result.normal.x, result.normal.y, result.normal.z );
    //                         hadCollision = true;
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     if (hadCollision)
    //     {
    //         camera.setVelocity(velocity); // Atualizar velocidade corrigida
    //         camera.setPosition(position); // Atualizar posição corrigida
    //     }

    //     return hadCollision;
    // }
    bool checkAndResolveCollision(Camera &camera)
    {
        Vec3 position = camera.getPosition();
        Vec3 totalCorrection(0, 0, 0);
        bool hadCollision = false;
        bool groundContact = false; // Nova flag para verificar contato com o chão

        // Área de teste
        int minX = Floor(position.x - sphereRadius) - 1;
        int minY = Floor(position.y - sphereRadius) - 1;
        int minZ = Floor(position.z - sphereRadius) - 1;
        int maxX = Floor(position.x + sphereRadius) + 1;
        int maxY = Floor(position.y + sphereRadius) + 1;
        int maxZ = Floor(position.z + sphereRadius) + 1;

        const int MAX_ITERATIONS = 3;
        int iteration = 0;

        while (iteration < MAX_ITERATIONS)
        {
            bool foundCollision = false;
            Vec3 currentPosition = position + totalCorrection;

            for (int y = minY; y <= maxY; y++)
            {
                for (int x = minX; x <= maxX; x++)
                {
                    for (int z = minZ; z <= maxZ; z++)
                    {
                        int chunkX = x >> 4;
                        int chunkZ = z >> 4;
                        Chunk *chunk = world->getChunk(chunkX, chunkZ);
                        if (!chunk)
                            continue;

                        int localX = x & 15;
                        int localZ = z & 15;

                        if (chunk->getBlock(localX, y, localZ) != BLOCK_AIR)
                        {
                            SphereCollisionResult result;
                            if (checkSphereVoxelCollision(currentPosition, Vec3(x, y, z), result))
                            {
                                Vec3 correction = result.normal * result.penetration;
                                totalCorrection = totalCorrection + correction;
                                hadCollision = true;
                                foundCollision = true;

                                // Verifica se a normal aponta para cima (contato com o chão)
                                if (result.normal.y > 0.5f)
                                { // Threshold para considerar uma superfície como chão
                                    groundContact = true;
                                }
                            }
                        }
                    }
                }
            }

            if (!foundCollision)
                break;
            iteration++;
        }

        if (hadCollision)
        {
            camera.setPosition(position + totalCorrection);

            Vec3 velocity = camera.getVelocity();
            if (velocity.lengthSquared() > 0)
            {
                Vec3 lastNormal = totalCorrection.normalize();
                velocity = velocity - lastNormal * velocity.dot(lastNormal);
                camera.setVelocity(velocity);
            }
        }

        camera.setOnGround(groundContact);

        return hadCollision;
    }

    CollisionResult resolveCollision(const Vec3 &position, const Vec3 &velocity)
    {
        CollisionResult result;
        result.collided = false;
        result.position = position;

        // Calcula a área de teste baseada no raio da esfera
        int minX = Floor(position.x - sphereRadius) - 1;
        int minY = Floor(position.y - sphereRadius) - 1;
        int minZ = Floor(position.z - sphereRadius) - 1;
        int maxX = Floor(position.x + sphereRadius) + 1;
        int maxY = Floor(position.y + sphereRadius) + 1;
        int maxZ = Floor(position.z + sphereRadius) + 1;

        // Testa colisão com todos os voxels próximos
        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                for (int z = minZ; z <= maxZ; z++)
                {
                    // Converte para coordenadas de chunk
                    int chunkX = x >> 4; // Divide por CHUNK_SIZE
                    int chunkZ = z >> 4;

                    Chunk *chunk = world->getChunk(chunkX, chunkZ);
                    if (!chunk)
                        continue;

                    // Converte para coordenadas locais do chunk
                    int localX = x & 15; // Módulo CHUNK_SIZE
                    int localZ = z & 15;

                    if (chunk->getBlock(localX, y, localZ) != BLOCK_AIR)
                    {
                        SphereCollisionResult sphereResult;
                        if (checkSphereVoxelCollision(position, Vec3(x, y, z), sphereResult))
                        {
                            result.collided = true;
                            result.normal = sphereResult.normal;

                            // Resolve a penetração
                            Vec3 correction = sphereResult.normal * sphereResult.penetration;
                            result.position = position + correction;
                            return result;
                        }
                    }
                }
            }
        }

        result.position = position + velocity;
        return result;
    }
};