#pragma once

#include <Image.h>

namespace dfgenerator
{

class DistanceField
{
public:
    DistanceField(const std::string &fileName, const Resolution<size_t> &targetResolution);

    const Image<uint32_t,1> getDfImage() const;
    const Image<uint32_t,1> &getOriginalImage() const;

private:
    Image<uint32_t,1> m_originalImage;
    Resolution<size_t> m_targetResolution;
};

DistanceField::DistanceField(const std::string &fileName, const Resolution<size_t> &targetResolution) :
    m_originalImage(fileName), m_targetResolution(targetResolution)
{}

bool isIn(const Pixel<uint32_t,1> &pixel)
{ return pixel.m_channels[0] & 0xff000000; }

int getDistanceToNearestOpposite(const Image<uint32_t,1> &image, const size_t &index)
{
    int distance{0};
    bool thisPixelOn{isIn(image.getPixels()[index])};
    while (distance++ < 10)
    {
        // TL -> TR
        for (size_t i = index - (distance * image.getResolution().w()) - distance; i < index - (distance * image.getResolution().w()) + distance; ++i)
            if (i > 0 && i < image.getResolution().getProduct() && (thisPixelOn != isIn(image.getPixels()[i])))
                return thisPixelOn ? distance : -distance;

        // BL -> BR
        for (size_t i = index + (distance * image.getResolution().w()) - distance; i < index + (distance * image.getResolution().w()) + distance; i += 4)
            if (i > 0 && i < image.getResolution().getProduct() && (thisPixelOn != isIn(image.getPixels()[i])))
                return thisPixelOn ? distance : -distance;

        // TL -> BL
        for (size_t i = index - (distance * image.getResolution().w()) - distance; i < index + (distance * image.getResolution().w()) - distance;
             i += image.getResolution().w())
            if (i > 0 && i < image.getResolution().getProduct() && (thisPixelOn != isIn(image.getPixels()[i])))
                return thisPixelOn ? distance : -distance;

        // TR -> BR
        for (size_t i = index - (distance * image.getResolution().w()) + distance; i < index + (distance * image.getResolution().w()) + distance;
             i += image.getResolution().w())
            if (i > 0 && i < image.getResolution().getProduct() && (thisPixelOn != isIn(image.getPixels()[i])))
                return thisPixelOn ? distance : -distance;

    }
    return 10;
}

const Image<uint32_t,1> DistanceField::getDfImage() const
{
    typedef std::pair<bool, int> ppair;
    typedef Pixel<ppair, 1> px;

    std::vector<px> newPixelArray{m_originalImage.getResolution().getProduct()};
    for (size_t i{0}; i < m_originalImage.getResolution().getProduct(); ++i)
        newPixelArray[i] = {{{isIn(m_originalImage.getPixels()[i]), getDistanceToNearestOpposite(m_originalImage, i)}}};

    Image<uint32_t, 1> newImg(m_originalImage.getResolution());
//    for (size_t i{0}; i < newImg.getResolution().getProduct(); ++i)
//        newImg.getPixels()[i] =

    Image<uint32_t,1> dfImg{m_originalImage};
    return dfImg;
}

const Image<uint32_t,1> &DistanceField::getOriginalImage() const
{ return m_originalImage; }

}
