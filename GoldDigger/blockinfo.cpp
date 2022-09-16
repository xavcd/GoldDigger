#include "blockinfo.h"
#include <iostream>

BlockInfo::BlockInfo(BlockType type, const std::string& name) : m_type(type), m_name(name)
{

};

BlockInfo::~BlockInfo()
{

}

BlockType BlockInfo::GetType() const
{
	return m_type;
}

const std::string& BlockInfo::GetName() const
{
	return m_name;
}

int BlockInfo::GetDurability()  const
{
	return m_durability;
}

void BlockInfo::SetDurability(int durability)
{
	m_durability = durability;
}

void BlockInfo::Show() const
{
	std::cout << "Type.........: " + m_type << std::endl;
	std::cout << "Name.........: " + m_name << std::endl;
	std::cout << "Durability...: " + m_durability << std::endl;
}