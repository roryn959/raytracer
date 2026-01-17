#pragma once

#include <cstddef>
#include <random>
#include <thread>

#include "Core/Axis.h"
#include "Core/Collision.h"
#include "Core/Colour.h"
#include "Core/Cuboid.h"
#include "Core/Material.h"
#include "Core/Plane.h"
#include "Core/Ray.h"
#include "View/Canvas.h"
#include "World.h"

#define DIFFUSE_DAMPEN_FACTOR 0.9f
#define EPSILON 1e-4
#define MAX_COLLISIONS 7

const int NUM_THREADS = std::thread::hardware_concurrency();
const int RAYS_PER_THREAD = NUM_PIXELS / NUM_THREADS;


class CpuExecutor {
public:
	CpuExecutor(const World& world);
	~CpuExecutor();

	void TraceRays(uint32_t* pixelBuffer);

private:
	void RefreshAccumulator();

	float Rand_11();
	float Rand01();

	Vector Scatter(const Vector& normal);

	bool ShouldSpectralReflect(float reflectionIndex);

	void CalculateNextRay(Ray& ray, const Collision& collision);

	bool TryCollision(const Plane& plane, const Ray& ray, Collision& bestCollision);
	bool TryCollision(const Cuboid& cuboid, const Ray& ray, Collision& bestCollision);

	Ray GenerateInitialRay(size_t i);

	void TraceRay(Colour* buffer, size_t i);
	void TraceRayRange(Colour* buffer, size_t start, size_t end);

	const World&	m_world;
	Colour* 		m_accumulator;
	size_t			m_accumulationCount;
};