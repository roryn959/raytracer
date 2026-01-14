#pragma once

#include "Core/Colour.h"
#include "Core/Vector.h"


struct Cuboid {
	Vector m_min;
	Vector m_max;
	Colour m_colour;
};