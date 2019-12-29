#pragma once

#include <utility>
#include <string>
#include <string_view>
#include <ostream>
#include <variant>

namespace util {
    namespace detail {
        template< typename Char, typename Traits, typename Allocator >
        std::basic_string<Char, Traits, Allocator>
        string( typename std::basic_string<Char, Traits, Allocator>::size_type n ) {
            std::basic_string<Char, Traits, Allocator> s;
            s.reserve( n );
            return s;
        }

        template< typename Char, typename Traits, typename Allocator >
        class StringBuilderBuffer : public std::basic_streambuf<Char, Traits> {
            using String     = std::basic_string<Char, Traits, Allocator>;
            using StringView = std::basic_string_view<Char, Traits>;
        public:
            explicit StringBuilderBuffer( String& string ) : string( string ) {
                this->setp( string.data(), string.data() + string.capacity());
            }

            StringBuilderBuffer( const StringBuilderBuffer& ) = delete;

            StringBuilderBuffer( StringBuilderBuffer&& ) noexcept( false ) = default;

            StringBuilderBuffer& operator=( const StringBuilderBuffer& ) = delete;

            StringBuilderBuffer& operator=( StringBuilderBuffer&& ) noexcept( false ) = default;

        protected:
            std::streamsize xsputn( const Char* s, std::streamsize count ) override {
                if ( const auto  spaceRemaining = this->epptr() - this->pptr(); count > spaceRemaining )
                    count = spaceRemaining;
                const StringView stringView( s, static_cast<typename StringView::size_type>(count));
                string += stringView;
                this->pbump( static_cast<int>(count));
                return count;
            }

        private:
            String& string;
        };
    }

    /**
     * A utility for building strings on the fly that implicitly converts to a string when it's finished.
     *
     * Designed to allow minimal dynamic memory allocation if needed. If it's not, just use the default constructor.
     *
     * THere are two constructors, one takes a size_t and constructs a string with that amount of reserved memory, the
     * other takes a reference to a string and uses that string as its buffer. The second constructor is intended to be
     * used when a StringBuilder will be created repeatedly in a loop, by making a std::string outside the loop, there
     * will be less dynamic memory allocation.
     *
     * @tparam Char The character type for the string, defaults to char
     * @tparam Traits The character traits of the string, defaults to std::char_traits<Char>
     * @tparam Allocator The allocator to use when constructing the string
     */
    template< typename Char = char,
              typename Traits = std::char_traits<Char>,
              typename Allocator = std::allocator<Char> >
    class StringBuilder {
    public:
        /** The string type */
        using String  = std::basic_string<Char, Traits, Allocator>;
    private:
        using Buffer  = detail::StringBuilderBuffer<Char, Traits, Allocator>;
        using Ostream = std::basic_ostream<Char, Traits>;
    public:
        /**
         * Makes a StringBuilder object with the underlying string starting with space for <code>buffSize</code>
         * characters.
         *
         * @param buffSize
         */
        explicit StringBuilder( typename String::size_type buffSize = 16 ) :
                string( detail::string<Char, Traits, Allocator>( buffSize )),
                buffer( std::get<String>( string )),
                ostream( &buffer ) {}

        /**
         * Makes a StringBuilder object using the given string as its buffer.
         *
         * @param string
         */
        explicit StringBuilder( String& string ) : string( &string ),
                                                   buffer( *( std::get<String*>( this->string ))),
                                                   ostream( &buffer ) {
            string.clear();
        }

        StringBuilder( const StringBuilder& ) = delete;

        StringBuilder( StringBuilder&& ) noexcept( false ) = default;

        StringBuilder& operator=( const StringBuilder& ) = delete;

        StringBuilder& operator=( StringBuilder&& ) noexcept( false ) = default;

        operator String()&& { // NOLINT(google-explicit-constructor)
            return std::move(*this).toString();
        }

        explicit operator String() const& {
            return toString();
        }

        String toString() const& {
            return std::holds_alternative<String>( string ) ?
                   std::get<String>( string ) :
                   *( std::get<String*>( string ));
        }

        String toString()&& {
            return std::holds_alternative<String>( string ) ?
                   std::move( std::get<String>( string )) :
                   *( std::get<String*>( string ));
        }

        template< typename T >
        friend StringBuilder& operator<<( StringBuilder& stringBuilder, const T& t ) {
            stringBuilder.ostream << t;
            return stringBuilder;
        }

        template< typename T >
        friend StringBuilder&& operator<<( StringBuilder&& stringBuilder, const T& t ) {
            stringBuilder.ostream << t;
            return std::move( stringBuilder );
        }

        template< typename Char_, typename Traits_ >
        friend std::basic_ostream<Char_, Traits_>&
        operator<<( std::basic_ostream<Char_, Traits_>& out, const StringBuilder& stringBuilder ) {
            return out << stringBuilder.toString();
        }

        template< typename Char_, typename Traits_ >
        friend std::basic_ostream<Char_, Traits_>&
        operator<<( std::basic_ostream<Char_, Traits_>& out, StringBuilder&& stringBuilder ) {
            return out << std::move( stringBuilder ).toString();
        }

    private:
        std::variant<String*, String> string;
        Buffer                        buffer;
        Ostream                       ostream;
    };
}
