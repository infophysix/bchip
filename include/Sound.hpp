#pragma once

#include <cmath>
#include <cstdint>
#include <queue>

#include <SDL.h>

constexpr std::size_t Amplitude { 28000 };
constexpr std::size_t Frequency { 44100 };

class Sound
{
public:
	
	struct SoundObject
	{
		double frequency;
		int samples_left;
	};

	Sound();
	~Sound();

	void beep(double frequency, int duration);
    void generateSamples(std::int16_t *stream, int length);
    void wait();

private:

	double v;
	std::queue<SoundObject> beeps;
};
