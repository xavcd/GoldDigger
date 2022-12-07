#ifndef ENGINE_H__
#define ENGINE_H__
#include "define.h"
#include "openglcontext.h"
#include "texture.h"
#include "player.h"
#include "shader.h"
#include "chunk.h"
#include "textureatlas.h"
#include "blockinfo.h"
#include "array2d.h"
#include "inventory.h"

class Engine : public OpenglContext
{
public:
	Engine();
	virtual ~Engine();
	virtual void Init();
	virtual void DeInit();
	virtual void LoadResource();
	virtual void UnloadResource();
	virtual void Render(float elapsedTime);
	virtual void DrawHud();
	virtual float GetFps();
	virtual void StartInventory();
	virtual void KeyPressEvent(unsigned char key);
	virtual void KeyReleaseEvent(unsigned char key);
	virtual void MouseMoveEvent(int x, int y);
	virtual void MousePressEvent(const MOUSE_BUTTON& button, int x, int y);
	virtual void MouseReleaseEvent(const MOUSE_BUTTON& button, int x, int y);
	virtual void PrintText(unsigned int x, unsigned int y, const std::string& t);
	static bool EqualWithEpsilon(float v1, float v2, float epsilon = 0.09f);
	static bool InRangeWithEpsilon(float v, float vinf, float vsup, float epsilon = 0.0001f);
	virtual void GetBlocAtCursor();
	template <class T>
	Chunk* ChunkAt(T x, T y, T z) const;
	template <class T>
	Chunk* ChunkAt(const Vector3<T>& pos) const;
	template <class T>
	BlockType BlockAt(T x, T y, T z, BlockType defaultBlockType) const;

private:
	bool LoadTexture(Texture& texture, const std::string& filename, bool stopOnError = true);

private:
	bool m_wireframe = false;

	Texture m_textureFloor;
	Texture m_textureBlock;
	Texture m_textureCrosshair;
	Texture m_textureFont;
	Texture m_textureHotbar;
	Texture m_textureDirt;
	Texture m_textureStone;
	Texture m_textureGrass;
	Texture m_textureBrick;
	Shader m_shader01;
	TextureAtlas m_textureAtlas;
	BlockInfo* m_blockInfo[BTYPE_LAST];
	Array2d<Chunk*> m_array2d;
	Vector3f m_currentBlock;
	Vector3f m_currentFaceNormal;
	BlockType m_selectedBlockType = BTYPE_AIR;
	BlockType m_brokenBlock;

	float m_velocity = 0.0f;
	bool m_keyW = false;
	bool m_keyA = false;
	bool m_keyS = false;
	bool m_keyD = false;
	bool m_jump = false;
	bool m_sprint = false;
	bool m_inventoryOpen = false;

	Inventory m_inventory;
	Player m_player;
};

#endif // ENGINE_H__
