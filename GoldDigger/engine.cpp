#include "engine.h"
#include "transformation.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Engine::Engine() : m_player(Vector3f(50.f, 4.8f, 50.f), 0.f, 0.f), m_textureAtlas(BTYPE_LAST), m_array2d((VIEW_DISTANCE * 2) / CHUNK_SIZE_X, (VIEW_DISTANCE * 2) / CHUNK_SIZE_Z)
{
}

Engine::~Engine()
{
}

void Engine::Init()
{
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
	{
		std::cerr << " ERREUR GLEW : " << glewGetErrorString(glewErr) << std::endl;
		abort();
	}

	glClearColor(0.1f, 0.6f, 1.0f, 0.f);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)Width() / (float)Height(), 0.0001f, 1000.0f);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_CULL_FACE);

	// Light
	GLfloat light0Pos[4] = { 0.0f, CHUNK_SIZE_Y, 0.0f, 1.0f };
	GLfloat light0Amb[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat light0Diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light0Spec[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Spec);

	CenterMouse();
	HideCursor();
}

void Engine::DeInit()
{
}

void Engine::LoadResource()
{
	std::cout << " Loading and compiling shaders ..." << std::endl;
	if (!m_shader01.Load(SHADER_PATH "shader01.vert", SHADER_PATH "shader01.frag", true))
	{
		std::cout << " Failed to load shader " << std::endl;
		exit(1);
	}

	for (int x = 0; x < (VIEW_DISTANCE * 2) / CHUNK_SIZE_X; x++)
		for (int z = 0; z < (VIEW_DISTANCE * 2) / CHUNK_SIZE_Z; z++)
			m_array2d.Set(x, z, new Chunk(x * CHUNK_SIZE_X, z * CHUNK_SIZE_Z));

	LoadTexture(m_textureFloor, TEXTURE_PATH "grass.jpg");
	LoadTexture(m_textureBlock, TEXTURE_PATH "dirt.png");
	LoadTexture(m_textureCrosshair, TEXTURE_PATH "cross.bmp");
	LoadTexture(m_textureFont, TEXTURE_PATH "font.bmp");

	TextureAtlas::TextureIndex texDirtIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "dirt.png");
	float u, v, w;
	m_textureAtlas.TextureIndexToCoord(texDirtIndex, u, v, w, w);
	m_blockInfo[BTYPE_DIRT] = new BlockInfo(BTYPE_DIRT, "dirt");
	m_blockInfo[BTYPE_DIRT]->SetUVW(u, v, w);

	TextureAtlas::TextureIndex texGrassIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "grass.jpg");
	m_textureAtlas.TextureIndexToCoord(texGrassIndex, u, v, w, w);
	m_blockInfo[BTYPE_GRASS] = new BlockInfo(BTYPE_GRASS, "grass");
	m_blockInfo[BTYPE_GRASS]->SetUVW(u, v, w);

	TextureAtlas::TextureIndex texBrickIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "brick.png");
	m_textureAtlas.TextureIndexToCoord(texBrickIndex, u, v, w, w);
	m_blockInfo[BTYPE_BRICK] = new BlockInfo(BTYPE_BRICK, "brick");
	m_blockInfo[BTYPE_BRICK]->SetUVW(u, v, w);

	TextureAtlas::TextureIndex texStoneIndex = m_textureAtlas.AddTexture(TEXTURE_PATH "stone.png");
	m_textureAtlas.TextureIndexToCoord(texStoneIndex, u, v, w, w);
	m_blockInfo[BTYPE_STONE] = new BlockInfo(BTYPE_STONE, "stone");
	m_blockInfo[BTYPE_STONE]->SetUVW(u, v, w);

	if (!m_textureAtlas.Generate(128, false))
	{
		std::cout << "Unable to generate texture atlas ..." << std::endl;
		abort();
	}
}

void Engine::UnloadResource()
{
}

