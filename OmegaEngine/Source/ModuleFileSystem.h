#pragma once
#include "Module.h"
#include "Globals.h"

#define ASSETS_PATH "Assets/";
#define LIBRARY_PATH "Library/";
#define LIB_MESH_PATH "Library/Meshes/";
#define LIB_MATERIAL_PATH "Library/Materials/";

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(Application* app, bool start_enabled = true);
	~ModuleFileSystem();
	
	bool Init();

	uint FileToBuffer(const char* filePath, char** fileBuffer)const;
	uint ImportFileToAssets(const char* fileName);
	void SaveFile(const char* filePath, char* buffer, uint size);

};
