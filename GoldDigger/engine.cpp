#include "engine.h"
#include "transformation.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Engine::Engine() : m_player(Vector3f(50.f, 67.8f, 50.f), 0.f, 0.f), m_textureAtlas(BTYPE_LAST), m_array2d((VIEW_DISTANCE * 2) / CHUNK_SIZE_X, (VIEW_DISTANCE * 2) / CHUNK_SIZE_Z)
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
	LoadTexture(m_textureCrosshair, TEXTURE_PATH "cross.bmp");
	LoadTexture(m_textureFont, TEXTURE_PATH "font.bmp");
	LoadTexture(m_textureHotbar, TEXTURE_PATH "toolbar.bmp");
	LoadTexture(m_textureDirt, TEXTURE_PATH "dirt.bmp");
	LoadTexture(m_textureStone, TEXTURE_PATH "stone.bmp");
	LoadTexture(m_textureBrick, TEXTURE_PATH "bricks.bmp");
	LoadTexture(m_textureGrass, TEXTURE_PATH "grass.bmp");




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

	StartInventory();
}

void Engine::UnloadResource()
{
	for (int x = 0; x < (VIEW_DISTANCE * 2) / CHUNK_SIZE_X; x++)
		for (int z = 0; z < (VIEW_DISTANCE * 2) / CHUNK_SIZE_Z; z++)
			delete m_array2d.Get(x, z);
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
	Vector3f delta = m_player.SimulateMove(m_keyW, m_keyS, m_keyA, m_keyD, m_sprint, elapsedTime);
	BlockType block_under = BlockAt(pos.x, pos.y + delta.y - 1.9f, pos.z, BTYPE_AIR);

	// Gravit�
	delta.y -= elapsedTime * 6;

	delta.y += m_velocity * elapsedTime;
	m_velocity *= 0.9f;

	BlockType bt1, bt2, bt3;
	// Collision par rapport au d�placement en x:
	bt1 = BlockAt(pos.x + delta.x, pos.y, pos.z, BTYPE_AIR);
	bt2 = BlockAt(pos.x + delta.x, pos.y - 0.9f, pos.z, BTYPE_AIR);
	bt3 = BlockAt(pos.x + delta.x, pos.y - 1.7f, pos.z, BTYPE_AIR);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR)
		delta.x = 0;
	// Collision par rapport au d�placement en y:
	bt1 = BlockAt(pos.x, pos.y + delta.y, pos.z, BTYPE_AIR);
	bt2 = BlockAt(pos.x, pos.y + delta.y - 0.9f, pos.z, BTYPE_AIR);
	bt3 = BlockAt(pos.x, pos.y + delta.y - 1.7f, pos.z, BTYPE_AIR);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR)
		delta.y = 0;
	// Collision par rapport au d�placement en z:
	bt1 = BlockAt(pos.x, pos.y, pos.z + delta.z, BTYPE_AIR);
	bt2 = BlockAt(pos.x, pos.y - 0.9f, pos.z + delta.z, BTYPE_AIR);
	bt3 = BlockAt(pos.x, pos.y - 1.7f, pos.z + delta.z, BTYPE_AIR);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR)
		delta.z = 0;

	if (block_under != BTYPE_AIR)
		m_jump = true;
	else
		m_jump = false;

	if (!m_inventoryOpen)
	{
		pos += delta;
		m_player.SetPosition(pos);
	}

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
	GetBlocAtCursor();
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
	std::string sbtype = "";
	switch (m_selectedBlockType)
	{
	case 0:
		sbtype = "None";
		break;
	case 1:
		sbtype = "Dirt";
		break;
	case 2:
		sbtype = "Grass";
		break;
	case 3:
		sbtype = "Stone";
		break;
	case 4:
		sbtype = "Bricks";
		break;
	default:
		break;
	}
	ss << "Selected block : " << sbtype;
	PrintText(Width() / 2 - 85, Height() / 4, ss.str());
	ss.str("");
	ss << " Position : " << m_player.Position(); // IMPORTANT : on utilise l � operateur << pour afficher la position
	PrintText(10, 10, ss.str());
	// Affichage de la barre d'outils
	m_textureHotbar.Bind();
	static const int vertHotbarSize = 100;
	static const int horizHotbarSize = 580;
	static const int vertSlotSize = 40;
	static const int horizSlotSize = 43;
	glLoadIdentity();
	glTranslated(Width() / 2 - horizHotbarSize / 2, Height() / 2 - vertHotbarSize / 2 - Height() / 3, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2i(0, 0);
	glTexCoord2f(1, 0);
	glVertex2i(horizHotbarSize, 0);
	glTexCoord2f(1, 1);
	glVertex2i(horizHotbarSize, vertHotbarSize);
	glTexCoord2f(0, 1);
	glVertex2i(0, vertHotbarSize);
	glEnd();
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

	// Affichage des blocks dans la barre d'outils
	glDisable(GL_BLEND);
	for (int i = 1; i <= 9; i++)
	{
		switch (m_inventory.BlockAtIndex(i - 1))
		{
		case BTYPE_DIRT:
			m_textureDirt.Bind();
			break;
		case BTYPE_GRASS:
			m_textureGrass.Bind();
			break;
		case BTYPE_STONE:
			m_textureStone.Bind();
			break;
		case BTYPE_BRICK:
			m_textureBrick.Bind();
			break;
		default:
			break;
		}
		if (m_inventory.BlockAtIndex(i - 1) != BTYPE_AIR)
		{
			glLoadIdentity();
			glTranslated((Width() / 2) - 271 + (i * (horizSlotSize + 6)), (Height() / 2 - vertHotbarSize / 2 - Height() / 3) + 30, 0);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2i(0, 0);
			glTexCoord2f(1, 0);
			glVertex2i(horizSlotSize, 0);
			glTexCoord2f(1, 1);
			glVertex2i(horizSlotSize, vertSlotSize);
			glTexCoord2f(0, 1);
			glVertex2i(0, vertSlotSize);
			glEnd();
		}
		if (m_inventoryOpen)
		{

		}
	}

	// Affichage de la case qui affiche la case s�lectionn�e dans la barre d'outils
	//switch (m_selectedToolbarSlot)
	//{
	//case 1:
	//	glLoadIdentity();
	//	glTranslated((Width() / 2) - 271 + (m_selectedToolbarSlot * (horizSlotSize + 6)), (Height() / 2 - vertHotbarSize / 2 - Height() / 3) + 30, 0);
	//	glBegin(GL_QUADS);
	//	glTexCoord2f(0, 0);
	//	glVertex2i(0, 0);
	//	glTexCoord2f(1, 0);
	//	glVertex2i(horizSlotSize, 0);
	//	glTexCoord2f(1, 1);
	//	glVertex2i(horizSlotSize, vertSlotSize);
	//	glTexCoord2f(0, 1);
	//	glVertex2i(0, vertSlotSize);
	//	glEnd();
	//	break;
	//default:
	//	break;
	//}

	glEnable(GL_LIGHTING);
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

		// Front et back:
		if (EqualWithEpsilon((float)posZ, (float)m_currentBlock.z))
			m_currentFaceNormal.z = -1;
		else if (EqualWithEpsilon((float)posZ, (float)m_currentBlock.z + 1.f))
			m_currentFaceNormal.z = 1;
		else if (EqualWithEpsilon((float)posX, (float)m_currentBlock.x))
			m_currentFaceNormal.x = -1;
		else if (EqualWithEpsilon((float)posX, (float)m_currentBlock.x + 1.f))
			m_currentFaceNormal.x = 1;
		else if (EqualWithEpsilon((float)posY, (float)m_currentBlock.y))
			m_currentFaceNormal.y = -1;
		else if (EqualWithEpsilon((float)posY, (float)m_currentBlock.y + 1.f))
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

void Engine::StartInventory()
{
	//m_inventory.AddBlock(BTYPE_DIRT);
	//m_inventory.AddBlock(BTYPE_GRASS);
	m_inventory.AddBlock(BTYPE_STONE);
	m_inventory.AddBlock(BTYPE_BRICK);
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
	case 4: // E
		m_inventoryOpen = !m_inventoryOpen;
		break;
	case 5: // F
		break;
	case 18: // S
		m_keyS = true;
		break;
	case 22: // W
		m_keyW = true;
		break;
	case 27: // 1
		m_selectedToolbarSlot = 1;
		m_selectedBlockType = m_inventory.BlockAtIndex(0);
		break;
	case 28: // 2
		m_selectedToolbarSlot = 2;
		m_selectedBlockType = m_inventory.BlockAtIndex(1);
		break;
	case 29: // 3
		m_selectedToolbarSlot = 3;
		m_selectedBlockType = m_inventory.BlockAtIndex(2);
		break;
	case 30: // 4
		m_selectedToolbarSlot = 4;
		m_selectedBlockType = m_inventory.BlockAtIndex(3);
		break;
	case 31: // 5
		m_selectedToolbarSlot = 5;
		m_selectedBlockType = m_inventory.BlockAtIndex(4);
		break;
	case 32: // 6
		m_selectedToolbarSlot = 6;
		m_selectedBlockType = m_inventory.BlockAtIndex(5);
		break;
	case 33: // 7
		m_selectedToolbarSlot = 7;
		m_selectedBlockType = m_inventory.BlockAtIndex(6);
		break;
	case 34: // 8
		m_selectedToolbarSlot = 8;
		m_selectedBlockType = m_inventory.BlockAtIndex(7);
		break;
	case 35: // 9
		m_selectedToolbarSlot = 9;
		m_selectedBlockType = m_inventory.BlockAtIndex(8);
		break;
	case 36: // ESC
		Stop();
		break;
	case 38: // Shift
		m_sprint = true;
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
	case 38: // Shift
		m_sprint = false;
		break;
	}
}

