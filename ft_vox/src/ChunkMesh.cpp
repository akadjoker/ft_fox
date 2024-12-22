
#include "ChunkMesh.hpp"

Vertex::Vertex(float x, float y, float z, float u, float v,float nx,float ny, float nz)
    : x(x), y(y), z(z), u(u), v(v), nx(nx), ny(ny), nz(nz)
{
    
}

ChunkMesh::ChunkMesh() : vao(0), vbo(0), ibo(0), dirty(true), indexCount(0)
{
    
}

ChunkMesh::~ChunkMesh()
{
   
}

void ChunkMesh::create()
{
    if ( vao || vbo || ibo)
        destroy();
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBindVertexArray(0);
}

void ChunkMesh::destroy()
{
    if (vao)
        glDeleteVertexArrays(1, &vao);
    if (vbo)
        glDeleteBuffers(1, &vbo);
    if (ibo)
        glDeleteBuffers(1, &ibo);
    vao = vbo = ibo = 0;
}

void ChunkMesh::addTopFace(float x, float y, float z, float atlasX, float atlasY)
{
    unsigned int startIndex = vertices.size();

    float tileU = atlasX / TILES;
    float tileV = atlasY / TILES;
    float tileSizeU = 1.0f / TILES;

    float left = tileU + PADDING;
    float right = tileU + tileSizeU - PADDING;
    float top = tileV + PADDING;
    float bottom = tileV + tileSizeU - PADDING;

    vertices.push_back(Vertex(x, y + 1, z + 1, left, top, 0.0f, 1.0f, 0.0f));      // 0
    vertices.push_back(Vertex(x + 1, y + 1, z + 1, right, top, 0.0f, 1.0f, 0.0f)); // 1
    vertices.push_back(Vertex(x + 1, y + 1, z, right, bottom, 0.0f, 1.0f, 0.0f));  // 2
    vertices.push_back(Vertex(x, y + 1, z, left, bottom, 0.0f, 1.0f, 0.0f));       // 3

    indices.push_back(startIndex);     // 0
    indices.push_back(startIndex + 1); // 1
    indices.push_back(startIndex + 2); // 2
    indices.push_back(startIndex);     // 0
    indices.push_back(startIndex + 2); // 2
    indices.push_back(startIndex + 3); // 3

    dirty = true;
}

void ChunkMesh::addBottomFace(float x, float y, float z, float atlasX, float atlasY)
{
    unsigned int startIndex = vertices.size();

    float tileU = atlasX / TILES;
    float tileV = atlasY / TILES;
    float tileSizeU = 1.0f / TILES;
    float left = tileU + PADDING;
    float right = tileU + tileSizeU - PADDING;
    float top = tileV + PADDING;
    float bottom = tileV + tileSizeU - PADDING;

    vertices.push_back(Vertex(x, y + 1, z, left, top, 0.0f, -1.0f, 0.0f));
    vertices.push_back(Vertex(x + 1, y + 1, z, right, top,0.0f, -1.0f, 0.0f));
    vertices.push_back(Vertex(x + 1, y + 1, z + 1, right, bottom,0.0f, -1.0f, 0.0f));
    vertices.push_back(Vertex(x, y + 1, z + 1, left, bottom,0.0f, -1.0f, 0.0f));

    indices.push_back(startIndex);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);

    dirty = true;
}

void ChunkMesh::addFrontFace(float x, float y, float z, float atlasX, float atlasY)
{
    unsigned int startIndex = vertices.size();

    float tileU = atlasX / TILES;   // Posição U do tile (0 a 1)
    float tileV = atlasY / TILES;   // Posição V do tile (0 a 1)
    float tileSizeU = 1.0f / TILES; // Tamanho do tile em coordenadas UV

    // Adiciona uma pequena margem para evitar bleeding

    float left = tileU + PADDING;
    float right = tileU + tileSizeU - PADDING;
    float top = tileV + PADDING;
    float bottom = tileV + tileSizeU - PADDING;

    // Adiciona os vértices com as coordenadas UV corretas
    vertices.push_back(Vertex(x, y, z + 1, left, top, 0.0f, 0.0f, -1.0f));              
    vertices.push_back(Vertex(x + 1, y, z + 1, right, top, 0.0f, 0.0f, -1.0f));        
    vertices.push_back(Vertex(x + 1, y + 1, z + 1, right, bottom, 0.0f, 0.0f, -1.0f));  
    vertices.push_back(Vertex(x, y + 1, z + 1, left, bottom, 0.0f, 0.0f, -1.0f));       

    indices.push_back(startIndex);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);

    dirty = true;
}

