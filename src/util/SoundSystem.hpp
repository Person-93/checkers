#pragma once

#include "Sound.hpp"

class Sound;

class Resource;

class SoundSystem {
public:
    SoundSystem();

    ~SoundSystem();

    Sound makeSound( const Resource& resource );
};
