#include "SoundSystem.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include "util/define_logger.hpp"
#include "Exception.hpp"
#include "AL/alut.h"
#include "Sound.hpp"

DEFINE_LOGGER( SoundSystem )

namespace {
    std::atomic_int soundSystems;
    std::mutex      mutex;
}

using namespace std::string_literals;

SoundSystem::SoundSystem() {
    int count = ++soundSystems;
    if ( count > 1 ) return;
    std::unique_lock lock{ mutex, std::try_to_lock };
    if ( !lock ) return;

    if ( !alutInit( nullptr, nullptr )) {
        ALenum error = alutGetError();
        THROW_SIMPLE_EXCEPTION( "SoundSystemError: "s + alutGetErrorString( error ));
    }
}

SoundSystem::~SoundSystem() {
    int count = --soundSystems;
    if ( count > 0 ) return;
    std::unique_lock lock{ mutex, std::try_to_lock };
    if ( !lock ) return;
    alutExit();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"
Sound SoundSystem::makeSound( const Resource& resource ) {
    return Sound( resource );
}
#pragma clang diagnostic pop
