#include "blockarray3d.h"
#include "blockinfo.h"
#include "define.h"
#include <iostream>

BlockArray3D::BlockArray3D()
{
	std::cout << "ctor blockarray" << std::endl;
}

BlockArray3D::~BlockArray3D()
{
}

void BlockArray3D::Set(int x, int y, int z, BlockType type)
{
}

BlockType BlockArray3D::Get(int x, int y, int z) const
{
	return BlockType();
}

void BlockArray3D::Reset(BlockType type)
{
}