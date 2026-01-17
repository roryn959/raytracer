#pragma once

constexpr float GAMMA = 1.0f / 2.2f;

struct Colour {
	float m_opacity;
    float m_red;
    float m_green;
    float m_blue;
};

inline Colour operator+(const Colour& c1, const Colour& c2) {
	return Colour{ 1.0f, c1.m_red + c2.m_red, c1.m_green + c2.m_green, c1.m_blue + c2.m_blue };
}

inline Colour operator*(float d, const Colour& c) {
	return Colour{ 1.0f, c.m_red * d, c.m_green * d, c.m_blue * d };
}

inline Colour operator*(const Colour& c, float d) {
	return Colour{ 1.0f, c.m_red * d, c.m_green * d, c.m_blue * d };
}

inline Colour operator/(const Colour& c, float d) {
	return Colour{ 1.0f, c.m_red / d, c.m_green / d, c.m_blue / d };
}

inline Colour GammaCorrect(const Colour& c) {
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

inline Colour Filter(const Colour& c1, const Colour& c2) {
	return Colour{ c1.m_opacity * c2.m_opacity, c1.m_red * c2.m_red, c1.m_green * c2.m_green, c1.m_blue * c2.m_blue };
}

inline Colour Dampen(const Colour& c, float factor) {
	return Colour{ c.m_opacity, c.m_red * factor, c.m_green * factor, c.m_blue * factor };
}

inline float Max(const Colour& c) {
	float max = (c.m_red > c.m_blue) ? c.m_red : c.m_blue;
	max = (max > c.m_green) ? max : c.m_green;
	return max;
}

inline uint32_t ToUint32(const Colour& c) {
	return 	( (static_cast<int>(c.m_opacity * 255)) << 24 ) | ( (static_cast<int>(c.m_red * 255)) << 16 ) |
		( (static_cast<int>(c.m_green * 255)) << 8 ) | ( (static_cast<int>(c.m_blue * 255)) );
}

constexpr Colour COLOUR_BLACK 		{ 	1.0f,	0.0f,	0.0f, 	0.0f 	};
constexpr Colour COLOUR_WHITE 		{ 	1.0f,	1.0f,	1.0f,	1.0f 	};
constexpr Colour COLOUR_RED 		{ 	1.0f,	1.0f,	0.0f, 	0.0f 	};
constexpr Colour COLOUR_GREEN 		{ 	1.0f,	0.0f, 	1.0f,	0.0f 	};
constexpr Colour COLOUR_BLUE 		{ 	1.0f,	0.0f, 	0.0f, 	1.0f 	};
constexpr Colour COLOUR_YELLOW 		{ 	1.0f,	1.0f,	1.0f,	0.0f 	};
constexpr Colour COLOUR_PINK 		{ 	1.0f,	1.0f,	0.0f, 	1.0f 	};
constexpr Colour COLOUR_TURQUOISE	{ 	1.0f,	0.0f,	1.0f,	1.0f 	};
constexpr Colour COLOUR_PURPLE		{	1.0f,	0.45f,	0.31f,	0.67f	};
constexpr Colour COLOUR_WARM_LIGHT	{	1.0f,	1.0f, 	0.82f,	0.65f	};
