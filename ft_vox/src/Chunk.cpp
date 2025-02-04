#include "Chunk.hpp"
#include <iostream>

Chunk::Chunk(Vec2 position, int id) : position(position)
{
	this->id = id;
	// box.merge(Vec3(position.x, 0, position.y));
	// box = BoundingBox(position.x, 0,position.y, position.x + CHUNK_XY,  CHUNK_Z, position.y + CHUNK_XY);
	glGenBuffers(1, &vboID);
	glGenBuffers(1, &iboID);
	glGenBuffers(1, &translationsID);
	glGenBuffers(1, &cubeID);
	nbInstances = 0;
	chunk = new unsigned char **[CHUNK_XY];
	for (int i = 0; i < CHUNK_XY; i++)
	{
		chunk[i] = new unsigned char *[CHUNK_XY];
		for (int j = 0; j < CHUNK_XY; j++)
			chunk[i][j] = new unsigned char[CHUNK_Z];
	}
	vertices = new float[8 * 3];
	indices = new unsigned int[12 * 3];
	translations = new GLfloat[CHUNK_SIZE * 3];
	cubes = new GLuint[CHUNK_SIZE];
	enabled = true;
	nb = 0;
	generated = false;
	usable = false;
	left = NULL;
	right = NULL;
	front = NULL;
	back = NULL;
	reloaded = false;
}

Chunk::~Chunk()
{
	if (vboID == 0)
		return;
	delete[] (vertices);
	delete[] (indices);
	delete[] (translations);
	delete[] (cubes);
	for (int i = 0; i < CHUNK_XY; i++)
	{
		for (int j = 0; j < CHUNK_XY; j++)
			delete[] (chunk[i][j]);
		delete[] (chunk[i]);
	}
	delete[] (chunk);
	glDeleteBuffers(1, &vboID);
	glDeleteBuffers(1, &iboID);
	glDeleteBuffers(1, &translationsID);
	glDeleteBuffers(1, &cubeID);
	vboID = 0;
}



