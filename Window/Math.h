#pragma once

struct Vec2
{
	float x, y;

	Vec2(float x, float y)
		: x(x), y(y)
	{}
};

#define view_x(x) (x) / 640.0f * 2.0f - 1.0f
#define view_y(y) 1.0f - (y) / 480.f * 2.0f
