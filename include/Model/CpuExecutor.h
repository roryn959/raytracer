#pragma once

#include <cstddef>

#include "Core/Colour.h"
#include "Core/Cuboid.h"
#include "View/Canvas.h"
#include "World.h"

#define MAX_COLLISIONS 10


enum class AXIS {
	X,
	Y,
	Z
};

struct Ray {
	Vector m_pos;
	Vector m_vel;
};

struct Collision {
	float 	m_t;
	bool 	m_final;
	Colour	m_colour;
	Ray		m_nextRay;
};


class CpuExecutor {
public:
	CpuExecutor(const World& world);

	void TraceRays(uint32_t* buffer);

private:
	void TryCollision(const Cuboid& cuboid, const Ray& ray, Collision& collision);

	void TraceRay(uint32_t* buffer, size_t i);

	const World& m_world;
};