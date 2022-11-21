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
	virtual void KeyPressEvent(unsigned char key);
	virtual void KeyReleaseEvent(unsigned char key);
	virtual void MouseMoveEvent(int x, int y);
	virtual void MousePressEvent(const MOUSE_BUTTON& button, int x, int y);
	virtual void MouseReleaseEvent(const MOUSE_BUTTON& button, int x, int y);
	virtual void PrintText(unsigned int x, unsigned int y, const std::string& t);
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
	Shader m_shader01;
	TextureAtlas m_textureAtlas;
	BlockInfo* m_blockInfo[BTYPE_LAST];
	Array2d<Chunk*> m_array2d;

	int iGrav = 0;
	bool m_keyW = false;
	bool m_keyA = false;
	bool m_keyS = false;
	bool m_keyD = false;
	bool m_keySpace = false;

	Player m_player;
};

#endif // ENGINE_H__
