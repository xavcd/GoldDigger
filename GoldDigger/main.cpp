#include <iostream>
#include "define.h"
#include "blockinfo.h"
#include "blockarray3d.h"

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

	BlockArray3D* world = new BlockArray3D;
}