void Engine::MouseMoveEvent(int x, int y)
{
	if (!m_inventoryOpen)
	{
		// Centrer la souris seulement si elle n'est pas d�j� centr�e
		// Il est n�cessaire de faire la v�rification pour �viter de tomber
		// dans une boucle infinie o� l'appel � CenterMouse g�n�re un
		// MouseMoveEvent, qui rapelle CenterMouse qui rapelle un autre
		// MouseMoveEvent, etc
		if (x == (Width() / 2) && y == (Height() / 2))
			return;

		MakeRelativeToCenter(x, y);

		m_player.TurnLeftRight(x);
		m_player.TurnTopBottom(y);
		CenterMouse();
	}
}

void Engine::MousePressEvent(const MOUSE_BUTTON& button, int x, int y)
{
	if (!m_inventoryOpen)
	{
		Vector3f pos = m_currentBlock;
		Chunk* c = ChunkAt(pos.x, pos.y, pos.z);
		int posx = pos.x;
		int posy = pos.y;
		int posz = pos.z;
		switch (button)
		{
		case 1:
			if (posx >= 0 && posy >= 0 && posz >= 0)
			{
				m_brokenBlock = c->GetBlock(posx % CHUNK_SIZE_X, posy, posz % CHUNK_SIZE_Z);
				c->RemoveBlock(posx % CHUNK_SIZE_X, posy, posz % CHUNK_SIZE_Z);
				m_inventory.AddBlock(m_brokenBlock);
				m_selectedBlockType = m_inventory.BlockAtIndex(m_selectedToolbarSlot);
			}
			break;
		case 4:
			if (m_selectedBlockType != BTYPE_AIR)
				if (posx >= 0 && posy >= 0 && posz >= 0)
				{
					if (m_currentFaceNormal.x > 0)
						c->SetBlock((posx + 1) % CHUNK_SIZE_X, posy, posz % CHUNK_SIZE_Z, m_selectedBlockType);
					else if (m_currentFaceNormal.x < 0)
						c->SetBlock((posx - 1) % CHUNK_SIZE_X, posy, posz % CHUNK_SIZE_Z, m_selectedBlockType);
					else if (m_currentFaceNormal.y < 0)
						c->SetBlock(posx % CHUNK_SIZE_X, posy - 1, posz % CHUNK_SIZE_Z, m_selectedBlockType);
					else if (m_currentFaceNormal.y > 0)
						c->SetBlock(posx % CHUNK_SIZE_X, posy + 1, posz % CHUNK_SIZE_Z, m_selectedBlockType);
					else if (m_currentFaceNormal.z > 0)
						c->SetBlock(posx % CHUNK_SIZE_X, posy, (posz + 1) % CHUNK_SIZE_Z, m_selectedBlockType);
					else if (m_currentFaceNormal.z < 0)
						c->SetBlock(posx % CHUNK_SIZE_X, posy, (posz - 1) % CHUNK_SIZE_Z, m_selectedBlockType);
					m_inventory.RemoveFromInvent(m_selectedToolbarSlot - 1);
					m_selectedBlockType = BTYPE_AIR;
				}
			break;
		default:
			break;
		}
	}
	else
	{

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