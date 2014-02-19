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
    T m_channels[N];
};

template <class T, int N>
class Image
{
public:
    Image(const std::string &fileName)
    {
        int w,h,c;
        unsigned char *img_data = stbi_load(fileName.c_str(), &w, &h, &c, STBI_rgb_alpha);
        if (img_data)
        {
            m_width = w;
            m_height = h;
            m_pixels = std::vector<Pixel<T,N>>(m_width * m_height);
            memcpy((void*)m_pixels.data(), img_data, w * h * c);
        }
        else
        {
            std::cerr << "Unable to open image: " << fileName << std::endl;
        }
    }

    ~Image()
    {}

    const std::vector<Pixel<T,N>> &getPixels() const
    { return m_pixels; }

    void setPixels(const std::vector<Pixel<T,N>> &pixels, const size_t &width, const size_t &height)
    {
        m_pixels = pixels;
        m_width = width;
        m_height = height;
    }

    int getWidth() const
    { return m_width; }

    int getHeight() const
    { return m_height; }

    bool saveImage(const std::string &fileName)
    {
        int channels{4};
        int status = stbi_write_png(fileName.c_str(), m_width, m_height, channels, (unsigned char *)m_pixels.data(), 0);
        return status;
    }

private:
    std::vector<Pixel<T,N>> m_pixels;
    int m_width, m_height;
    ColorMode m_colorMode;
};

}
