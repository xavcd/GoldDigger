#ifndef CHUNK_H__
#define CHUNK_H__
#include "blockarray3d.h"
#include "vertexbuffer.h"

class Chunk
{
public:
    Chunk();
    ~Chunk();

    void RemoveBlock(int x, int y, int z);
    void SetBlock(int x, int y, int z, BlockType type);
    BlockType GetBlock(int x, int y, int z);

    void AddBlockToMesh(VertexBuffer::VertexData* vd, int& count, BlockType bt, int x, int y, int z);

    void Update();
    void Render() const;
    bool IsDirty() const;
private:
    BlockArray3d m_blocks;
    VertexBuffer m_vertexBuffer;
    bool m_isDirty;
};

#endif // CHUNK_H__
