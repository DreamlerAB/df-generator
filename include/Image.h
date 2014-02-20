#pragma once

#include <vector>
#include <string>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace dfgenerator
{

enum ColorMode {
    CM_RGBA,
    CM_RGB,
    CM_ARGB
};

template <class T, size_t N>
struct Pixel
{
    Pixel()
    {
        for (size_t i = 0; i < N; ++i)
            m_channels[i] = {};
    }

    Pixel(const T (&channels)[N])
    {
        setChannels(channels);
    }

    const Pixel &operator=(const Pixel<T, N> &pixel)
    {
        setChannels(pixel.m_channels);
        return *this;
    }

    void setChannels(const T (&channels)[N])
    {
        for (size_t i = 0; i < N; ++i)
            m_channels[i] = channels[i];
    }

    T m_channels[N];
};

template <class T>
class Resolution
{
public:
    Resolution(const T &width, const T &height) :
        m_width(width), m_height(height), m_product(width * height)
    {}

    const T &getProduct() const
    { return m_product; }

    const T &w() const
    { return m_width; }

    const T &h() const
    { return m_height; }

private:
    T m_width, m_height, m_product;
};

template <class T, int N>
class Image
{
public:

    Image(const std::string &fileName) : m_resolution(0,0)
    {
        int w,h,c;
        unsigned char *img_data = stbi_load(fileName.c_str(), &w, &h, &c, STBI_rgb_alpha);
        if (img_data)
        {
            m_resolution = {size_t(w),size_t(h)};
            m_pixels = std::vector<Pixel<T,N>>(m_resolution.getProduct());
            memcpy((void*)m_pixels.data(), img_data, w * h * c);
        }
        else
        {
            std::cerr << "Unable to open image: " << fileName << std::endl;
        }
    }

    Image(const Resolution<size_t> &resolution) : m_resolution(resolution)
    {
        m_pixels = std::vector<Pixel<T,N>>(m_resolution.getProduct(), Pixel<T,N>());
    }

    Image(const std::vector<Pixel<T,N>> &pixels, const Resolution<size_t> &resolution) :
        m_pixels(pixels), m_resolution(resolution)
    {}

    Image() : m_resolution{0,0}
    {}

    ~Image()
    {}

    const Image<T,N> &operator=(const Image<T,N> &img)
    {
        m_resolution = img.getResolution();
        m_pixels = img.m_pixels;
        return *this;
    }

    const std::vector<Pixel<T,N>> &getPixels() const
    { return m_pixels; }

    void setPixels(const std::vector<Pixel<T,N>> &pixels, const Resolution<size_t> &resolution)
    {
        m_pixels = pixels;
        m_resolution = resolution;
    }

    const Resolution<size_t> &getResolution() const
    { return m_resolution; }

    bool saveImage(const std::string &fileName) const
    {
        int channels{4};
        int status = stbi_write_png(fileName.c_str(), m_resolution.w(), m_resolution.h(), channels, (unsigned char *)m_pixels.data(), 0);
        return status;
    }

    void filterAnd(const Pixel<uint32_t,1> &filter)
    {
        for (Pixel<uint32_t,1> &px : m_pixels)
            px.m_channels[0] &= filter.m_channels[0];
    }

private:
    std::vector<Pixel<T,N>> m_pixels;
    Resolution<size_t> m_resolution;
    ColorMode m_colorMode;
};

}
