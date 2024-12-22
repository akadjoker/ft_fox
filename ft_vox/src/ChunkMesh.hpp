
#pragma once
#include <vector>
#include "Config.hpp"

struct Vertex
{
    float x, y, z; // position
    float u, v;    // texture coordinates
    float nx, ny, nz;

    Vertex(float x, float y, float z, float u, float v,float nx=0.0f,float ny=0.0f, float nz=0.0f);
};

class ChunkMesh
{
public:

private:
// Atlas de 64x64 com 4x4 tiles (cada tile é 16x16)
    const float ATLAS_SIZE = 64.0f;   // Tamanho total do atlas
    const float TILES = 4.0f;         // Número de tiles por linha/coluna
    const float TILE_SIZE = 16.0f;    // Tamanho de cada tile em pixels
    const float PADDING = 0.005f;
    
    
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    bool dirty;
    int indexCount;

public:
    ChunkMesh();
    ~ChunkMesh();

    void create();
    void destroy();

    void addTopFace(float x, float y, float z, float atlasX, float atlasY);
    void addBottomFace(float x, float y, float z, float atlasX, float atlasY);
    void addFrontFace(float x, float y, float z, float atlasX, float atlasY);
    void addBackFace(float x, float y, float z, float atlasX, float atlasY);
    void addLeftFace(float x, float y, float z, float atlasX, float atlasY);
    void addRightFace(float x, float y, float z, float atlasX, float atlasY);

    void update();
    void render();
    void clear();
    bool isEmpty() const;

    u32 count() const { return indexCount; }
};