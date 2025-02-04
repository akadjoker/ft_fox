#include "World.hpp"

World::World(Camera *camera, bool is_seed, unsigned int seed)
{
	this->camera = camera;

	if (is_seed)
		mapgen = new PerlinNoise(seed);
	else
		mapgen = new PerlinNoise();

	isCity = true;
}

World::~World()
{
	delete (mapgen);
}

void World::MapHandler()
{
	int x;
	int y;
	int i;
	int j;

	Vec3 pos = camera->getPosition();

	i = -VIEW_DISTANCE + pos.x - CHUNK_XY * 4;
	while (i < VIEW_DISTANCE + pos.x)
	{
		j = -VIEW_DISTANCE + pos.z - CHUNK_XY * 4;
		while (j < VIEW_DISTANCE + pos.z)
		{
			x = i - i % CHUNK_XY;
			y = j - j % CHUNK_XY;
			if (Vec2::Distance(Vec2(x, y) + Vec2(CHUNK_XY / 2, CHUNK_XY / 2), Vec2(pos.x, pos.z)) <= VIEW_DISTANCE)
			{
				if (!CheckIfChunkAtPos(x, y))
					AddChunkAtPos(x, y);
			}
			j += CHUNK_XY;
		}
		i += CHUNK_XY;
	}
	DisableChunks();
	RemoveFarChunks();
}

static std::string GetPosIndex(int x, int y)
{
	return std::string(std::to_string(x) + " " + std::to_string(y));
}

static std::string GetPosIndex(Vec2 pos)
{
	return GetPosIndex(pos.x, pos.y);
}

void World::CheckNeigbors(Chunk *chunk)
{
	Chunk *neighbor;

	if ((neighbor = GetChunkAtPos((chunk->GetPos() + Vec2(CHUNK_XY, 0)).x,
								  (chunk->GetPos() + Vec2(CHUNK_XY, 0)).y)) != NULL)
	{
		neighbor->left = chunk;
		chunk->right = neighbor;
	}
	if ((neighbor = GetChunkAtPos((chunk->GetPos() + Vec2(-CHUNK_XY, 0)).x,
								  (chunk->GetPos() + Vec2(-CHUNK_XY, 0)).y)) != NULL)
	{
		neighbor->right = chunk;
		chunk->left = neighbor;
	}
	if ((neighbor = GetChunkAtPos((chunk->GetPos() + Vec2(0, CHUNK_XY)).x,
								  (chunk->GetPos() + Vec2(0, CHUNK_XY)).y)) != NULL)
	{
		neighbor->front = chunk;
		chunk->back = neighbor;
	}
	if ((neighbor = GetChunkAtPos((chunk->GetPos() + Vec2(0, -CHUNK_XY)).x,
								  (chunk->GetPos() + Vec2(0, -CHUNK_XY)).y)) != NULL)
	{
		neighbor->back = chunk;
		chunk->front = neighbor;
	}
}

void World::DisableChunks()
{
	Chunk *chunk;
	std::map<std::string, Chunk *>::iterator it = enabledChunks.begin();
	std::map<std::string, Chunk *>::iterator tmp;
	Vec3 pos = camera->getPosition();

	while (it != enabledChunks.end() && (chunk = (*it).second))
	{
		if (!chunk->isUsable() && chunk->isGenerated())
		{
			CheckNeigbors(chunk);
			chunk->HasFourNeigbors();
		}
		if (chunk->isGenerated() && Vec2::Distance(chunk->GetPos() + Vec2(CHUNK_XY / 2, CHUNK_XY / 2), Vec2(pos.x, pos.z)) > VIEW_DISTANCE)
		{
			chunk->Disable();
			disabledChunks.insert(std::pair<std::string, Chunk *>(GetPosIndex(chunk->GetPos()), chunk));
			enabledChunks.erase(it);
			it = enabledChunks.begin();
		}
		else
			it++;
	}
}

void World::LoadChunks()
{
	Chunk *current;
	std::map<std::string, Chunk *>::iterator it = enabledChunks.begin();
	Voxels = 0;
	Cubes = 0;
	Vertices = 0;

	while (it != enabledChunks.end() && (current = (*it).second))
	{

		// Vec2 pos =  current->GetPos();
		if (current->isUsable() && current->HasFourNeigbors() &&
			// camera->getFrustum()->isPointInFrustum(pos.x + CHUNK_XY/2, CHUNK_Z/2, pos.y + CHUNK_XY/2))
			camera->getFrustum()->isBoxInFrustum(current->getBoundingBox()))

		{
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, current->GetVBOID());
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, current->GetTID());
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glVertexAttribDivisor(1, 1);
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, current->GetCID());
			glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);
			glVertexAttribDivisor(2, 1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current->GetIBOID());
			glDrawElementsInstanced(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, NULL, current->GetNbInstances());
			Vertices += 12 * 3;
			Voxels += 1;
			Cubes += current->GetNbInstances();
		}
		it++;
	}
}

