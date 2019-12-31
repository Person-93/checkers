#pragma once

#include "AL/alut.h"

class Resource;

class Sound {
public:
    void play();

private:
    friend class SoundSystem;

    explicit Sound( const Resource& resource );

    ALuint buffer;
    ALuint source;
};



