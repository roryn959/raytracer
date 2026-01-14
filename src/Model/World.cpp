#include "Model/World.h"

World::World() :
	m_cuboids{}
{
	Cuboid leftWall{ Vector{ -1.0f, -1.0f, -1.0f}, Vector{ -1.0f + EPSILON, 1.0f, 1.0f}, COLOUR_RED };
	m_cuboids.push_back(leftWall);

	Cuboid frontWall{ Vector{ -1.0f, -1.0f, 1.0f }, Vector{ 1.0f, 1.0f, 1.0f + EPSILON }, COLOUR_TURQUOISE };
	m_cuboids.push_back(frontWall);

	Cuboid rightWall{ Vector{ 1.0f, -1.0f, -1.0f }, Vector{ 1.0f + EPSILON, 1.0f, 1.0f }, COLOUR_BLUE };
	m_cuboids.push_back(rightWall);

	Cuboid backWall{ Vector{ -1.0f, -1.0f, -1.0f }, Vector{ 1.0f, 1.0f, -1.0f + EPSILON }, COLOUR_PINK };
	m_cuboids.push_back(backWall);

	Cuboid bottomWall{ Vector{ -1.0f, -1.0f, -1.0f }, Vector{ 1.0f, -1.0f + EPSILON, 1.0f }, COLOUR_GREEN };
	m_cuboids.push_back(bottomWall);

	Cuboid topWall{ Vector{ -1.0f, 1.0f, -1.0f }, Vector{ 1.0f, 1.0f + EPSILON, 1.0f }, COLOUR_YELLOW };
	m_cuboids.push_back(topWall);

	Cuboid cuboid0{ Vector{ -1.0f, -0.5f, 0.75f }, Vector{ -0.75f, -0.25f, 1.0f }, COLOUR_BLACK };
	//m_cuboids.push_back(cuboid0);

	Cuboid cuboid1{ Vector{ -0.35f, 0.4f, 0.5f}, Vector{ 0.9f, 0.8f, 0.7f }, COLOUR_BLACK };
	//m_cuboids.push_back(cuboid1);
}