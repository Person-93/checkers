#include "Sound.hpp"
#include <string>
#include "Resource.h"
#include "util/define_logger.hpp"
#include "Exception.hpp"

DEFINE_LOGGER( Sound )

Sound::Sound( const Resource& resource ) {
    using namespace std::string_literals;

    buffer = alutCreateBufferFromFileImage( resource.begin(), resource.size());
    if ( buffer == AL_NONE )
        THROW_SIMPLE_EXCEPTION( "SoundError: "s + alutGetErrorString( alutGetError()));

    alGenSources( 1, &source );
    alSourcei( source, AL_BUFFER, buffer );
}

void Sound::play() {
    alSourcePlay( source );
}