void Engine::Render(float elapsedTime)
{
	if (elapsedTime > .2f)
	{
		return;
	}
	static float gameTime = elapsedTime;

	gameTime += elapsedTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Transformations initiales
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	Vector3f pos = m_player.Position();
	Vector3f delta = m_player.SimulateMove(m_keyW, m_keyS, m_keyA, m_keyD, elapsedTime);
	BlockType block_under = BlockAt(pos.x, pos.y + delta.y - 1.9f, pos.z, BTYPE_AIR);

	// Gravité
	delta.y -= elapsedTime * 6;

	delta.y += m_velocity * elapsedTime;
	m_velocity *= 0.9f;

	BlockType bt1, bt2, bt3;
	// Collision par rapport au déplacement en x:
	bt1 = BlockAt(pos.x + delta.x, pos.y, pos.z, BTYPE_AIR);
	bt2 = BlockAt(pos.x + delta.x, pos.y - 0.9f, pos.z, BTYPE_AIR);
	bt3 = BlockAt(pos.x + delta.x, pos.y - 1.7f, pos.z, BTYPE_AIR);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR)
		delta.x = 0;
	// Collision par rapport au déplacement en y:
	bt1 = BlockAt(pos.x, pos.y + delta.y, pos.z, BTYPE_AIR);
	bt2 = BlockAt(pos.x, pos.y + delta.y - 0.9f, pos.z, BTYPE_AIR);
	bt3 = BlockAt(pos.x, pos.y + delta.y - 1.7f, pos.z, BTYPE_AIR);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR)
		delta.y = 0;
	// Collision par rapport au déplacement en z:
	bt1 = BlockAt(pos.x, pos.y, pos.z + delta.z, BTYPE_AIR);
	bt2 = BlockAt(pos.x, pos.y - 0.9f, pos.z + delta.z, BTYPE_AIR);
	bt3 = BlockAt(pos.x, pos.y - 1.7f, pos.z + delta.z, BTYPE_AIR);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR)
		delta.z = 0;

	if (block_under != BTYPE_AIR)
		m_jump = true;
	else
		m_jump = false;

	pos += delta;
	m_player.SetPosition(pos);


	Transformation c;
	m_player.ApplyTransformation(c);
	c.ApplyTranslation(.5f, .5f, .5f);
	c.Use();

	// Chunks
	m_textureAtlas.Bind();
	m_shader01.Use();
	for (int x = 0; x < (VIEW_DISTANCE * 2) / CHUNK_SIZE_X; x++)
		for (int z = 0; z < (VIEW_DISTANCE * 2) / CHUNK_SIZE_Z; z++)
		{
			if (m_array2d.Get(x, z)->IsDirty())
				m_array2d.Get(x, z)->Update(m_blockInfo);
			m_array2d.Get(x, z)->Render();
		}

	Shader::Disable();

	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	DrawHud();
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Engine::DrawHud()
{
	// Setter le blend function , tout ce qui sera noir sera transparent
	glDisable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width(), 0, Height(), -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	// Bind de la texture pour le font
	m_textureFont.Bind();
	std::ostringstream ss;
	ss << " Fps : " << GetFps();
	PrintText(10, Height() - 25, ss.str());
	ss.str("");
	ss << " Position : " << m_player.Position(); // IMPORTANT : on utilise l ’ operateur << pour afficher la position
	PrintText(10, 10, ss.str());
	// Affichage du crosshair
	m_textureCrosshair.Bind();
	static const int crossSize = 32;
	glLoadIdentity();
	glTranslated(Width() / 2 - crossSize / 2, Height() / 2 - crossSize / 2, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(0, 0);
	glTexCoord2f(1, 0);
	glVertex2i(crossSize, 0);
	glTexCoord2f(1, 1);
	glVertex2i(crossSize, crossSize);
	glTexCoord2f(0, 1);
	glVertex2i(0, crossSize);
	glEnd();
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

bool Engine::EqualWithEpsilon(float v1, float v2, float epsilon)
{
	return (fabs(v2 - v1) < epsilon);
}

bool Engine::InRangeWithEpsilon(float v, float vinf, float vsup, float epsilon)
{
	return (v >= vinf - epsilon && v <= vsup + epsilon);
}

void Engine::GetBlocAtCursor()
{
	int x = Width() / 2;
	int y = Height() / 2;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	posX += .5f;
	posY += .5f;
	posZ += .5f;

	// Le cast vers int marche juste pour les valeurs entiere, utiliser une fonction de la libc si besoin
	// de valeurs negatives
	int px = (int)(posX);
	int py = (int)(posY);
	int pz = (int)(posZ);

	bool found = false;

	if ((m_player.Position() - Vector3f((float)posX, (float)posY, (float)posZ)).Length() < MAX_SELECTION_DISTANCE)
	{
		// Apres avoir determine la position du bloc en utilisant la partie entiere du hit
		// point retourne par opengl, on doit verifier de chaque cote du bloc trouve pour trouver
		// le vrai bloc. Le vrai bloc peut etre different a cause d'erreurs de precision de nos
		// nombres flottants (si z = 14.999 par exemple, et qu'il n'y a pas de blocs a la position
		// 14 (apres arrondi vers l'entier) on doit trouver et retourner le bloc en position 15 s'il existe
		// A cause des erreurs de precisions, ils arrive que le cote d'un bloc qui doit pourtant etre a la
		// position 15 par exemple nous retourne plutot la position 15.0001
		for (int x = px - 1; !found && x <= px + 1; ++x)
		{
			for (int y = py - 1; !found && x >= 0 && y <= py + 1; ++y)
			{
				for (int z = pz - 1; !found && y >= 0 && z <= pz + 1; ++z)
				{
					if (z >= 0)
					{
						BlockType bt = BlockAt((float)x, (float)y, (float)z, BTYPE_AIR);
						if (bt == BTYPE_AIR)
							continue;

						// Skip water blocs
						//if(bloc->Type == BT_WATER)
						//    continue;

						m_currentBlock.x = x;
						m_currentBlock.y = y;
						m_currentBlock.z = z;

						if (InRangeWithEpsilon((float)posX, (float)x, (float)x + 1.f, 0.05f) &&
							InRangeWithEpsilon((float)posY, (float)y, (float)y + 1.f, 0.05f) &&
							InRangeWithEpsilon((float)posZ, (float)z, (float)z + 1.f, 0.05f))
						{
							found = true;
						}
					}
				}
			}
		}
	}

	if (!found)
	{
		m_currentBlock.x = -1;
	}
	else
	{
		// Find on which face of the bloc we got an hit
		m_currentFaceNormal.Zero();

		const float epsilon = 0.005f;

		// Front et back:
		if (EqualWithEpsilon((float)posZ, (float)m_currentBlock.z, epsilon))
			m_currentFaceNormal.z = -1;
		else if (EqualWithEpsilon((float)posZ, (float)m_currentBlock.z + 1.f, epsilon))
			m_currentFaceNormal.z = 1;
		else if (EqualWithEpsilon((float)posX, (float)m_currentBlock.x, epsilon))
			m_currentFaceNormal.x = -1;
		else if (EqualWithEpsilon((float)posX, (float)m_currentBlock.x + 1.f, epsilon))
			m_currentFaceNormal.x = 1;
		else if (EqualWithEpsilon((float)posY, (float)m_currentBlock.y, epsilon))
			m_currentFaceNormal.y = -1;
		else if (EqualWithEpsilon((float)posY, (float)m_currentBlock.y + 1.f, epsilon))
			m_currentFaceNormal.y = 1;
	}
}


float Engine::GetFps()
{
	static float framesPerSecond = 0.0f;
	static int fps;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount64() * 0.001f;
	++framesPerSecond;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		fps = (int)framesPerSecond;
		framesPerSecond = 0;
	}

	return fps;
}

void Engine::KeyPressEvent(unsigned char key)
{
	switch (key)
	{
	case 0: // A
		m_keyA = true;
		break;
	case 3: // D
		m_keyD = true;
		break;
	case 18: // S
		m_keyS = true;
		break;
	case 22: // W
		m_keyW = true;
		break;
	case 36: // ESC
		Stop();
		break;
	case 57: // Space
		if (m_jump)
			m_velocity = 20.f;
		break;
	case 94: // F10
		SetFullscreen(!IsFullscreen());
		break;
	default:
		std::cout << "Unhandled key: " << (int)key << std::endl;
	}
}

void Engine::KeyReleaseEvent(unsigned char key)
{
	switch (key)
	{
	case 0: // A
		m_keyA = false;
		break;
	case 3: // D
		m_keyD = false;
		break;
	case 18: // S
		m_keyS = false;
		break;
	case 22: // W
		m_keyW = false;
		break;
	case 24: // Y
		m_wireframe = !m_wireframe;
		if (m_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
}

void Engine::MouseMoveEvent(int x, int y)
{
	// Centrer la souris seulement si elle n'est pas déjà centrée
	// Il est nécessaire de faire la vérification pour éviter de tomber
	// dans une boucle infinie où l'appel à CenterMouse génère un
	// MouseMoveEvent, qui rapelle CenterMouse qui rapelle un autre
	// MouseMoveEvent, etc
	if (x == (Width() / 2) && y == (Height() / 2))
		return;

	MakeRelativeToCenter(x, y);

	m_player.TurnLeftRight(x);
	m_player.TurnTopBottom(y);
	CenterMouse();
}

void Engine::MousePressEvent(const MOUSE_BUTTON& button, int x, int y)
{
	Vector3f pos = m_player.Position();
	Chunk* c = ChunkAt(pos.x, pos.y, pos.z);
	switch (button)
	{
	case 1:
		GetBlocAtCursor();
		if (m_currentBlock.x > 0 && m_currentBlock.y > 0 && m_currentBlock.z > 0)
			c->RemoveBlock(m_currentBlock.x, m_currentBlock.y, m_currentBlock.z);
		break;
	case 4:
		if (m_currentFaceNormal.y > 0)
			c->SetBlock(m_currentBlock.x, m_currentBlock.y + 1, m_currentBlock.z, BTYPE_DIRT);
		break;
	default:
		break;
	}
}

void Engine::MouseReleaseEvent(const MOUSE_BUTTON& button, int x, int y)
{
}

bool Engine::LoadTexture(Texture& texture, const std::string& filename, bool stopOnError)
{
	texture.Load(filename);
	if (!texture.IsValid())
	{
		std::cerr << "Unable to load texture (" << filename << ")" << std::endl;
		if (stopOnError)
			Stop();

		return false;
	}

	return true;
}

void Engine::PrintText(unsigned int x, unsigned int y, const std::string& t)
{
	glLoadIdentity();
	glTranslated(x, y, 0);
	for (unsigned int i = 0; i < t.length(); ++i)
	{
		float left = (float)((t[i] - 32) % 16) / 16.0f;
		float top = (float)((t[i] - 32) / 16) / 16.0f;
		top += 0.5f;
		glBegin(GL_QUADS);
		glTexCoord2f(left, 1.0f - top - 0.0625f);
		glVertex2f(0, 0);
		glTexCoord2f(left + 0.0625f, 1.0f - top - 0.0625f);
		glVertex2f(12, 0);
		glTexCoord2f(left + 0.0625f, 1.0f - top);
		glVertex2f(12, 12);
		glTexCoord2f(left, 1.0f - top);
		glVertex2f(0, 12);
		glEnd();
		glTranslated(8, 0, 0);
	}
}

template <class T>
Chunk* Engine::ChunkAt(T x, T y, T z) const
{
	int cx = (int)x / CHUNK_SIZE_X;
	int cz = (int)z / CHUNK_SIZE_Z;

	if ((x > VIEW_DISTANCE * 2 || z > VIEW_DISTANCE * 2) || (x < 0 || z < 0))
		return nullptr;

	return m_array2d.Get(cx, cz);
}


template <class T>
Chunk* Engine::ChunkAt(const Vector3<T>& pos) const
{
	return ChunkAt(pos.x, pos.y, pos.z);
}

template <class T>
BlockType Engine::BlockAt(T x, T y, T z, BlockType defaultBlockType) const
{
	Chunk* c = ChunkAt(x, y, z);

	if (!c)
		return defaultBlockType;

	int bx = (int)x % CHUNK_SIZE_X;
	int by = (int)y % CHUNK_SIZE_Y;
	int bz = (int)z % CHUNK_SIZE_Z;

	return c->GetBlock(bx, by, bz);
}
