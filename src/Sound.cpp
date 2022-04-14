#include "Sound.hpp"

void audio_callback(void *_beeper, std::uint8_t *_stream, int _length);

Sound::Sound()
{
    SDL_AudioSpec desiredSpec;

    desiredSpec.freq = Frequency;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;

    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    SDL_PauseAudio(0);
}

Sound::~Sound()
{
    SDL_CloseAudio();
}

void Sound::generateSamples(std::int16_t *stream, int length)
{
    int i = 0;
    while (i < length) {

        if (beeps.empty()) {
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        SoundObject& bo = beeps.front();

        int samplesToDo = std::min(i + bo.samples_left, length);
        bo.samples_left -= samplesToDo - i;

        while (i < samplesToDo) {
            stream[i] = Amplitude * std::sin(v * 2 * M_PI / Frequency);
            i++;
            v += bo.frequency;
        }

        if (bo.samples_left == 0) {
            beeps.pop();
        }
    }
}

void Sound::beep(double freq, int duration)
{
    SoundObject bo;
    bo.frequency = freq;
    bo.samples_left = duration * Frequency / 1000;

    SDL_LockAudio();
    beeps.push(bo);
    SDL_UnlockAudio();
}

void Sound::wait()
{
    int size;
    do {
        SDL_Delay(20);
        SDL_LockAudio();
        size = beeps.size();
        SDL_UnlockAudio();
    } while (size > 0);

}

void audio_callback(void *_beeper, std::uint8_t *_stream, int _length)
{
	std::int16_t *stream = reinterpret_cast<std::int16_t*>(_stream);
    int length = _length / 2;
    Sound* beeper = (Sound*) _beeper;

    beeper->generateSamples(stream, length);
}
