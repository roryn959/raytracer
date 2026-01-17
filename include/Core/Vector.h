#pragma once

struct Vector {
	float m_x;
	float m_y;
	float m_z;
};

inline Vector operator*(float s, const Vector& v) {
	return Vector{ s * v.m_x, s * v.m_y, s * v.m_z };
}

inline Vector operator*(const Vector& v, float s) {
	return Vector{ s * v.m_x, s * v.m_y, s * v.m_z };
}

inline Vector operator+(const Vector& v1, const Vector& v2) {
	return Vector{ v1.m_x + v2.m_x, v1.m_y + v2.m_y, v1.m_z + v2.m_z };
}

inline Vector operator-(const Vector& v1, const Vector& v2) {
	return Vector{ v1.m_x - v2.m_x, v1.m_y - v2.m_y, v1.m_z - v2.m_z };
}

inline Vector Normalise(const Vector& v) {
	float mag = v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
	return Vector { v.m_x / mag, v.m_y / mag, v.m_z / mag };
}

inline float Dot(const Vector& v1, const Vector& v2) {
	return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
}

inline Vector Reflect(const Vector& v, const Vector& n) {
	return v - 2.0f * Dot(v, n) * n;
}