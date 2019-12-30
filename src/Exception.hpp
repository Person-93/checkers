#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <boost/exception/info.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/get_error_info.hpp>
#include <log4cplus/loggingmacros.h>


namespace exception {
    using Because                 = boost::error_info<struct cause, std::string>;
    using WrappedExceptionType    = boost::error_info<struct wrapped_exception_type, std::string>;
    using WrappedExceptionMessage = boost::error_info<struct wrapped_exception_message, std::string>;

    class Exception : public virtual std::exception, public virtual boost::exception {
    public:
        const char* what() const noexcept override {
            const std::string* const info = boost::get_error_info<Because>( *this );
            return info ? info->c_str() : std::exception::what();
        }
    };
}

#define THROW_EXCEPTION( exception ) do { \
    auto ex_ = exception;                 \
    LOG4CPLUS_ERROR( GetLogger(), "Exception thrown: " << ex_.what()); \
    BOOST_THROW_EXCEPTION(ex_);           \
} while(false)

#define THROW_SIMPLE_EXCEPTION( message ) THROW_EXCEPTION(exception::Exception{} << exception::Because((message)))
