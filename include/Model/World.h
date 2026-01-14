#pragma once

#include <vector>

#include "Core/Colour.h"
#include "Core/Cuboid.h"
#include "Core/Vector.h"

#define EPSILON 1e-3


class World {
public:
	World();

	inline const std::vector<Cuboid>& GetCuboids() const { return m_cuboids; }

private:
	std::vector<Cuboid> m_cuboids;
};