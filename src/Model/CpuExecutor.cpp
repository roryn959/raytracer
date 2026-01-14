#include "Model/CpuExecutor.h"

#include <iostream>

CpuExecutor::CpuExecutor(const World& world) :
	m_world{world}
{}

void CpuExecutor::TraceRays(uint32_t* buffer) {
	for (int i = 0; i < NUM_PIXELS; ++i)
		TraceRay(buffer, i);
}

void CpuExecutor::TryCollision(const Cuboid& cuboid, const Ray& ray, Collision& bestCollision) {
	// Returns 0 if there is no collision, because we will be throwing away
	// t values at 0 anyway to avoid counting a recent bounce as a valid next collision

	const Vector& min = cuboid.m_min;
	const Vector& max = cuboid.m_max;

	float xT1 = (min.m_x - ray.m_pos.m_x) / ray.m_vel.m_x;
	float xT2 = (max.m_x - ray.m_pos.m_x) / ray.m_vel.m_x;

	float yT1 = (min.m_y - ray.m_pos.m_y) / ray.m_vel.m_y;
	float yT2 = (max.m_y - ray.m_pos.m_y) / ray.m_vel.m_y;

	float zT1 = (min.m_z - ray.m_pos.m_z) / ray.m_vel.m_z;
	float zT2 = (max.m_z - ray.m_pos.m_z) / ray.m_vel.m_z;

	float xMinT = (xT1 < xT2) ? xT1 : xT2;
	float xMaxT = (xT1 > xT2) ? xT1 : xT2;

	float yMinT = (yT1 < yT2) ? yT1 : yT2;
	float yMaxT = (yT1 > yT2) ? yT1 : yT2;

	float zMinT = (zT1 < zT2) ? zT1 : zT2;
	float zMaxT = (zT1 > zT2) ? zT1 : zT2;

	float tEnter = xMinT;
	AXIS axis = AXIS::X;

	if (yMinT > tEnter) {
		tEnter = yMinT;
		axis = AXIS::Y;
	}

	if (zMinT > tEnter) {
		tEnter = zMinT;
		axis = AXIS::Z;
	}

	float tExit = (xMaxT < yMaxT) ? xMaxT : yMaxT;
	tExit = (tExit < zMaxT) ? tExit : zMaxT;

	if (tExit < EPSILON || tEnter > tExit || bestCollision.m_t < tEnter) return;

	// This is a good collision!
	bestCollision.m_t = tEnter;
	bestCollision.m_final = false;
	bestCollision.m_colour = cuboid.m_colour;

	if (bestCollision.m_final) return;

	Vector n;
	switch (axis) {
		case AXIS::X:
			n = (ray.m_vel.m_x > 0) ? Vector(-1.0f, 0.0f, 0.0f) : Vector(1.0f, 0.0f, 0.0f);
			break;

		case AXIS::Y:
			n = (ray.m_vel.m_y > 0) ? Vector(0.0f, -1.0f, 0.0f) : Vector(0.0f, 1.0f, 0.0f);
			break;
		
		case AXIS::Z:
			n = (ray.m_vel.m_z > 0) ? Vector(0.0f, 0.0f, -1.0f) : Vector(0.0f, 0.0f, 1.0f);
			break;
	}

	Vector newRayVelocity = Reflect(ray.m_vel, n);
	Vector newRayPos = ray.m_pos + tEnter * ray.m_vel + EPSILON * n;
	bestCollision.m_nextRay = Ray{ newRayPos, newRayVelocity };
}

void CpuExecutor::TraceRay(uint32_t* buffer, size_t i) {
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};

	float dx = ( ( (i % WINDOW_W) / static_cast<float>(WINDOW_W - 1) ) * 2 ) - 1;
	float dy = ( ( (i / WINDOW_W) / static_cast<float>(WINDOW_H - 1) ) * -2 ) + 1;
	float dz = 0.5f; // normal lens
	// float dz = sqrt( 1 - (xVelocity * xVelocity) - (yVelocity * yVelocity) ); // -- > fisheye lens

	Vector rayPosition{ x, y, z};
	Vector rayVelocity{ dx, dy, dz };
	Ray ray{ rayPosition, rayVelocity };

	Collision bestCollision { FLT_MAX, true, COLOUR_BLACK, Ray{} };

	int collisions{0};
	while (collisions < MAX_COLLISIONS) {
		bestCollision = Collision{ FLT_MAX, true, COLOUR_BLACK, Ray{} };

		// Cubes
		for (const Cuboid& cuboid : m_world.GetCuboids()) TryCollision(cuboid, ray, bestCollision);

		if (bestCollision.m_final) {
			buffer[i] = toUint32(bestCollision.m_colour);
			return;
		}

		ray = bestCollision.m_nextRay;

		++collisions;
	}

	buffer[i] = toUint32(bestCollision.m_colour);
}