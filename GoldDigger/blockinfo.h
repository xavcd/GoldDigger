#ifndef BLOCKINFO_H__
#define BLOCKINFO_H__

#include "define.h"
#include <string>

class BlockInfo
{
public:
	BlockInfo(BlockType type, const std::string& name);
	~BlockInfo();

	BlockType GetType() const;

	const std::string& GetName() const;

	int GetDurability() const;
	void SetDurability(int durability);

	void Show() const;

private:
	BlockType m_type;
	std::string m_name;
	int m_durability;
};

#endif