#include "blockarray3d.h"
#include "blockinfo.h"
#include "define.h"
#include <iostream>

BlockArray3D::BlockArray3D(int x, int y, int z) : m_x(x), m_y(y), m_z(z)
{
	m_blocks = new BlockType[m_x*m_y*m_z];
	Reset(BTYPE_AIR);
}

BlockArray3D::~BlockArray3D()
{
	delete[] m_blocks;
	std::cout << "dtor" << std::endl; // TEST
}

BlockArray3D::BlockArray3D (const BlockArray3D& array)
{
	m_x = array.m_x;
	m_y = array.m_y;
	m_z = array.m_z;

	m_blocks = new BlockType[m_x * m_y * m_z];
	for (int i = 0; i < m_x * m_y * m_z; i++)
		m_blocks[i] = array.m_blocks[i];
}

void BlockArray3D::Set(int x, int y, int z, BlockType type)
{
	m_blocks[x + (z * m_x) + (y * m_z * m_x)] = type;
}

BlockType BlockArray3D::Get(int x, int y, int z) const
{
	return m_blocks[x + (z * m_x) + (y * m_z * m_x)];
}

void BlockArray3D::Reset(BlockType type)
{
	for (int i = 0; i < (m_x * m_y * m_z); i++)
		m_blocks[i] = type;
}