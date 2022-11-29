#include "chunk.h"
#include <iostream>

Chunk::Chunk(float x, float z) : m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z), m_posx(x), m_posz(z), m_perlin(16, 6, 1, 95)
{
	m_blocks.Reset(BTYPE_AIR);
	for (int x = 0; x < CHUNK_SIZE_X; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; ++z)
		{
			// La méthode Get accepte deux parametre ( coordonnée en X et Z) et retourne une valeur qui respecte
			// les valeurs utilisées lors de la création de l’objet Perlin
			// La valeur retournée est entre -1 et 1

			float val = m_perlin.Get((float)(m_posx * CHUNK_SIZE_X + x) / 2000.f, (float)(m_posz * CHUNK_SIZE_Z + z) / 2000.f);
			val = val * 10 + (CHUNK_SIZE_Y /2);

			for (int y = 0; y < val; ++y)
			{
				if (y < 40)
					SetBlock(x, y, z, BTYPE_STONE);
				else if (y >= 40 && y < 60)
					SetBlock(x, y, z, BTYPE_DIRT);
				else if (y > 60 && y < 80)
					SetBlock(x, y, z, BTYPE_GRASS);

				// Utiliser val pour déterminer la hauteur du terrain à la position x,z
				// Vous devez vous assurer que la hauteur ne dépasse pas CHUNK_SIZE_Y
				// Remplir les blocs du bas du terrain jusqu’à la hauteur calculée.
				// N’hésitez pas à jouer avec la valeur retournée pour obtenir un résultat qui vous semble satisfaisant
			}
		}
	}
}

Chunk::~Chunk()
{
}

void Chunk::RemoveBlock(int x, int y, int z)
{
	m_blocks.Set(x, y, z, BTYPE_AIR);
	m_isDirty = true;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type)
{
	m_blocks.Set(x, y, z, type);
	m_isDirty = true;
}

BlockType Chunk::GetBlock(int x, int y, int z)
{
	return m_blocks.Get(x, y, z);
}

void Chunk::Update(BlockInfo* blockInfo[BTYPE_LAST])
{
	// Update mesh
	if (m_isDirty)
	{
		int maxVertexCount = (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z) * (6 * 4);
		VertexBuffer::VertexData* vd = new VertexBuffer::VertexData[maxVertexCount];
		int count = 0;
		for (int x = 0; x < CHUNK_SIZE_X; ++x)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; ++z)
			{
				for (int y = 0; y < CHUNK_SIZE_Y; ++y)
				{
					if (count > USHRT_MAX)
						break;
					BlockType bt = GetBlock(x, y, z);

					if (bt != BTYPE_AIR)
					{
						BlockInfo* biCurrent = blockInfo[bt];

						float u = biCurrent->GetU();
						float v = biCurrent->GetV();
						float w = biCurrent->GetW();
						AddBlockToMesh(vd, count, bt, x, y, z, u, v, w);
					}
				}
			}
		}
		if (count > USHRT_MAX)
		{
			count = USHRT_MAX;
			std::cout << "[ Chunk :: Update ] Chunk data truncaned , too much vertices to have a 16 bit index " << std::endl;
		}
		m_vertexBuffer.SetMeshData(vd, count);
		delete[] vd;
	}
	m_isDirty = false;

}

void Chunk::AddBlockToMesh(VertexBuffer::VertexData* vd, int& count, BlockType bt, int x, int y, int z, float u, float v, float w)
{
	// front
	if (CHUNK_SIZE_Z - 1 == z || m_blocks.Get(x, y, z + 1) == BTYPE_AIR)
	{
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u + w, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u + w, v + w);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u, v + w);
	}
	// back
	if (z == 0 || m_blocks.Get(x, y, z - 1) == BTYPE_AIR)
	{
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u + w, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u + w, v + w);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u, v + w);
	}
	// right
	if (x == 0 || m_blocks.Get(x + 1, y, z) == BTYPE_AIR)
	{
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z + .5f, .9f, .9f, .9f, u, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z - .5f, .9f, .9f, .9f, u + w, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z - .5f, .9f, .9f, .9f, u + w, v + w);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z + .5f, .9f, .9f, .9f, u, v + w);
	}
	// left
	if (CHUNK_SIZE_X - 1 == x || m_blocks.Get(x - 1, y, z) == BTYPE_AIR)
	{
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z - .5f, .9f, .9f, .9f, u, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z + .5f, .9f, .9f, .9f, u + w, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z + .5f, .9f, .9f, .9f, u + w, v + w);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z - .5f, .9f, .9f, .9f, u, v + w);
	}
	// top
	if (CHUNK_SIZE_Y - 1 == y || m_blocks.Get(x, y + 1, z) == BTYPE_AIR)
	{
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z + .5f, .8f, .8f, .8f, u + w, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z - .5f, .8f, .8f, .8f, u + w, v + w);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z - .5f, .8f, .8f, .8f, u, v + w);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z + .5f, .8f, .8f, .8f, u, v);
	}
	// bottom
	if (y == 0 || m_blocks.Get(x, y - 1, z) == BTYPE_AIR)
	{
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z - .5f, .8f, .8f, .8f, u, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z - .5f, .8f, .8f, .8f, u + w, v);
		vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z + .5f, .8f, .8f, .8f, u + w, v + w);
		vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z + .5f, .8f, .8f, .8f, u, v + w);
	}
}

void Chunk::Render() const
{
	m_vertexBuffer.Render();
}

bool Chunk::IsDirty() const
{
	return m_isDirty;
}

