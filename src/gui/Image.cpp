#include "GL/gl3w.h"
#include "Image.hpp" // IWYU pragma: associated
#include "util/StringBuilder.hpp"
#include "Exception.hpp"
#include "util/define_logger.hpp"
#include "Resource.h"

DEFINE_LOGGER( gui::Image );

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT( x )           \
do {                               \
    if( !(x) ) {                   \
        using namespace exception; \
        using util::StringBuilder; \
        THROW_EXCEPTION(           \
            Exception{} << Because( StringBuilder( 512 ) << "STBI assertion failed: " << #x )); \
    }                              \
} while ( false )

#include "../vendor/stb/stb_image.h"

using gui::Image;

Image::Image( const Resource& resource ) : data(
        stbi_load_from_memory((unsigned char*) resource.begin(), resource.size(), &width_, &height_, &channels_, 0 )),
                                           texture_( nullTexture ) {
    TRACE;

    if ( data == nullptr ) {
        using namespace exception;
        THROW_EXCEPTION( Exception{} << Because(
                util::StringBuilder( 512 ) << "Failed to load image: " << stbi_failure_reason()));
    }

    LOG4CPLUS_DEBUG( GetLogger(), "Loaded image at " << static_cast<void*>(data));
}

Image::Image( ImGuiWrapper& imGuiWrapper, const Resource& resource ) : Image( resource ) {
    makeTexture( imGuiWrapper, false );
}

Image::~Image() {
    try {
        dispose();
    }
    catch ( ... ) {}
}

void Image::dispose() {
    if ( data == nullptr ) return;
    stbi_image_free( data );
    data = nullptr;
    glDeleteTextures( 1, &texture_ );
}

void Image::makeTexture( ImGuiWrapper&, bool remake ) {
    TRACE;

    if ( texture_ != nullTexture ) {
        if ( !remake ) return;
        glDeleteTextures( 1, &texture_ );
    }

    LOG4CPLUS_DEBUG( GetLogger(), "Making texture from image at " << static_cast<void*>(data));

    GLuint glChannels;
    switch ( channels_ ) {
        case STBI_rgb: glChannels = GL_RGB;
            break;
        case STBI_rgb_alpha: glChannels = GL_RGBA;
            break;
        default: {
            using namespace exception;
            THROW_EXCEPTION( Exception{} << Because{ "Unable to load image: unrecognized number of channels" } );
        }
    }

    glGenTextures( 1, &texture_ );
    glBindTexture( GL_TEXTURE_2D, texture_ );
    glTexImage2D( GL_TEXTURE_2D, 0, glChannels, width_, height_, 0, glChannels, GL_UNSIGNED_BYTE, data );
    glGenerateMipmap( GL_TEXTURE_2D );
}

GLuint gui::Image::texture() const {
    if ( texture_ != nullTexture ) return texture_;

    using namespace exception;
    THROW_EXCEPTION( Exception{} << Because( "Texture not created" ));
}
