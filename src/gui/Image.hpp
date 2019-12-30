#pragma once

#include <GL/gl.h>
#include <limits>

class Resource;

namespace gui {
    struct ImGuiWrapper;

    /**
     * Class for loading images and storing their OpenGL textures
     */
    class Image {
    public:
        explicit Image( const Resource& resource );

        Image( ImGuiWrapper& imGuiWrapper, const Resource& resource );

        Image( const Image& other ) = delete;

        Image( Image&& other ) = delete;

        Image& operator=( const Image& other ) = delete;

        Image& operator=( Image&& other ) = delete;

        ~Image();

        /**
         * Makes a texture for the image
         * @param _ Takes an imGuiWrapper reference to prevent calling this method before initializing imgui and opgnl
         * @param remake Should the texture be destroyed and recreated if it already exists
         */
        void makeTexture( ImGuiWrapper&, bool remake = false );

        /**
         * Return the texture for the image
         * @return The texture
         * @throw Throws an exception if the texture was not made yet, the texture can be made by constructing the
         *          image using the constructor that makes the texture or explicitly by calling Image::makeTexture()
         * @ref Image::Image(ImGuiWrapper&, const std::filesystem::path&)
         * @ref Image::makeTexture()
         */
        [[nodiscard]] GLuint texture() const;

        /**
         * Deletes the texture and drops the image from memory <br/>
         * This method is called from the destructor, but can be called explicitly if exception handling is desired
         */
        void dispose();

        /** @return The width of the image in pixels */
        [[nodiscard]] int width() const noexcept { return width_; }

        /** @return The height of the image in pixels */
        [[nodiscard]] int height() const noexcept { return height_; }

        /** The number of color channels in the image */
        [[nodiscard]] int channels() const noexcept { return channels_; }

    private:
        int width_{};
        int height_{};
        int channels_{};
        unsigned char* data;
        GLuint texture_;

        static constexpr auto nullTexture = std::numeric_limits<decltype( texture_ )>::max();
    };
}
