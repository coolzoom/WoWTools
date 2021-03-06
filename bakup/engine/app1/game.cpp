#include "game.h"
#include "Engine.h"
#include "function3d.h"
#include "CMeshManager.h"
#include "CMeshSceneNode.h"

CGame* g_pGame = nullptr;

void MyMessageHandler::onExit(window_type hwnd)
{
	g_pGame->m_bExit = true;
}

void MyMessageHandler::onSize(window_type hwnd, int width, int height)
{
	if (width == 0 || height == 0)					//minimized, stop
	{
		g_pGame->m_bBackMode = true;
	}
	else
	{
		g_pGame->m_bBackMode = false;

		dimension2d screenSize((uint32_t)width, (uint32_t)height);
		g_Engine->onWindowResized(screenSize);

		if (g_pGame->m_pScene)
			g_pGame->m_pScene->onScreenResize(screenSize);
	}
}

CGame::CGame()
{
	m_bExit = false;
	m_bBackMode = false;
	m_pScene = nullptr;
}

CGame::~CGame()
{
	delete m_pScene;
	m_pScene = nullptr;
}

void CGame::createScene()
{
	m_pScene = new CScene("default");

	const auto& dim = g_Engine->getDriver()->getDisplayMode();
	float aspect = (float)dim.width / dim.height;
	vector3df camPos(0, 5, -10);
	vector3df camDir = f3d::normalize(vector3df(0, 0, 0) - camPos);
	m_pScene->init3DCamera(PI / 4, aspect, 1, 2000.0f, camPos, camDir, vector3df::UnitY());

	m_pScene->init2DCamera(dim, 0, 1);

	//add mesh
	g_Engine->getMeshManager()->addGridLineMesh("$grid20", 20, 1, SColor(128, 128, 128), SColor::Black());
	g_Engine->getMeshManager()->addSphere("$sphere", 1, 20, 11, SColor::White());

	{
		CMeshSceneNode* gridSceneNode = m_pScene->addMeshSceneNode("$grid20");
		gridSceneNode->setMesh(g_Engine->getMeshManager()->getMesh("$grid20"));
		SMaterial& mat = gridSceneNode->getMeshRenderer()->getMaterial();
		mat.RenderQueue = (ERQ_GEOMETRY - 50);
	}

	{
		CMeshSceneNode* sphereSceneNode = m_pScene->addMeshSceneNode("$sphere");
		sphereSceneNode->setMesh(g_Engine->getMeshManager()->getMesh("$sphere"));
		SMaterial& mat = sphereSceneNode->getMeshRenderer()->getMaterial();
		mat.setMainTexture(g_Engine->getDriver()->getTextureWhite());
		sphereSceneNode->getTransform()->setPos(vector3df(1, 1, 0));
	}
}

void CGame::update()
{
	processInput();
}

void CGame::onKeyMessage(window_type hwnd, E_INPUT_MESSAGE message, int key)
{
	if (message == Key_Up)
	{

	}
}

void createGame()
{
	g_pGame = new CGame;
}

void destroyGame()
{
	delete g_pGame;
	g_pGame = nullptr;
}
