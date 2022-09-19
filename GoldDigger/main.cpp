#include <iostream>
#include "define.h"
#include "blockinfo.h"
#include "blockarray3d.h"
#include "chunk.h"

int main()
{
	std::cout << "hello world" << std::endl;

	// TESTS.
	BlockInfo b1;
	BlockInfo b2(BTYPE_DIRT,"Terre");
	b1.SetDurability(0);
	b2.SetDurability(10);

	b1.Show();

	std::cout << "Type: " << b2.GetType() << std::endl;
	std::cout << "Name: " << b2.GetName() << std::endl;
	std::cout << "Durability: " << b2.GetDurability() << std::endl;
	b2.Show();

	BlockArray3D world(16,128,16);

	std::cout << world.Get(1, 1, 1) << std::endl;
	world.Set(1, 1, 2, BTYPE_DIRT);
	std::cout << world.Get(1, 1, 2) << std::endl;

	Chunk c1;
	c1.GetBlock(1, 1, 1);
}