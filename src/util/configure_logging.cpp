#include "configure_logging.hpp"
#include <iostream>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/appender.h>
#include <log4cplus/layout.h>
#include <log4cplus/spi/appenderattachable.h>
#include <log4cplus/config.hxx>
#include <string>


using namespace log4cplus;
using namespace log4cplus::spi;

class SpecialConsoleAppender : public Appender {
protected:
    void append( const spi::InternalLoggingEvent& event ) override {
        if ( const auto level = event.getLogLevel(); level <= INFO_LOG_LEVEL )
            normalLayout().formatAndAppend( std::cout, event );
        else if ( level <= ERROR_LOG_LEVEL )
            normalLayout().formatAndAppend( std::cerr, event );
        else
            fatalLayout().formatAndAppend( std::cerr, event );
    }

    void close() override {}

    ~SpecialConsoleAppender() override { destructorImpl(); }

private:
    static PatternLayout& normalLayout() {
        static PatternLayout layout( "%d{%Y-%m-%d %H:%M:%S,%Q} [%T] %-5p %-20c - %m%n" );
        return layout;
    }

    static PatternLayout& fatalLayout() {
        static PatternLayout layout(
                "%n"
                "================================================================================%n"
                "%d{%d %m %Y %H:%M:%S,%Q} [%T] %-5p %c %l%n%m%n"
                "================================================================================%n"
                                   );
        return layout;
    }
};

#ifndef APPLICATION_LOG_LEVEL
#error The macro APPLICATION_LOG_LEVEL must be defined
#endif

void util::ConfigureLogging() {
    initialize();
    auto root = Logger::getRoot();
    root.addAppender( SharedAppenderPtr( new SpecialConsoleAppender()));
    root.setLogLevel( APPLICATION_LOG_LEVEL );
    if constexpr ( APPLICATION_LOG_LEVEL <= INFO_LOG_LEVEL ) {
        LogLevelManager manager;
        const std::string& level = manager.toString( root.getLogLevel());
        LOG4CPLUS_INFO( root, "Logging at level " << level );
    }
}