void ChunkMesh::addBackFace(float x, float y, float z, float atlasX, float atlasY)
{
    unsigned int startIndex = vertices.size();

    float tileU = atlasX / TILES;
    float tileV = atlasY / TILES;
    float tileSizeU = 1.0f / TILES;
    float left = tileU + PADDING;
    float right = tileU + tileSizeU - PADDING;
    float top = tileV + PADDING;
    float bottom = tileV + tileSizeU - PADDING;

    vertices.push_back(Vertex(x + 1, y, z, left, top, 0.0f, 0.0f, 1.0f));
    vertices.push_back(Vertex(x, y, z, right, top, 0.0f, 0.0f, 1.0f));
    vertices.push_back(Vertex(x, y + 1, z, right, bottom, 0.0f, 0.0f, 1.0f));
    vertices.push_back(Vertex(x + 1, y + 1, z, left, bottom, 0.0f, 0.0f, 1.0f));

    indices.push_back(startIndex);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);

    dirty = true;
}

void ChunkMesh::addLeftFace(float x, float y, float z, float atlasX, float atlasY)
{
    unsigned int startIndex = vertices.size();
    float tileU = atlasX / TILES;
    float tileV = atlasY / TILES;
    float tileSizeU = 1.0f / TILES;
    float left = tileU + PADDING;
    float right = tileU + tileSizeU - PADDING;
    float top = tileV + PADDING;
    float bottom = tileV + tileSizeU - PADDING;

    vertices.push_back(Vertex(x, y, z, left, top, -1.0f, 0.0f, 0.0f));
    vertices.push_back(Vertex(x, y, z + 1, right, top, -1.0f, 0.0f, 0.0f));
    vertices.push_back(Vertex(x, y + 1, z + 1, right, bottom, -1.0f, 0.0f, 0.0f));
    vertices.push_back(Vertex(x, y + 1, z, left, bottom, -1.0f, 0.0f, 0.0f));
    indices.push_back(startIndex);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);

    dirty = true;
}

void ChunkMesh::addRightFace(float x, float y, float z, float atlasX, float atlasY)
{
    unsigned int startIndex = vertices.size();
    float tileU = atlasX / TILES;
    float tileV = atlasY / TILES;
    float tileSizeU = 1.0f / TILES;
    float left = tileU + PADDING;
    float right = tileU + tileSizeU - PADDING;
    float top = tileV + PADDING;
    float bottom = tileV + tileSizeU - PADDING;

    vertices.push_back(Vertex(x + 1, y, z + 1, left, top, 1.0f, 0.0f, 0.0f));
    vertices.push_back(Vertex(x + 1, y, z, right, top, 1.0f, 0.0f, 0.0f));
    vertices.push_back(Vertex(x + 1, y + 1, z, right, bottom, 1.0f, 0.0f, 0.0f));
    vertices.push_back(Vertex(x + 1, y + 1, z + 1, left, bottom, 1.0f, 0.0f, 0.0f));

    indices.push_back(startIndex);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);

    dirty = true;
}

void ChunkMesh::update()
{
     if(vao == 0 || vbo == 0 || ibo == 0)
     {
        create();
     }   

    if (!dirty || vertices.empty())
        return;

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),indices.data(), GL_DYNAMIC_DRAW);

    indexCount = indices.size();
    dirty = false;
}

void ChunkMesh::render()
{
    if (indexCount == 0)
        return;

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ChunkMesh::clear()
{
    vertices.clear();
    indices.clear();
    indexCount = 0;
    dirty = true;
}

bool ChunkMesh::isEmpty() const
{
    return vertices.empty();
}