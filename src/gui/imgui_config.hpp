#pragma once

#include <log4cplus/loggingmacros.h>
#include "util/StringBuilder.hpp"
#include "Exception.hpp"


namespace gui::detail {
    log4cplus::Logger& GetLogger();
}

#define IM_ASSERT( x )                \
do {                                  \
    if ( !(x) ) {                     \
        using namespace exception;    \
        using util::StringBuilder;    \
        using gui::detail::GetLogger; \
        THROW_EXCEPTION(              \
             Exception{} << Because( StringBuilder( 512 ) << "ImGui assertion failed: " << #x )); \
    }                                 \
} while ( false )
