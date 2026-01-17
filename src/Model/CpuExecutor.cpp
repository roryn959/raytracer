#include "Model/CpuExecutor.h"


CpuExecutor::CpuExecutor(const World& world) :
	m_world{world},
	m_accumulator{},
	m_accumulationCount{1}
{
	m_accumulator = new Colour[NUM_PIXELS];
	RefreshAccumulator();
}

CpuExecutor::~CpuExecutor() {
	delete[] m_accumulator;
}

void CpuExecutor::TraceRays(uint32_t* pixelBuffer) {
	Colour* rayBuffer = new Colour[NUM_PIXELS];

	std::vector<std::thread> threads;
	for (int t = 0; t < NUM_THREADS; ++t) {
		int start = t * RAYS_PER_THREAD;
		int end  = (t == NUM_THREADS - 1) ? NUM_PIXELS : start + RAYS_PER_THREAD;

		threads.emplace_back([this, start, end, rayBuffer]() {
        for (int i = start; i < end; ++i)
            TraceRay(rayBuffer, i);
    	});
	}

	for (auto& th : threads) th.join();

	for (int i = 0; i < NUM_PIXELS; ++i) {
		m_accumulator[i] = m_accumulator[i] + rayBuffer[i];
	}

	for (int i = 0; i < NUM_PIXELS; ++i) {
		pixelBuffer[i] = ToUint32( GammaCorrect(m_accumulator[i] / m_accumulationCount) );
	}

	++m_accumulationCount;
	delete[] rayBuffer;
}

Ray CpuExecutor::GenerateInitialRay(size_t i) {
	float dx = ( ( (i % WINDOW_W) / static_cast<float>(WINDOW_W - 1) ) * 2 ) - 1;
	float dy = ( ( (i / WINDOW_W) / static_cast<float>(WINDOW_H - 1) ) * -2 ) + 1;
	float dz = 0.5f; // normal lens
	//float dz = sqrt( 1 - (dx * dx) - (dy * dy) ); // -- > fisheye lens

	return Ray{ Vector{ 0.0f, 0.0f, 0.0f }, Vector{ dx, dy, dz }, COLOUR_WHITE };
}

void CpuExecutor::RefreshAccumulator() {
	memset(m_accumulator, 0.0f, NUM_PIXELS * sizeof(Colour));
	m_accumulationCount = 0;
}

float CpuExecutor::Rand_11() {
	float r = Rand01();
	return (r * 2) - 1.0f;
}

float CpuExecutor::Rand01() {
	static std::mt19937 rng{ std::random_device{}() };
	static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	return dist(rng);
}

Vector CpuExecutor::Scatter(const Vector& normal) {
	Vector random{ Rand_11(), Rand_11(), Rand_11() };
	return Normalise(normal + random);
}

bool CpuExecutor::ShouldSpectralReflect(float reflectIndex) {
	return (Rand01() < reflectIndex);
}

void CpuExecutor::CalculateNextRay(Ray& ray, const Collision& collision) {
	// Is the material finalising?
	if (collision.m_material.m_final) {
		ray.m_colour = Filter(ray.m_colour, collision.m_material.m_colour);
		return;
	}

	// Calculate ray energy
	if (ShouldSpectralReflect(collision.m_material.m_reflectionIndex)) {
		// Spectral Reflection
		ray.m_vel = Reflect(ray.m_vel, collision.m_normal);
	} else {
		// Diffuse Reflection
		ray.m_vel = Scatter(collision.m_normal);
		Colour newRayColour = Dampen(Filter(ray.m_colour, collision.m_material.m_colour), DIFFUSE_DAMPEN_FACTOR);
		ray.m_colour = newRayColour;
	}

	ray.m_pos = collision.m_location;
}

bool CpuExecutor::TryCollision(const Plane& plane, const Ray& ray, Collision& bestCollision) {
	float t;
	Vector normal;

	switch (plane.m_axis) {
		case AXIS::X: {
			t = (plane.m_offset - ray.m_pos.m_x) / ray.m_vel.m_x;
			normal = (ray.m_vel.m_x > 0) ? Vector{ -1.0f, 0.0f, 0.0f } : Vector{ 1.0f, 0.0f, 0.0f };
			break;
		}
		case AXIS::Y: {
			t = (plane.m_offset - ray.m_pos.m_y) / ray.m_vel.m_y;
			normal = (ray.m_vel.m_y > 0) ? Vector{ 0.0f, -1.0f, 0.0f } : Vector{ 0.0f, 1.0f, 0.0f };
			break;
		}
		case AXIS::Z: {
			t = (plane.m_offset - ray.m_pos.m_z) / ray.m_vel.m_z;
			normal = (ray.m_vel.m_z > 0) ? Vector{ 0.0f, 0.0f, -1.0f } : Vector{ 0.0f, 0.0f, 1.0f };
			break;
		}
	}

	if (t < EPSILON || bestCollision.m_t < t) return false;

	bestCollision.m_t = t;
	bestCollision.m_normal = normal;
	bestCollision.m_location = ray.m_pos + t * ray.m_vel + EPSILON * normal;
	bestCollision.m_material = plane.m_material;

	return true;
}

bool CpuExecutor::TryCollision(const Cuboid& cuboid, const Ray& ray, Collision& bestCollision) {
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

	if (tExit < EPSILON || tEnter > tExit || bestCollision.m_t < tEnter) return false;

	Vector normal;
	switch (axis) {
		case AXIS::X: {
			normal = (ray.m_vel.m_x > 0) ? Vector(-1.0f, 0.0f, 0.0f) : Vector(1.0f, 0.0f, 0.0f);
			break;
		}
		case AXIS::Y: {
			normal = (ray.m_vel.m_y > 0) ? Vector(0.0f, -1.0f, 0.0f) : Vector(0.0f, 1.0f, 0.0f);
			break;
		}
		case AXIS::Z: {
			normal = (ray.m_vel.m_z > 0) ? Vector(0.0f, 0.0f, -1.0f) : Vector(0.0f, 0.0f, 1.0f);
			break;
		}
	}

	bestCollision.m_t = tEnter;
	bestCollision.m_normal = normal;
	bestCollision.m_location = ray.m_pos + tEnter * ray.m_vel + EPSILON * normal;
	bestCollision.m_material = cuboid.m_material;

	return true;
}

void CpuExecutor::TraceRay(Colour* buffer, size_t i) {
	Ray ray = GenerateInitialRay(i);

	Collision bestCollision;

	int collisions{0};
	while (collisions < MAX_COLLISIONS) {
		bestCollision = Collision{ FLT_MAX, Vector{}, Vector{}, Material{} };

		for (const Cuboid& cuboidLight : m_world.GetCuboidLights()) TryCollision(cuboidLight, ray, bestCollision);
		for (const Plane& plane : m_world.GetPlanes()) TryCollision(plane, ray, bestCollision);
		for (const Cuboid& cuboid : m_world.GetCuboids()) TryCollision(cuboid, ray, bestCollision);

		CalculateNextRay(ray, bestCollision);

		if (bestCollision.m_material.m_final) {
			buffer[i] = ray.m_colour;
			return;
		}

		float rayEnergy = Max(ray.m_colour);

		if (rayEnergy < 0.01) break;

		if (collisions > 3) {
			if (rayEnergy < Rand01()) break;
			ray.m_colour = ray.m_colour / rayEnergy;
		}

		++collisions;
	}

	buffer[i] = COLOUR_BLACK;
}

void CpuExecutor::TraceRayRange(Colour* buffer, size_t start, size_t end) {
	for (int i = start; i < end; ++i) {
		TraceRay(buffer, i);
	}
}