#ifndef CHUNK_H__
#define CHUNK_H__

#include "blockarray3d.h" 
#include <string>

class Chunk : public BlockArray3D
{
public:
	Chunk();
	~Chunk();

	void RemoveBlock(int x, int y, int z);
	void SetBlock(int x, int y, int z, BlockType type);
	BlockType GetBlock(int x, int y, int z) const;
};


#endif