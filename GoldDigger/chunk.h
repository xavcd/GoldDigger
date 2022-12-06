#ifndef CHUNK_H__
#define CHUNK_H__
#include "array3d.h"
#include "vertexbuffer.h"
#include "blockinfo.h"
#include "perlin.h"

class Chunk
{
public:
    Chunk(float x, float z);
    ~Chunk();

    void RemoveBlock(int x, int y, int z);
    void SetBlock(int x, int y, int z, BlockType type);
    BlockType GetBlock(int x, int y, int z);

    void AddBlockToMesh(VertexBuffer::VertexData* vd, int& count, BlockType bt, int x, int y, int z, float u, float v, float w);

    void Update(BlockInfo *blockInfo[BTYPE_LAST]);
    void Render() const;
    bool IsDirty() const;
private:
    Array3d<BlockType> m_blocks;
    VertexBuffer m_vertexBuffer;
    Perlin m_perlin;
    bool m_isDirty;
    bool m_modified;
    float m_posx;
    float m_posz;
};

#endif // CHUNK_H__
