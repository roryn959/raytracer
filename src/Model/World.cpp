#include "Model/World.h"

#if GPU_BUILD
#include "Model/GpuExecutor.h"
#else
#include "Model/CpuExecutor.h"
#endif

World::World() :
	m_planes{},
	m_cuboids{},
	m_cuboidLights{},
	m_viewpoint{ Vector{0.0f, 0.0f, 0.0f}, Vector{ 0.0f, 0.0f, 0.0f } },
	m_velocity{ 0.0f, 0.0f, 0.0f }
{
	Plane leftWall{ AXIS::X, -1.0f, Material{ false, COLOUR_RED, 0.00f } };
	m_planes.push_back(leftWall);

	Plane rightWall{ AXIS::X, 1.0f, Material{ false, COLOUR_BLUE, 0.00f } };
	m_planes.push_back(rightWall);

	Plane bottomWall{ AXIS::Y, -1.0f, Material{ false, COLOUR_GREEN, 0.00f } };
	m_planes.push_back(bottomWall);

	Plane topWall{ AXIS::Y, 1.0f, Material{ false, COLOUR_YELLOW, 0.00f } };
	m_planes.push_back(topWall);

	Plane backWall{ AXIS::Z, -1.0f, Material{ false, COLOUR_PINK, 0.00f } };
	m_planes.push_back(backWall);

	Plane frontWall{ AXIS::Z, 1.0f, Material{ false, COLOUR_TURQUOISE, 0.00f } };
	m_planes.push_back(frontWall);

	Cuboid light1{ Vector{ -1.0f, -0.5f, 0.75f }, Vector{ -0.75f, -0.25f, 1.0f }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	m_cuboidLights.push_back(light1);

	Cuboid light2{ Vector{ 0.75f, 0.75f, 0.75f }, Vector{ 1.0f, 1.0f, 1.0f }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	m_cuboidLights.push_back(light2);

	Cuboid light3{ Vector{ -0.35f, -0.35f, -0.9f }, Vector{ 0.35f, 0.35f, -0.9f }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	m_cuboidLights.push_back(light3);

	Cuboid mirror1{ Vector{ -0.99f, -0.60f, 0.6f}, Vector{ -0.3f, -0.65f, 0.99f }, Material{ false, COLOUR_BLACK, 1.0f } };
	m_cuboids.push_back(mirror1);

	Cuboid slab1{ Vector{ -0.99f, 0.15f, 0.6f}, Vector{ -0.3f, 0.20f, 0.99f }, Material{ false, COLOUR_BLACK, 0.0f } };
	m_cuboids.push_back(slab1);
}

void World::ProcessTimeTick(float t, Executor& executor) {
	ShiftPosition(t * WALK_SPEED * m_velocity);

	if (IsMoving())
		executor.RefreshAccumulator();
}