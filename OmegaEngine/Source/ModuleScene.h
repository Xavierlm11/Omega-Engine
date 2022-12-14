#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Component.h"
#include "PhysBody3D.h"

class ModuleScene : public Module
{
public:

	enum class Shapes {
		NONE,
		HOUSE,
		CUBE,
		SPHERE,
		PYRAMID,
		CYLINDER
	};

	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool SaveScene();
	bool SaveSceneAtPlay();
	bool LoadScene();
	bool LoadSceneAtPlay();

	void LoadSpecific(uint id);

	void CleanMeshes(std::vector<MeshData*>* meshes);

	GameObject* CreateGO( const char* name, GameObject* parent);
	void CreateGORoot(GameObject* obj, const char* name, GameObject* parent);
	void AddGOList(GameObject* obj);

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	bool house_loaded = false;
	bool cube_loaded = false;
	bool sphere_loaded = false;
	bool pyramid_loaded = false;
	bool cylinder_loaded = false;

	const aiScene* modelScene;
	MeshData * modelMesh;

	std::vector<ModelData*> models;
	
	GameObject* RootParent = nullptr;

	std::vector <GameObject*> ListGO;

	std::vector <Resource*> resourceList;
	std::vector <ModelData*> modelList;
};