void	Chunk::generateDefault(PerlinNoise **mapgen)
{
	int				x;
	int				y;
	int				z;
	int				power;
	float			p;
	float			cave;
	float			height;
	float			width;
	float			biome;
	unsigned char	cube;

	x = 0;
	while (x < CHUNK_XY)
	{
		y = 0;
		while (y < CHUNK_XY)
		{
			biome = (*mapgen)->noise((float)(x + this->position.x) / 350.0f + 0.5f, (float)(y + this->position.y) / 350.0f + 0.5f, 0.0f);
			if (biome < 0.0f)
			{
				p = (*mapgen)->octave((float)(x + this->position.x) / 30.0f + 0.5f, (float)(y + this->position.y) / 30.0f + 0.5f, 0.0f, 2, 1.0f);
				cube = GRASS;
			}
			else if (biome >= 0.00f && biome < 0.20f)
			{
				p = (*mapgen)->octave((float)(x + this->position.x) / 30.0f + 0.5f, (float)(y + this->position.y) / 30.0f + 0.5f, 0.0f, 2, 1.0f);
				cube = SAND;
			}
			else
			{
				p = ((*mapgen)->octave((float)(x + this->position.x) / 60.0f + 0.5f, (float)(y + this->position.y) / 60.0f + 0.5f, 0.0f, 2, 220.0f) + 0.25f) * 3.0f;
				cube = GRASS;
				if (p < 0.15f && p >= -0.10f)
					cube = SAND;
				else if (p < -0.10f)
				{
					cube = WATER;
					p = -0.10f;
				}
			}
			cave = (*mapgen)->BM3D((float)(x + this->position.x + 1000) / 30.0f + 0.5f, (float)(y + this->position.y + 1001) / 30.0f + 0.5f, 0.0f);
			height = 1.0f + (*mapgen)->BM3D((float)(x + this->position.x + 2000) / 30.0f + 0.5f, (float)(y + this->position.y + 2001) / 30.0f + 0.5f, 0.0f);
			width = 1.0f + (*mapgen)->BM3D((float)(x + this->position.x + 3000) / 30.0f + 0.5f, (float)(y + this->position.y + 3001) / 30.0f + 0.5f, 0.0f);
			power = (int)(13.0f * p) + 128;
			z = 0;
			while (z < power && z < CHUNK_Z)
			{
				if (z != power - 1)
					chunk[x][y][z] = STONE;
				else
					chunk[x][y][z] = cube;
				z++;
			}
			while (z < CHUNK_Z)
			{
				chunk[x][y][z] = AIR;
				z++;
			}
			if (cave > 0.40f)
			{
				for (int q = 0; q < (int)(width * 13.0f + 3.0f); q++)
					chunk[x][y][(int)(height * 55.0f + 30.0f) + q] = AIR;
			}
			y++;
		}
		x++;
	}
	calcVertices();
	generated = true;
}
void Chunk::generateCity(PerlinNoise **mapgen)
{
	int x, y, z, power;
	float p, cave, height, width;
	unsigned char cube;

	const float BIOME_SCALE = 600.0f;
	const float TERRAIN_SCALE = 100.0f;
	const float MOUNTAIN_SCALE = 40.0f;
	const float CAVE_THRESHOLD = 0.35f;

	x = 0;
	while (x < CHUNK_XY)
	{
		y = 0;
		while (y < CHUNK_XY)
		{
			float terrainType = (*mapgen)->noise(
				(float)(x + this->position.x) / (TERRAIN_SCALE * 2) + 0.5f,
				(float)(y + this->position.y) / (TERRAIN_SCALE * 2) + 0.5f,
				0.0f);

			float biome = (*mapgen)->noise(
				(float)(x + this->position.x) / BIOME_SCALE + 0.5f,
				(float)(y + this->position.y) / BIOME_SCALE + 0.5f,
				0.0f);

			// Base (flatter)
			float baseHeight = (*mapgen)->octave(
				(float)(x + this->position.x) / TERRAIN_SCALE + 0.5f,
				(float)(y + this->position.y) / TERRAIN_SCALE + 0.5f,
				0.0f,
				2,
				0.8f);

			float mountainHeight = (*mapgen)->octave(
				(float)(x + this->position.x) / MOUNTAIN_SCALE + 0.5f,
				(float)(y + this->position.y) / MOUNTAIN_SCALE + 0.5f,
				0.0f,
				4,
				4.0f);

			float peakNoise = (*mapgen)->noise(
				(float)(x + this->position.x) / (MOUNTAIN_SCALE * 0.5f) + 0.5f,
				(float)(y + this->position.y) / (MOUNTAIN_SCALE * 0.5f) + 0.5f,
				0.0f);

			if (terrainType > 0.3f)
			{

				p = mountainHeight;
				if (peakNoise > 0.6f)
				{ // height for peaks
					p *= 1.5f;
				}
				cube = GRASS;
			}
			else
			{
				p = baseHeight;
				if (biome < 0.0f)
				{
					cube = GRASS;
				}
				else if (biome < 0.20f)
				{
					cube = SAND;
				}
				else
				{
					if (p < 0.15f && p >= -0.10f)
						cube = SAND;
					else if (p < -0.10f)
					{
						cube = WATER;
						p = -0.10f;
					}
					else
						cube = GRASS;
				}
			}

			// Cave generation
			cave = (*mapgen)->BM3D(
				(float)(x + this->position.x + 1000) / MOUNTAIN_SCALE + 0.5f,
				(float)(y + this->position.y + 1001) / MOUNTAIN_SCALE + 0.5f,
				0.0f);

			height = 0.8f + (*mapgen)->BM3D(
								(float)(x + this->position.x + 2000) / MOUNTAIN_SCALE + 0.5f,
								(float)(y + this->position.y + 2001) / MOUNTAIN_SCALE + 0.5f,
								0.0f) *
								0.4f;

			width = 0.8f + (*mapgen)->BM3D(
							   (float)(x + this->position.x + 3000) / MOUNTAIN_SCALE + 0.5f,
							   (float)(y + this->position.y + 3001) / MOUNTAIN_SCALE + 0.5f,
							   0.0f) *
							   0.4f;

			float heightMod = terrainType > 0.3f ? 25.0f : 8.0f; //  mountain multiplier
			power = (int)(heightMod * p) + 128;

			// Extra height boost for peaks
			if (terrainType > 0.3f && peakNoise > 0.6f)
			{
				power += 20; // Add extra height to peaks
			}

			// Terrain generation
			z = 0;
			while (z < power && z < CHUNK_Z)
			{
				if (z != power - 1)
					chunk[x][y][z] = STONE;
				else
					chunk[x][y][z] = cube;
				z++;
			}
			while (z < CHUNK_Z)
			{
				chunk[x][y][z] = AIR;
				z++;
			}

			// Cave generation
			if (cave > CAVE_THRESHOLD)
			{
				for (int q = 0; q < (int)(width * 10.0f + 2.0f); q++)
					chunk[x][y][(int)(height * 45.0f + 30.0f) + q] = AIR;
			}

			y++;
		}
		x++;
	}

	calcVertices();
	generated = true;
}

