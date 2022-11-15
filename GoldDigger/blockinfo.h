#ifndef BLOCKINFO_H__
#define BLOCKINFO_H__

#include <string>
#include "define.h"

class BlockInfo
{
    public:
    BlockInfo(BlockType type, const std::string& name);
    ~BlockInfo();

    BlockType GetType() const;

    void SetDurability(int durability);
    int GetDurability() const;

    void SetUVW(float u, float v, float w);

    float GetU() const;
    float GetV() const;
    float GetW() const;

    void Show() const;

    private:
    BlockType m_type;
    std::string m_name;
    int m_durability;
    float m_u, m_v, m_w;
};

#endif // BLOCKINFO_H__
