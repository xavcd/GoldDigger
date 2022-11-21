#include "chunk.h"
#include <iostream>

Chunk::Chunk(float x, float z) : m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z), m_posx(x), m_posz(z)
{
	m_blocks.Reset(BTYPE_AIR);
	for (int y = 0; y < CHUNK_SIZE_Y; ++y)
		for (int x = 0; x < CHUNK_SIZE_X; ++x)
			for (int z = 0; z < CHUNK_SIZE_Z; ++z)
				if (y < 2)
					SetBlock(x, y, z, BTYPE_DIRT);
				else if (y == 2)
					SetBlock(x, y, z, BTYPE_GRASS);
				else
					SetBlock(x, y, z, BTYPE_AIR);
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
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u + w, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u + w, v + w);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z + .5f, 1.f, 1.f, 1.f, u, v + w);
	// right
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z + .5f, .9f, .9f, .9f, u, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z - .5f, .9f, .9f, .9f, u + w, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z - .5f, .9f, .9f, .9f, u + w, v + w);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z + .5f, .9f, .9f, .9f, u, v + w);
	// back
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u + w, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u + w, v + w);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z - .5f, 1.f, 1.f, 1.f, u, v + w);
	// left
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z - .5f, .9f, .9f, .9f, u, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z + .5f, .9f, .9f, .9f, u + w, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z + .5f, .9f, .9f, .9f, u + w, v + w);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z - .5f, .9f, .9f, .9f, u, v + w);
	// top
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z + .5f, .8f, .8f, .8f, u + w, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y + .5f, m_posz + z - .5f, .8f, .8f, .8f, u + w, v + w);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z - .5f, .8f, .8f, .8f, u, v + w);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y + .5f, m_posz + z + .5f, .8f, .8f, .8f, u, v);
	// bottom
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z - .5f, .8f, .8f, .8f, u, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z - .5f, .8f, .8f, .8f, u + w, v);
	vd[count++] = VertexBuffer::VertexData(m_posx + x + .5f, y - .5f, m_posz + z + .5f, .8f, .8f, .8f, u + w, v + w);
	vd[count++] = VertexBuffer::VertexData(m_posx + x - .5f, y - .5f, m_posz + z + .5f, .8f, .8f, .8f, u, v + w);
}

void Chunk::Render() const
{
	m_vertexBuffer.Render();
}

bool Chunk::IsDirty() const
{
	return m_isDirty;
}

