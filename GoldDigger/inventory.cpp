#include "inventory.h"

Inventory::Inventory()
{
	for (int i = 0; i < ARRAYSIZE(m_inventory); i++)
	{
		m_inventory[i] = BTYPE_AIR;
	}
}

Inventory::~Inventory()
{

}

BlockType Inventory::BlockAtIndex(int pos)
{
	return m_inventory[pos];
}

void Inventory::AddBlock(BlockType block)
{
	bool added = false;
	for (int i = 0; i < ARRAYSIZE(m_inventory); i++)
		if (added == false)
		{
			if (m_inventory[i] == BTYPE_AIR)
			{
				m_inventory[i] = block;
				added = true;
			}
		}
		else
			break;
}

void Inventory::RemoveFromInvent(int pos)
{
	m_inventory[pos] = BTYPE_AIR;
}

bool Inventory::Contains(BlockType block)
{
	for (int i = 0; i < ARRAYSIZE(m_inventory); i++)
	{
		if (m_inventory[i] == block)
			return true;
		else if (i == ARRAYSIZE(m_inventory) - 1)
			return false;
	}
}