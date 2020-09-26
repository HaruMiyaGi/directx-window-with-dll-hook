#pragma once
#include <chrono>

class Timer
{
public:
	Timer()
		: last(std::chrono::steady_clock::now())
	{}
	float Mark()
	{
		const auto old = last;
		last = std::chrono::steady_clock::now();
		const std::chrono::duration<float> frameTime = last - old;
		return frameTime.count();
	}
	float Peek() const
	{
		return std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
	}
private:
	std::chrono::steady_clock::time_point last;
};