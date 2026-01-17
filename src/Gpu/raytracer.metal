#include <metal_stdlib>
using namespace metal;

// ---------- Constants ----------

#define WINDOW_W 820
#define WINDOW_H 820

constant int NUM_PIXELS = WINDOW_W * WINDOW_H;

#define DIFFUSE_DAMPEN_FACTOR 0.8f
#define EPSILON 1e-4
#define MAX_COLLISIONS 20
#define ACCUMULATOR_DECAY_FACTOR 0.2

constant float GAMMA = 1.0f / 2.2f;

// ---------- Axis ----------

enum class AXIS {
	X,
	Y,
	Z
};

// ---------- Colour ----------

struct Colour {
	float m_opacity;
    float m_red;
    float m_green;
    float m_blue;
};

inline Colour operator+(Colour c1, Colour c2) {
	return Colour{ 1.0f, c1.m_red + c2.m_red, c1.m_green + c2.m_green, c1.m_blue + c2.m_blue };
}

inline Colour operator*(Colour c, float d) {
	return Colour{ 1.0f, c.m_red * d, c.m_green * d, c.m_blue * d };
}

inline Colour operator/(Colour c, float d) {
	return Colour{ 1.0f, c.m_red / d, c.m_green / d, c.m_blue / d };
}

inline Colour Filter(Colour c1, Colour c2) {
	return Colour{ c1.m_opacity * c2.m_opacity, c1.m_red * c2.m_red, c1.m_green * c2.m_green, c1.m_blue * c2.m_blue };
}

inline Colour Dampen(Colour c, float factor) {
	return Colour{ c.m_opacity, c.m_red * factor, c.m_green * factor, c.m_blue * factor };
}

inline float Max(Colour c) {
	float max = (c.m_red > c.m_blue) ? c.m_red : c.m_blue;
	max = (max > c.m_green) ? max : c.m_green;
	return max;
}

inline Colour GammaCorrect(Colour c) {
	float r = pow(c.m_red, GAMMA);
	float g = pow(c.m_green, GAMMA);
	float b = pow(c.m_blue, GAMMA);

	if (r < 0.0f) r = 0.0f;
	if (r > 1.0f) r = 1.0f;
	if (g < 0.0f) g = 0.0f;
	if (g > 1.0f) g = 1.0f;
	if (b < 0.0f) b = 0.0f;
	if (b > 1.0f) b = 1.0f;

	return Colour{ c.m_opacity, r, g, b };
}

inline uint32_t ToUint32(Colour c) {
	return 	( (static_cast<int>(c.m_opacity * 255)) << 24 ) | ( (static_cast<int>(c.m_red * 255)) << 16 ) |
		( (static_cast<int>(c.m_green * 255)) << 8 ) | ( (static_cast<int>(c.m_blue * 255)) );
}

// ---------- Material ----------

struct Material {
	uint	m_final;
	Colour 	m_colour;
	float	m_reflectionIndex;
};

// ---------- Vector ----------

struct Vector {
	float m_x;
	float m_y;
	float m_z;
};

inline Vector operator*(float s, Vector v) {
	return Vector{ s * v.m_x, s * v.m_y, s * v.m_z };
}

inline Vector operator+(Vector v1, Vector v2) {
	return Vector{ v1.m_x + v2.m_x, v1.m_y + v2.m_y, v1.m_z + v2.m_z };
}

inline Vector operator-(Vector v1, Vector v2) {
	return Vector{ v1.m_x - v2.m_x, v1.m_y - v2.m_y, v1.m_z - v2.m_z };
}

inline Vector Normalise(Vector v) {
	float mag = v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
	return Vector { v.m_x / mag, v.m_y / mag, v.m_z / mag };
}

inline float Dot(Vector v1, Vector v2) {
	return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
}

inline Vector Reflect(Vector v, Vector n) {
	return v - 2.0f * Dot(v, n) * n;
}

// ---------- Collision ----------

struct Collision {
	float 		m_t;
	Vector		m_normal;
	Vector		m_location;
	Material	m_material;
};

constant Colour COLOUR_BLACK 		=	{ 	1.0f,	0.0f,	0.0f, 	0.0f 	};
constant Colour COLOUR_WHITE 		=	{ 	1.0f,	1.0f,	1.0f,	1.0f 	};
constant Colour COLOUR_RED 			=	{ 	1.0f,	1.0f,	0.0f, 	0.0f 	};
constant Colour COLOUR_GREEN 		=	{ 	1.0f,	0.0f, 	1.0f,	0.0f 	};
constant Colour COLOUR_BLUE 		=	{ 	1.0f,	0.0f, 	0.0f, 	1.0f 	};
constant Colour COLOUR_YELLOW 		=	{ 	1.0f,	1.0f,	1.0f,	0.0f 	};
constant Colour COLOUR_PINK 		=	{ 	1.0f,	1.0f,	0.0f, 	1.0f 	};
constant Colour COLOUR_TURQUOISE	=	{ 	1.0f,	0.0f,	1.0f,	1.0f 	};
constant Colour COLOUR_PURPLE		=	{	1.0f,	0.45f,	0.31f,	0.67f	};
constant Colour COLOUR_WARM_LIGHT	=	{	1.0f,	1.0f, 	0.82f,	0.65f	};