bool Chunk::isGenerated()
{
	return generated;
}

bool Chunk::isUsable()
{
	return usable;
}

void Chunk::Build()
{
	setTranslationsO();
	setCubeO(nbInstances);
	setVBO();
	setIBO();
	usable = true;
}

void Chunk::reloadChunk()
{
	int x;
	int y;
	int z;

	nbInstances = 0;
	nb = 0;
	x = 0;
	while (x < CHUNK_XY)
	{
		y = 0;
		while (y < CHUNK_XY)
		{
			z = 0;
			while (z < CHUNK_Z)
			{
				if (chunk[x][y][z] > 0)
				{
					if (isCubeVisible(x, y, z))
					{
						translations[nb] = x + this->position.x;
						translations[nb + 2] = y + this->position.y;
						translations[nb + 1] = z;
						cubes[nbInstances] = chunk[x][y][z];
						box.merge(Vec3(x + this->position.x, z, y + this->position.y));
						nbInstances++;
						nb += 3;
					}
				}
				z++;
			}
			y++;
		}
		x++;
	}
	Build();
	reloaded = true;
}

void Chunk::setTranslationsO()
{
	glBindBuffer(GL_ARRAY_BUFFER, translationsID);
	glBufferData(GL_ARRAY_BUFFER, nb * sizeof(GLfloat), translations, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::setCubeO(int nb)
{
	glBindBuffer(GL_ARRAY_BUFFER, cubeID);
	glBufferData(GL_ARRAY_BUFFER, nb * sizeof(GLuint), cubes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::setVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::setIBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::addCubeVertices()
{
	float cube[] = {
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f};
	int i;

	i = 0;
	while (i < 8 * 3)
	{
		vertices[i] = cube[i];
		vertices[i + 1] = cube[i + 1];
		vertices[i + 2] = cube[i + 2];
		i += 3;
	}
}

void Chunk::addIndices()
{
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
		1, 5, 6,
		6, 2, 1,
		7, 6, 5,
		5, 4, 7,
		4, 0, 3,
		3, 7, 4,
		4, 5, 1,
		1, 0, 4,
		3, 2, 6,
		6, 7, 3};
	int i;

	i = 0;
	while (i < 12 * 3)
	{
		this->indices[i] = indices[i];
		this->indices[i + 1] = indices[i + 1];
		this->indices[i + 2] = indices[i + 2];
		i += 3;
	}
}

void Chunk::calcVertices()
{
	addCubeVertices();
	addIndices();
}

bool Chunk::isCubeVisible(int x, int y, int z)
{
	int nbSide;

	nbSide = 0;
	if (z == CHUNK_Z - 1)
		return true;
	if (x == 0 && !left)
		return true;
	if (x == CHUNK_XY - 1 && !right)
		return true;
	if (y == 0 && !back)
		return true;
	if (y == CHUNK_XY - 1 && !front) // top
		return true;
	if (z < CHUNK_Z - 1 && chunk[x][y][z + 1] > 0)
		nbSide++;
	if (z > 0 && chunk[x][y][z - 1] > 0) // bottom 1
		nbSide++;
	if (x > 0 && chunk[x - 1][y][z] > 0) // left 1
		nbSide++;
	if (x < CHUNK_XY - 1 && chunk[x + 1][y][z] > 0) // right 1
		nbSide++;
	if (y > 0 && chunk[x][y - 1][z] > 0) // back 1
		nbSide++;
	if (y < CHUNK_XY - 1 && chunk[x][y + 1][z] > 0) // front 1
		nbSide++;
	if (x == 0 && left && left->GetCube(CHUNK_XY - 1, y, z) > 0) // left 2
		nbSide++;
	if (x == CHUNK_XY - 1 && right && right->GetCube(0, y, z) > 0) // right 2
		nbSide++;
	if (y == 0 && front && front->GetCube(x, CHUNK_XY - 1, z) > 0) // back 2
		nbSide++;
	if (y == CHUNK_XY - 1 && back && back->GetCube(x, 0, z) > 0) // front 2
		nbSide++;
	if (nbSide == 6 || (z == 0 && nbSide == 5))
		return false;
	return true;
}

void Chunk::setCube(int x, int y, int z, unsigned char cube)
{
	if (x < 0 || x >= CHUNK_XY || y < 0 || y >= CHUNK_XY || z < 0 || z >= CHUNK_Z)
		return;
	chunk[x][y][z] = cube;
}

unsigned char Chunk::GetCube(int x, int y, int z)
{
	if (x < 0 || x >= CHUNK_XY || y < 0 || y >= CHUNK_XY || z < 0 || z >= CHUNK_Z)
		return 0;
	else
		return chunk[x][y][z];
}

Vec2 Chunk::GetPos()
{
	return position;
}

int Chunk::GetNbInstances()
{
	return nbInstances;
}

GLuint Chunk::GetVBOID()
{
	return vboID;
}

GLuint Chunk::GetIBOID()
{
	return iboID;
}

GLuint Chunk::GetTID()
{
	return translationsID;
}

GLuint Chunk::GetCID()
{
	return cubeID;
}

bool Chunk::isEnabled()
{
	return enabled;
}

void Chunk::Enable()
{
	enabled = true;
}

void Chunk::Disable()
{
	enabled = false;
}

bool Chunk::HasFourNeigbors()
{
	if (left && right && front && back)
	{
		if (!reloaded)
			reloadChunk();
		return true;
	}
	return false;
}

int Chunk::GetID()
{
	return id;
}

bool Chunk::rayAABBIntersection(const Vec3 &rayOrigin, const Vec3 &rayDir, const Vec3 &boxMin, const Vec3 &boxMax, float &outDistance)
{
	Vec3 invDir = Vec3(1.0f / rayDir.x, 1.0f / rayDir.y, 1.0f / rayDir.z);

	float t1 = (boxMin.x - rayOrigin.x) * invDir.x;
	float t2 = (boxMax.x - rayOrigin.x) * invDir.x;
	float t3 = (boxMin.y - rayOrigin.y) * invDir.y;
	float t4 = (boxMax.y - rayOrigin.y) * invDir.y;
	float t5 = (boxMin.z - rayOrigin.z) * invDir.z;
	float t6 = (boxMax.z - rayOrigin.z) * invDir.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0 || tmin > tmax)
		return false;

	outDistance = tmin;
	return true;
}
bool Chunk::rayIntersectsVoxel(RenderBatch *batch, const Vec3 &rayOrigin, const Vec3 &rayDir, int &outX, int &outY, int &outZ, float &outDistance)
{
    float minDist = std::numeric_limits<float>::max();
    bool hit = false;

 
    Vec3 chunkPos(position.x * CHUNK_XY, 0, position.y * CHUNK_XY);

	
 
    for (int x = 0; x < CHUNK_XY; x++)
    {
        for (int y = 0; y < CHUNK_XY; y++)
        {
            for (int z = 0; z < CHUNK_Z; z++)
            {
             
                if (GetCube(x, y, z) == AIR)
                    continue;

          
                Vec3 voxelMin = chunkPos + Vec3(x, y, z);
                Vec3 voxelMax = voxelMin + Vec3(1, 1, 1); // Cada voxel tem 1x1x1 de tamanho

			//	printf("VoxelMin: %f, %f, %f, VoxelMax: %f, %f, %f\n", voxelMin.x, voxelMin.y, voxelMin.z, voxelMax.x, voxelMax.y, voxelMax.z);

						batch->SetColor(0, 255, 255,255);
						batch->Cube(voxelMin, 1,1,1,false);
						//batch->Box(voxelMin, voxelMax);

                float dist;
      
                if (rayAABBIntersection(rayOrigin, rayDir, voxelMin, voxelMax, dist))
                {
                    
                    if (dist < minDist)
                    {
                        minDist = dist;
                        outX = x;
                        outY = y;
                        outZ = z;
                        hit = true;
	
                    }
                }
            }
        }
    }

  
    if (hit)
    {
        outDistance = minDist;
    }

    return hit;
}


bool Chunk::getVoxelFromScreen(float windowWidth, float windowHeight, int mouseX, int mouseY, const Mat4 &projMatrix, const Mat4 &viewMatrix, int &outX, int &outY, int &outZ)
{
	
	float normalizedX = (2.0f * mouseX) / windowWidth - 1.0f;
	float normalizedY = 1.0f - (2.0f * mouseY) / windowHeight;

	
	Vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);

 
	Mat4 invProj = projMatrix.inverse();
	Vec4 rayEye = invProj * rayClip;
	rayEye = Vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

 
	Mat4 invView = viewMatrix.inverse();
	Vec4 rayWorld4 = invView * rayEye;
	Vec3 rayDir(rayWorld4.x, rayWorld4.y, rayWorld4.z);
	rayDir = rayDir.normalize();

 
	Vec4 eyePos4 = invView * Vec4(0, 0, 0, 1);
	Vec3 rayOrigin(eyePos4.x, eyePos4.y, eyePos4.z);

	return false;
	//float distance;
	//return rayIntersectsVoxel(rayOrigin, rayDir, outX, outY, outZ, distance);
}