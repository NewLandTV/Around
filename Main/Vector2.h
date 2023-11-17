#ifndef __VECTOR2_H__
#define __VECTOR2_H__

#include <cmath>

struct Vector2
{
	static const Vector2 zero;
	static const Vector2 one;

	float x;
	float y;

	Vector2() : x(0.0f), y(0.0f) { }
	Vector2(int x, int y) : x(x), y(y) { }
	Vector2(float x, float y) : x(x), y(y) { }
	Vector2(const Vector2& other) : x(other.x), y(other.y) { }

	void Set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void Set(Vector2& other)
	{
		x = other.x;
		y = other.y;
	}

	float Magnitude()
	{
		return sqrt(x * x + y * y);
	}

	const Vector2& Normalized()
	{
		float magnitude = Magnitude();

		x /= magnitude;
		y /= magnitude;

		return *this;
	}

	friend Vector2& operator-(Vector2 a, const Vector2 b)
	{
		Vector2 vector(a.x - b.x, a.y - b.y);

		return vector;
	}
};

#endif