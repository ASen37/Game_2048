#pragma once
#include<graphics.h>
#include<cmath>

class Vector2
{
public:
	float x = 0;
	float y = 0;

public:
	Vector2() = default;
	~Vector2() = default;

	Vector2(float x, float y) : x(x), y(y){ }
	//重载加法操作
	Vector2 operator+(const Vector2& vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}
	void operator+=(const Vector2& vec)
	{
		x += vec.x;
		y += vec.y;
	}
	//重载减法操作
	Vector2 operator-(const Vector2& vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}
	void operator-=(const Vector2& vec)
	{
		x -= vec.x;
		y -= vec.y;
	}
	//重载乘法操作
	float operator*(const Vector2& vec) const
	{
		return (x * vec.x + y * vec.y);
	}
	Vector2 operator*(float vec) const
	{
		return Vector2(x * vec, y * vec);
	}
	void operator*=(float vec)
	{
		x *= vec;
		y *= vec;
	}

	float length() { //获取向量的模
		return sqrt(x * x + y * y);
	}

	Vector2 normalize() { //标准化向量
		float len = length();
		if (len == 0) return Vector2(0, 0);
		return Vector2(x / len, y / len);
	}
};