void World::clean()
{

	std::map<std::string, Chunk *>::iterator it = disabledChunks.begin();
	Chunk *chunk;
	while (it != disabledChunks.end() && (chunk = (*it).second))
	{

		disabledChunks.erase(it);
		delete (chunk);
		it = disabledChunks.begin();
	}

	it = enabledChunks.begin();
	while (it != enabledChunks.end() && (chunk = (*it).second))
	{

		enabledChunks.erase(it);
		delete (chunk);
		it = enabledChunks.begin();
	}

	enabledChunks.clear();
	disabledChunks.clear();
}
RayPickResult World::getChunkAndVoxelFromScreen(RenderBatch *batch, float windowWidth, float windowHeight, int mouseX, int mouseY)
{
	RayPickResult result;

 
	Vec3 rayOrigin = camera->getPosition();
	Vec3 rayDir = screenToWorldRay(batch, windowWidth, windowHeight, mouseX, mouseY);

	float minDistance = std::numeric_limits<float>::max();


	for (const auto &pair : enabledChunks)
	{
		Chunk *chunk = pair.second;

	
		Vec3 chunkMin(chunk->GetPos().x, 0, chunk->GetPos().y);
		Vec3 chunkMax = chunkMin + Vec3(CHUNK_XY, CHUNK_Z, CHUNK_XY);

		float chunkDist;

		if (chunk->rayAABBIntersection(rayOrigin, rayDir, chunkMin, chunkMax, chunkDist))
		{
			int voxelX, voxelY, voxelZ;
			float voxelDist;


			if (chunk->rayIntersectsVoxel(batch, rayOrigin, rayDir, voxelX, voxelY, voxelZ, voxelDist))
			{
			 	batch->SetColor(255, 0, 0, 255);
				 batch->Box(chunkMin, chunkMax);
				if (voxelDist < minDistance)
				{
					minDistance = voxelDist;
					result.chunk = chunk;
				
					
					result.voxelX = voxelX;
					result.voxelY = voxelY;
					result.voxelZ = voxelZ;
					result.distance = voxelDist;
					result.found = true;
				}
			}
		}
	}
	batch->Line3D(rayOrigin, rayOrigin + rayDir * 320.0f);

	return result;
}


void generateChunk(bool city,Chunk **chunk, PerlinNoise **map)
{
	if (city)
		(*chunk)->generateCity(map);
	else
		(*chunk)->generateDefault(map);
}

Vec3 World::screenToWorldRay(RenderBatch *batch,float windowWidth, float windowHeight,int mouseX, int mouseY)
{
	// Converte coordenadas do mouse para NDC (-1 to 1)
	float normalizedX = (2.0f * mouseX) / windowWidth - 1.0f;
	float normalizedY = 1.0f - (2.0f * mouseY) / windowHeight;

	// Obter matrizes da câmera
	Mat4 projMatrix = camera->getProjectionMatrix();
	Mat4 viewMatrix = camera->getViewMatrix();

	// Criar ray em clip space
	Vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);

	// Transformar para eye space
	Mat4 invProj = projMatrix.inverse();
	Vec4 rayEye = invProj * rayClip;
	rayEye = Vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	// Transformar para world space
	Mat4 invView = viewMatrix.inverse();
	Vec4 rayWorld4 = invView * rayEye;
	Vec3 rayDir(rayWorld4.x, rayWorld4.y, rayWorld4.z);

	return rayDir.normalize();
}

void World::AddChunkAtPos(int x, int y)
{
	static int id = 0;
	Chunk *chunk;

	chunk = new Chunk(Vec2(x, y), id);
	id++;
	enabledChunks.insert(std::pair<std::string, Chunk *>(GetPosIndex(x, y), chunk));
	generateChunk(isCity,&(*(enabledChunks.find(GetPosIndex(x, y)))).second, &mapgen);
}

void World::RemoveFarChunks()
{
	std::map<std::string, Chunk *>::iterator it = disabledChunks.begin();
	Chunk *chunk;
	Vec3 pos = camera->getPosition();

	while (it != disabledChunks.end() && (chunk = (*it).second))
	{
		if (Vec2::Distance(chunk->GetPos(), Vec2(pos.x, pos.z)) > 3000.0f)
		{
			disabledChunks.erase(it);
			delete (chunk);
			it = disabledChunks.begin();
		}
		else
			it++;
	}
}

bool World::CheckIfChunkAtPos(int x, int y)
{
	Chunk *chunk;
	std::map<std::string, Chunk *>::iterator it;

	it = enabledChunks.find(GetPosIndex(x, y));
	if (it != enabledChunks.end())
		return true;
	it = disabledChunks.find(GetPosIndex(x, y));
	if (it != disabledChunks.end())
	{
		chunk = (*it).second;
		chunk->Enable();
		enabledChunks.insert(std::pair<std::string, Chunk *>(GetPosIndex(chunk->GetPos()), chunk));
		disabledChunks.erase(it);
		return true;
	}
	return false;
}

Chunk *World::GetChunkAtPos(int x, int y)
{
	std::map<std::string, Chunk *>::iterator it;

	it = enabledChunks.find(GetPosIndex(x, y));
	if (it != enabledChunks.end())
		return (*it).second;
	it = disabledChunks.find(GetPosIndex(x, y));
	if (it != disabledChunks.end())
		return (*it).second;
	return NULL;
}
