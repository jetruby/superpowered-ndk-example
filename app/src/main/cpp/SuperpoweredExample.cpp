#include "SuperpoweredExample.h"
#include <SuperpoweredAdvancedAudioPlayer.h>
#include <SuperpoweredSimple.h>
#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include <stdlib.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>


static bool audioProcessing(void *clientdata, short int *audioIO, int numberOfSamples,
                            int __unused samplerate) {
    return ((SuperpoweredExample *) clientdata)->process(audioIO, (unsigned int) numberOfSamples);
}

SuperpoweredExample::SuperpoweredExample(unsigned int samplerate, unsigned int buffersize,
                                         const char *path, int fileOffset, int fileLength) : volume(1.0f * headroom) {
    stereoBuffer = (float *) memalign(16, (buffersize + 16) * sizeof(float) * 2);

    player = new SuperpoweredAdvancedAudioPlayer(&player, NULL, samplerate, 0);
    player->open(path, fileOffset, fileLength);

    audioSystem = new SuperpoweredAndroidAudioIO(samplerate, buffersize, false, true,
                                                 audioProcessing,
                                                 this, -1, SL_ANDROID_STREAM_MEDIA, buffersize * 2);
}

SuperpoweredExample::~SuperpoweredExample() {
    delete audioSystem;
    delete player;
    free(stereoBuffer);
}

void SuperpoweredExample::onPlayPause(bool play) {
    if (play) {
        player->play(false);
    } else {
        player->pause();
    }
}

void SuperpoweredExample::onTempoSelected(int tempoMode) {
    __android_log_print(ANDROID_LOG_VERBOSE, "SuperpoweredExample", "TIME_SELECTED: %i", tempoMode);

    switch (tempoMode) {
        case SLOW_DOWN:
            player->setTempo(TEMPO_HALF, true);
            break;
        case PLAY_NORMAL:
            player->setTempo(TEMPO_ORIGINAL, true);
            break;
        case SPEED_UP:
            player->setTempo(TEMPO_DOUBLE, true);
            break;
        default:
            player->setTempo(TEMPO_ORIGINAL, true);
            break;
    }
}

void SuperpoweredExample::onPitchChanged(int pitchValue) {
    __android_log_print(ANDROID_LOG_VERBOSE, "SuperpoweredExample", "PITCH_CHANGED: %i",
                        pitchValue);
    player->setPitchShift(pitchValue);
}


void SuperpoweredExample::seekTo(double positionPercent) {
    player->seek(positionPercent);
}

bool SuperpoweredExample::process(short int *output, unsigned int numberOfSamples) {
    bool shouldPlay = player->process(stereoBuffer, false, numberOfSamples, volume);
    if (shouldPlay) {
        SuperpoweredFloatToShortInt(stereoBuffer, output, numberOfSamples);
    }
    return shouldPlay;
}

static SuperpoweredExample *example = NULL;

extern "C" {
    JNIEXPORT void Java_com_example_superpoweredsample_MainActivity_SuperpoweredExample(JNIEnv *javaEnvironment,
                                                                         jobject __unused obj,
                                                                         jint samplerate,
                                                                         jint buffersize,
                                                                         jstring apkPath,
                                                                         jint fileOffset,
                                                                         jint fileLength) {

        const char *path = javaEnvironment->GetStringUTFChars(apkPath, JNI_FALSE);
        example = new SuperpoweredExample((unsigned int) samplerate, (unsigned int) buffersize, path,
                                          fileOffset, fileLength);
        javaEnvironment->ReleaseStringUTFChars(apkPath, path);
    }

    JNIEXPORT void Java_com_example_superpoweredsample_MainActivity_onPlayPause(JNIEnv *__unused javaEnvironment, jobject __unused obj, jboolean play) {
        example->onPlayPause(play);
    }

    JNIEXPORT void Java_com_example_superpoweredsample_MainActivity_onPitchChanged(JNIEnv *__unused javaEnvironment, jobject __unused obj, jint pitchValue) {
        example->onPitchChanged(pitchValue);
    }
    JNIEXPORT void Java_com_example_superpoweredsample_MainActivity_onTempoSelected(JNIEnv *__unused javaEnvironment, jobject __unused obj, jint timeStretchValue) {
        example->onTempoSelected(timeStretchValue);
    }

    JNIEXPORT void Java_com_example_superpoweredsample_MainActivity_seekTo(JNIEnv *__unused javaEnvironment, jobject __unused obj, jdouble position) {
        example->seekTo(position);
    }
}



