#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#include"MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Geometry/Frustum.h"


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void UpdateFrustum();
	void Rotate();
	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void Zoom();
	void Orbit(float3 target);

	float* GetViewMatrix();
	float4x4 *GetViewMatrixOpenGL();
	float4x4 *GetProjectionMatrixOpenGL();

	void CalculateViewMatrix();
	void CalculateViewMatrixOpenGL();

	void CalculateProjectionMatrixOpenGL();

public:
	
	vec3 X, Y, Z, Position, Reference;
	Color background;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
	float4x4* ViewMatrixOpenGL;
	float4x4* ProjectionMatrixOpenGL;
	Frustum cameraFrustum;

	float nextRot = 0;

	bool isMovingAround = false;
};