#ifndef Header_SuperpoweredExample
#define Header_SuperpoweredExample

#include <math.h>
#include <pthread.h>

#include "SuperpoweredExample.h"
#include <SuperpoweredAdvancedAudioPlayer.h>
#include <SuperpoweredFilter.h>
#include <SuperpoweredRoll.h>
#include <SuperpoweredFlanger.h>
#include <AndroidIO/SuperpoweredAndroidAudioIO.h>
#include <SuperpoweredTimeStretching.h>

#define HEADROOM_DECIBEL 3.0f
static const float headroom = powf(10.0f, -HEADROOM_DECIBEL * 0.025f);


enum TEMPO_MODE {
    SLOW_DOWN, PLAY_NORMAL, SPEED_UP
};

static const float TEMPO_ORIGINAL = 1.0f;
static const float TEMPO_HALF = TEMPO_ORIGINAL / 2;
static const float TEMPO_DOUBLE = TEMPO_ORIGINAL * 2;

class SuperpoweredExample {
public:

    SuperpoweredExample(unsigned int samplerate, unsigned int buffersize, const char *path,
                        int fileOffset, int fileLength);

    ~SuperpoweredExample();

    bool process(short int *output, unsigned int numberOfSamples);

    void onPlayPause(bool play);

    void seekTo(double positionPercent);

    void onTempoSelected(int tempoMode);

    void onPitchChanged(int pitchValue);

private:
    SuperpoweredAndroidAudioIO *audioSystem;
    SuperpoweredAdvancedAudioPlayer *player;

    float *stereoBuffer;
    float volume;
};

#endif
