#include <iostream>
#include <list>

#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "wowDatabase.h"
#include "function.h"

#include "wowDbFile.h"
#include "wowWMOFile.h"
#include "wowM2File.h"

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testWowGameFile();

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	testWowGameFile();

	getchar();
	return 0;
}

void testWowGameFile()
{
	//CFileSystem* fs = new CFileSystem(R"(D:\World Of Warcraft 81)");
	CFileSystem* fs = new CFileSystem(R"(E:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);

	if (!wowEnv->init("wow_classic"))
		printf("init fail!\n");
	else
		printf("init success!\n");

	/*
	if (!wowEnv->loadCascListFiles())
		printf("listfile fail!\n");
	else
		printf("listfile success!\n");

	wowEnv->buildWmoFileList();
	*/

	//wmo test
	{
		const char* path = "World\\wmo\\kalimdor\\ogrimmar\\ogrimmar.wmo";
		wowWMOFile* wmoFile = new wowWMOFile(wowEnv);
		wmoFile->loadFile(path);
		delete wmoFile;

		printf("wmo load success! %s\n", path);
	}

	//m2 test
	{
		const char* path = "Character\\HUMAN\\Male\\humanmale.m2";
		wowM2File* m2File = new wowM2File(wowEnv);
		m2File->loadFile(path);
		delete m2File;

		printf("m2 load success! %s\n", path);
	}

	delete wowEnv;
	delete fs;
}