#ifndef BLOCKARRAY3D_H__
#define BLOCKARRAY3D_H__

#include "define.h"
#include "blockinfo.h" 
#include <string>

class BlockArray3D
{
public:
	BlockArray3D(int x = 1, int y = 1, int z = 1);
	~BlockArray3D();

	void Set(int x, int y, int z, BlockType type);
	BlockType Get(int x, int y, int z) const;
	void Reset(BlockType type);

private:
	int m_x;
	int m_y;
	int m_z;
	BlockType* m_blocks;
};
#endif