// ---------- Ray ----------

struct Ray {
	Vector m_pos;
	Vector m_vel;
	Colour m_colour;
};

// ---------- Plane ----------

struct Plane {
	AXIS 		m_axis;
	float 		m_offset;
	Material	m_material;
};

// ---------- Cuboid ----------

struct Cuboid {
	Vector 		m_min;
	Vector 		m_max;
	Material 	m_material;
};

inline Vector GetCentre(Cuboid c) {
	return Vector{ (c.m_max.m_x + c.m_min.m_x) / 2.0f, (c.m_max.m_y + c.m_min.m_y) / 2.0f, (c.m_max.m_z + c.m_min.m_z) / 2.0f };
}

// ---------- Viewpoint ----------

struct Viewpoint {
	Vector m_position;
	Vector m_direction;
};

// ---------- HELPER FUNCTIONS ----------

inline uint hash(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

inline float Rand01(uint seed) {
    return (float)(hash(seed)) * (1.0f / 4294967296.0f);
}

inline float Rand_11(uint seed) {
	float r = Rand01(seed);
	return (r * 2) - 1.0f;
}

Ray GenerateInitialRay(uint i, Viewpoint viewpoint) {
	float dx = ( ( (i % WINDOW_W) / static_cast<float>(WINDOW_W - 1) ) * 2 ) - 1;
	float dy = ( ( (i / WINDOW_W) / static_cast<float>(WINDOW_H - 1) ) * -2 ) + 1;
	float dz = 0.5f; // normal lens
	//float dz = sqrt( 1 - (dx * dx) - (dy * dy) ); // -- > fisheye lens

	Vector direction{ dx, dy, dz };

	return Ray{ viewpoint.m_position, direction, COLOUR_WHITE };
}

inline uint FloatToUint(float f) {
    return as_type<uint>(f * 65536.0f); // scale fractional part
}

inline uint3 HashVector(Vector v) {
    uint x = FloatToUint(v.m_x);
    uint y = FloatToUint(v.m_y);
    uint z = FloatToUint(v.m_z);

    uint hx = x * 73856093u ^ y * 19349663u ^ z * 83492791u;
    uint hy = x * 19349663u ^ y * 83492791u ^ z * 73856093u;
    uint hz = x * 83492791u ^ y * 73856093u ^ z * 19349663u;

    return uint3(hx, hy, hz);
}

Vector Scatter(Vector normal, uint seed) {
	uint3 hash = HashVector(normal);
	Vector random{ Rand_11(seed ^ hash.x), Rand_11(seed ^ hash.y), Rand_11(seed ^ hash.z) };
	return Normalise(normal + random);
}

bool ShouldSpectralReflect(float reflectIndex, uint seed, Ray ray) {
	uint3 hash = HashVector(ray.m_pos);
	return (Rand01(seed ^ hash.x ^ hash.y ^ hash.z) < reflectIndex);
}

void CalculateNextRay(thread Ray* ray, Collision collision, uint seed) {
	// Is the material finalising?
	if (collision.m_material.m_final) {
		ray->m_colour = Filter(ray->m_colour, collision.m_material.m_colour);
		return;
	}

	// Calculate ray energy
	if (ShouldSpectralReflect(collision.m_material.m_reflectionIndex, seed, *ray)) {
		// Spectral Reflection
		ray->m_vel = Reflect(ray->m_vel, collision.m_normal);
	} else {
		// Diffuse Reflection
		ray->m_vel = Scatter(collision.m_normal, seed);
		Colour newRayColour = Dampen(Filter(ray->m_colour, collision.m_material.m_colour), DIFFUSE_DAMPEN_FACTOR);
		ray->m_colour = newRayColour;
	}

	ray->m_pos = collision.m_location;
}

bool TryCollision(Plane plane, Ray ray, thread Collision* bestCollision) {
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

	if (t < EPSILON || bestCollision->m_t < t) return false;

	bestCollision->m_t = t;
	bestCollision->m_normal = normal;
	bestCollision->m_location = ray.m_pos + t * ray.m_vel + EPSILON * normal;
	bestCollision->m_material = plane.m_material;

	return true;
}

bool TryCollision(Cuboid cuboid, Ray ray, thread Collision* bestCollision) {
	Vector min = cuboid.m_min;
	Vector max = cuboid.m_max;

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

	if (tExit < EPSILON || tEnter > tExit || bestCollision->m_t < tEnter) return false;

	Vector normal;
	switch (axis) {
		case AXIS::X: {
			normal = (ray.m_vel.m_x > 0) ? Vector{-1.0f, 0.0f, 0.0f} : Vector{1.0f, 0.0f, 0.0f};
			break;
		}
		case AXIS::Y: {
			normal = (ray.m_vel.m_y > 0) ? Vector{0.0f, -1.0f, 0.0f} : Vector{0.0f, 1.0f, 0.0f};
			break;
		}
		case AXIS::Z: {
			normal = (ray.m_vel.m_z > 0) ? Vector{0.0f, 0.0f, -1.0f} : Vector{0.0f, 0.0f, 1.0f};
			break;
		}
	}

	bestCollision->m_t = tEnter;
	bestCollision->m_normal = normal;
	bestCollision->m_location = ray.m_pos + tEnter * ray.m_vel + EPSILON * normal;
	bestCollision->m_material = cuboid.m_material;

	return true;
}

uint GetSeed(uint i, uint j) {
	uint seed = i + j * 1664525u;
	seed ^= seed >> 16;
	seed *= 0x7feb352du;
	seed ^= seed >> 15;
	seed ^= i;

	return seed;
}

Colour SimulateRay(uint i, Viewpoint viewpoint, uint seed, device Plane* planes, uint numPlanes, device Cuboid* cuboids, uint numCuboids, device Cuboid* cuboidLights, uint numCuboidLights) {
	thread Ray ray = GenerateInitialRay(i, viewpoint);
	thread Collision bestCollision;

	uint collisions = 0;
	while (collisions < MAX_COLLISIONS) {
		bestCollision = Collision{ FLT_MAX, Vector{}, Vector{}, Material{} };

		for (uint j = 0; j < numCuboidLights; ++j) {
			Cuboid cuboidLight = cuboidLights[j];
			TryCollision(cuboidLight, ray, &bestCollision);
		}

		for (uint j = 0; j < numPlanes; ++j) {
			Plane plane = planes[j];
			TryCollision(plane, ray, &bestCollision);
		}

		for (uint j = 0; j < numCuboids; ++j) {
			Cuboid cuboid = cuboids[j];
			TryCollision(cuboid, ray, &bestCollision);
		}

		CalculateNextRay(&ray, bestCollision, seed);

		if (bestCollision.m_material.m_final) {
			return ray.m_colour;
		}

		float rayEnergy = Max(ray.m_colour);

		if (rayEnergy < 0.01) break;

		if (collisions > 3) {
			if (rayEnergy < Rand01(seed ^ collisions)) break;
			ray.m_colour = ray.m_colour / rayEnergy;
		}

		++collisions;
	}

	return COLOUR_BLACK;

}

// ---------- KERNEL ----------

kernel void TraceRay(
	constant 	uint& 		samples 			[[ buffer(0) ]],
	constant	uint&		randseed			[[ buffer(1) ]],
	constant	bool&		moving				[[ buffer(2) ]],
	constant	Viewpoint*	viewpoint			[[ buffer(3) ]],
    device 		uint* 		pixelBuffer 		[[ buffer(4) ]],
	device 		Colour* 	accumulationBuffer 	[[ buffer(5) ]],
	constant	uint&		accumulationCount	[[ buffer(6) ]],
	device 		Plane* 		planes 				[[ buffer(7) ]],
	constant 	uint& 		numPlanes 			[[ buffer(8) ]],
	device 		Cuboid* 	cuboids 			[[ buffer(9) ]],
	constant 	uint& 		numCuboids 			[[ buffer(10) ]],
	device 		Cuboid* 	cuboidLights 		[[ buffer(11) ]],
	constant 	uint& 		numCuboidLights 	[[ buffer(12) ]],
    uint pixel [[ thread_position_in_grid ]]
) {
	Colour accumulatedColour = accumulationBuffer[pixel];
	uint sampleNumber = accumulationCount;

	uint seed;
	Colour result;

	for (uint i = 0; i < samples; ++i) {
		seed = GetSeed(pixel ^ (randseed * 68392), sampleNumber);
		result = SimulateRay(pixel, *viewpoint, seed, planes, numPlanes, cuboids, numCuboids, cuboidLights, numCuboidLights);
		
		accumulatedColour = accumulatedColour + result;
		++sampleNumber;
	}

	if (moving)
		accumulatedColour = accumulatedColour * (1 - ACCUMULATOR_DECAY_FACTOR) + result * ACCUMULATOR_DECAY_FACTOR;

	accumulationBuffer[pixel] = accumulatedColour;

	if (!moving) {
		accumulatedColour = accumulatedColour / sampleNumber;
	}

	pixelBuffer[pixel] = ToUint32( GammaCorrect( accumulatedColour ) );
}
