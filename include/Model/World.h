#pragma once

#include <vector>
#include <iostream>

#include "Core/Cuboid.h"
#include "Core/Plane.h"
#include "Core/Sphere.h"
#include "Core/Vector.h"
#include "Core/Viewpoint.h"

#define WALK_SPEED 0.5f

#define GPU_BUILD 1

#if GPU_BUILD
class GpuExecutor;
using Executor = GpuExecutor;
#else
class CpuExecutor;
using Executor = CpuExecutor;
#endif

class World {
public:
	World();

	inline void ShiftView(float theta, float phi) {
		m_viewpoint.m_direction.m_x += theta;
		m_viewpoint.m_direction.m_y += phi;
		m_viewpoint.m_direction.m_y = std::max(m_viewpoint.m_direction.m_y, (float) -M_PI_4);
		m_viewpoint.m_direction.m_y = std::min(m_viewpoint.m_direction.m_y, (float) M_PI_4);
		m_viewChanged = true; 
	}
	inline void ShiftPosition(Vector direction) { m_viewpoint.m_position = m_viewpoint.m_position + direction; }
	
	inline void MoveLeft() 		{ if (m_velocity.m_x != WALK_SPEED) 	m_velocity.m_x = -WALK_SPEED; }
	inline void MoveRight() 	{ if (m_velocity.m_x != -WALK_SPEED) 	m_velocity.m_x = WALK_SPEED; }
	inline void MoveDown() 		{ if (m_velocity.m_y != WALK_SPEED) 	m_velocity.m_y = -WALK_SPEED; }
	inline void MoveUp() 		{ if (m_velocity.m_y != -WALK_SPEED)	m_velocity.m_y = WALK_SPEED; }
	inline void MoveBackward() 	{ if (m_velocity.m_z != WALK_SPEED) 	m_velocity.m_z = -WALK_SPEED; }
	inline void MoveForward() 	{ if (m_velocity.m_z != -WALK_SPEED) 	m_velocity.m_z = WALK_SPEED; }

	inline void UnMoveLeft() 		{ if (m_velocity.m_x == -WALK_SPEED)	m_velocity.m_x = 0.0f; }
	inline void UnMoveRight() 		{ if (m_velocity.m_x == WALK_SPEED) 	m_velocity.m_x = 0.0f; }
	inline void UnMoveDown() 		{ if (m_velocity.m_y == -WALK_SPEED) 	m_velocity.m_y = 0.0f; }
	inline void UnMoveUp() 			{ if (m_velocity.m_y == WALK_SPEED) 	m_velocity.m_y = 0.0f; }
	inline void UnMoveBackward() 	{ if (m_velocity.m_z == -WALK_SPEED) 	m_velocity.m_z = 0.0f; }
	inline void UnMoveForward() 	{ if (m_velocity.m_z == WALK_SPEED) 	m_velocity.m_z = 0.0f; }

	inline bool IsMoving() const { return !(m_velocity.m_x == 0.0f && m_velocity.m_y == 0.0f && m_velocity.m_z == 0.0f); }
	inline bool HasViewChanged() const { return m_viewChanged; }
	void 		SetViewChanged(bool changed) { m_viewChanged = changed; }

	void ProcessTimeTick(float t, Executor& executor);

	inline const std::vector<Cuboid>& 	GetCuboidLights() const { return m_cuboidLights; }
	inline const std::vector<Plane>&	GetPlanes() const { return m_planes; }
	inline const std::vector<Cuboid>& 	GetCuboids() const { return m_cuboids; }
	inline const std::vector<Sphere>&	GetSpheres() const { return m_spheres; }
	inline const Viewpoint&				GetViewpoint() const { return m_viewpoint; }

private:
	std::vector<Plane>		m_planes;
	std::vector<Cuboid> 	m_cuboids;
	std::vector<Cuboid>		m_cuboidLights;
	std::vector<Sphere>		m_spheres;

	Vector 					m_velocity;
	Viewpoint				m_viewpoint;
	bool					m_viewChanged;
};