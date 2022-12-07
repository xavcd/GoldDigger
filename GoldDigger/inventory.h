#ifndef INVENTORY_H__
#define INVENTORY_H__
#include "blockinfo.h"

class Inventory
{
public:
	Inventory();
	~Inventory();
	void AddBlock(BlockType block);
	void RemoveBlock(int pos);
	bool Contains(BlockType block);
	BlockType BlockAtIndex(int pos);

private:
	BlockType m_inventory[29];
};


#endif
