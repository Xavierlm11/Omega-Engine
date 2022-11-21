#include "Globals.h"
#include "External/MathGeoLib/include/Math/float3.h"
#include "Glew/include/glew.h"
#include "Assimp/include/scene.h"
#include "MaterialData.h"
#include "glmath.h"
//#include "GameObject.h"
//#include "Component.h"
//
//class ModelData {
//
//public:
//	ModelData();
//	~ModelData();
//
//	MeshData mesh;
//};

class MeshData {

public:
	MeshData();
	~MeshData();

	void LoadBuffers();
	void DrawMesh(GLuint textureID, mat4x4 mat);
	void UnloadMesh();

	uint id_indices = 0; // index in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // unique vertex in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_textureCoords = 0;
	uint num_textureCoords = 0;
	float* textureCoords = nullptr;

	bool hasLoadedBuffers;

	MaterialData* material = nullptr;

	//GameObject* obj;
};
