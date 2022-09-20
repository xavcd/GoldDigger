#include "chunk.h"
#include <iostream>

Chunk::Chunk() : BlockArray3D(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
}

Chunk::~Chunk()
{
}

void Chunk::RemoveBlock(int x, int y, int z)
{
	Set(x,y,z,BTYPE_AIR);
}

void Chunk::SetBlock(int x, int y, int z, BlockType type)
{
	Set(x, y, z, type);
}

BlockType Chunk::GetBlock(int x, int y, int z) const
{
	return BlockType(Get(x, y, z));
}
