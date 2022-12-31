#pragma once
#include "Component.h"
#include "glmath.h"
#include "ImGui/imgui.h"
#include <MathGeoLib/include/Math/float3.h>

#include "GameObject.h"



class CTransform : public Component
{
public:

	CTransform(GameObject* obj);
	virtual ~CTransform();

	void Enable() override { active = true; }

	 void Disable()override { active = false; }

	 bool IsEnable()override { return active ; }

	bool active = true;

	void Update() override;

	void TransformMatrix(float3 _pos, float3 _rot, float3 _scl);

	void OnInspector() override;

	mat4x4 GlobalMatrix();

	float3 GetPos() { return pos; }
	float3 GetRot() { return rot; }
	float3 GetScale() { return scl; }

	void SetPos(float3 _pos) { pos = _pos; TransformMatrix(pos, rot,scl); }
	void SetRot(float3 _rot) { rot = _rot; TransformMatrix(pos, rot, scl);}
	void SetScale(float3 _scl) { scl = _scl; TransformMatrix(pos, rot, scl); }

	//float4x4 GetMatrix() { return matrix; }

	/*float4x4 GetGlobal()
	{
		if (GO->GetParent() == nullptr)
		{
			return;
		}
		return ;
	}*/

public:

	float3 pos={0,0,0};
	float3 rot = { 0,0,0 };
	float3 scl = { 1,1,1 };
	
	mat4x4 matrix;
private:
	float3 npos = { 0,0,0 };
	float3 nrot = { 0,0,0 };
	float3 nscl = { 1,1,1 };
};

