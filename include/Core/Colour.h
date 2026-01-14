#pragma once

#include <cstdint>

struct Colour {
	float m_opacity;
    float m_red;
    float m_green;
    float m_blue;
};

inline uint32_t toUint32(const Colour& colour) {
	return 	( static_cast<int>(colour.m_opacity) << 24 ) | ( static_cast<int>(colour.m_red) << 16 ) |
		( static_cast<int>(colour.m_green) << 8 ) | ( static_cast<int>(colour.m_blue) );
}

constexpr Colour COLOUR_BLACK 		{ 255,	0, 		0, 		0 };
constexpr Colour COLOUR_WHITE 		{ 255,	255,	255,	255 };
constexpr Colour COLOUR_RED 		{ 255,	255,	0, 		0 };
constexpr Colour COLOUR_GREEN 		{ 255,	0, 		255, 	0 };
constexpr Colour COLOUR_BLUE 		{ 255,	0, 		0, 		255 };
constexpr Colour COLOUR_YELLOW 		{ 255,	255, 	255, 	0 };
constexpr Colour COLOUR_PINK 		{ 255,	255, 	0, 		255 };
constexpr Colour COLOUR_TURQUOISE	{ 255,	0,		255,	255 };