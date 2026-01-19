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
	m_spheres{},
	m_viewpoint{ Vector{0.0f, 0.0f, 0.0f}, Vector{ 0.0f, 0.0f, 0.0f } },
	m_velocity{ 0.0f, 0.0f, 0.0f },
	m_viewChanged{false}
{
	Plane leftWall{ AXIS::X, -0.4f, Material{ false, COLOUR_BLUE, 0.0f } };
	m_planes.push_back(leftWall);

	Plane rightWall{ AXIS::X, 0.4f, Material{ false, COLOUR_RED, 0.00f } };
	m_planes.push_back(rightWall);

	Plane bottomWall{ AXIS::Y, -0.1f, Material{ false, Colour{ 1.0f, 0.45f, 0.45f, 0.45f }, 0.02f } };
	m_planes.push_back(bottomWall);

	Plane topWall{ AXIS::Y, 0.4f, Material{ false, Colour{ 1.0f, 0.25f, 0.25f, 0.25f }, 0.00f } };
	m_planes.push_back(topWall);

	Plane backWall{ AXIS::Z, -0.6f, Material{ false, Colour{ 1.0f, 0.85f, 0.85f, 0.80f }, 0.00f } };
	m_planes.push_back(backWall);

	Plane frontWall{ AXIS::Z, 0.6f, Material{ false, Colour{ 1.0f, 0.85f, 0.85f, 0.80f }, 0.00f } };
	m_planes.push_back(frontWall);

	Cuboid light1{ Vector{ -0.35f, 0.39f, -0.5f }, Vector{ -0.25f, 0.4f, 0.5 }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	m_cuboidLights.push_back(light1);

	Cuboid light2{ Vector{ -0.2f, 0.39f, -0.5f }, Vector{ -0.1f, 0.39f, 0.5f }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	//m_cuboidLights.push_back(light2);

	Cuboid light3{ Vector{ -0.05f, 0.39f, -0.5f }, Vector{ 0.05f, 0.4f, 0.5 }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	m_cuboidLights.push_back(light3);

	Cuboid light4{ Vector{ 0.1f, 0.39f, -0.5f }, Vector{ 0.2f, 0.39f, 0.5f }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	//m_cuboidLights.push_back(light4);

	Cuboid light5{ Vector{ 0.25f, 0.39f, -0.5f }, Vector{ 0.35f, 0.4f, 0.5 }, Material{ true, COLOUR_WARM_LIGHT, 0.0f } };
	m_cuboidLights.push_back(light5);

	Sphere sphere1{ Vector{ 0.0f, 0.0f, 0.15f }, 0.1f, Material{ false, COLOUR_WHITE, 1.0f } };
	m_spheres.push_back(sphere1);

	Sphere sphere2{ Vector{ -0.15f, 0.0f, 0.0f }, 0.1f, Material{ false, COLOUR_RED, 0.15f } };
	m_spheres.push_back(sphere2);

	Sphere sphere3{ Vector{ 0.15f, 0.0f, 0.0f }, 0.1f, Material{ false, COLOUR_BLUE, 0.15f } };
	m_spheres.push_back(sphere3);

	Sphere sphere4{ Vector{ 0.0f, -0.1f + 0.05f, 0.0f }, 0.05f, Material{ false, COLOUR_WHITE, 0.25f } };
	m_spheres.push_back(sphere4);

	Sphere sphere5{ Vector{ 0.0f, 0.0f, -0.15f }, 0.1f, Material{ false, COLOUR_BLACK, 0.05f } };
	m_spheres.push_back(sphere5);

	// 2 -> 5
	// 0.6 1.5

}

void World::ProcessTimeTick(float t, Executor& executor) {
	ShiftPosition(t * WALK_SPEED * m_velocity);

	if (IsMoving() || m_viewChanged) {
		executor.RefreshAccumulator();
	}
}