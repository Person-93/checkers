#pragma once

#include <log4cplus/logger.h>        // IWYU pragma: export
#include <log4cplus/loggingmacros.h> // IWYU pragma: export
#include <ostream>                   // IWYU pragma: export
#include <boost/core/demangle.hpp>

#define DEFINE_LOGGER( class_ )                 \
static log4cplus::Logger& GetLogger() {         \
    using log4cplus::Logger;                    \
    static Logger logger = Logger::getInstance( boost::core::demangle( typeid( class_ ).name())); \
    return logger;                              \
}

namespace aima::util::detail {
    class Tracer {
    public:
        Tracer( log4cplus::Logger& logger, const char* const function ) : logger( logger ), function( function ) {
            LOG4CPLUS_TRACE( logger, "Entering " << function );
        }

        ~Tracer() {
            LOG4CPLUS_TRACE( logger, "Exiting " << function );
        }

    private:
        log4cplus::Logger& logger;
        const char       * const function;
    };

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

    inline auto makeTracer( log4cplus::Logger& logger, const char* const function ) {
        using namespace log4cplus;
        if constexpr ( APPLICATION_LOG_LEVEL <= TRACE_LOG_LEVEL ) // NOLINT(misc-redundant-expression)
            return Tracer( logger, function );
        else
            return '\0';
    }

#pragma clang diagnostic pop
}

#define TRACE auto tracer__ = aima::util::detail::makeTracer(GetLogger(), __PRETTY_FUNCTION__)
