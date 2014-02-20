#pragma once

#include <Image.h>

namespace dfgenerator
{

typedef Image<uint32_t,1> dfImage;

class DistanceField
{
public:
    DistanceField(const std::string &fileName, const size_t &maximumMeasurement);

    const dfImage getDfImage() const;
    const dfImage &getOriginalImage() const;

private:
    dfImage m_originalImage;
    Resolution<size_t> m_targetResolution;
};

DistanceField::DistanceField(const std::string &fileName, const size_t &maximumMeasurement) :
    m_originalImage(fileName), m_targetResolution{0,0}
{
    size_t w{size_t(maximumMeasurement * m_originalImage.getResolution().getAspectRatio())}, h{size_t(maximumMeasurement * (1.f / m_originalImage.getResolution().getAspectRatio()))};

    m_targetResolution = {w,h};
}

bool isIn(const Pixel<uint32_t,1> &pixel)
{ return pixel.m_channels[0] & 0xff000000; }

int getDistanceToNearestOpposite(const dfImage &image, const size_t &index, const int &spread)
{
    int distance{0};
    bool thisPixelOn{isIn(image.getPixels()[index])};
    while (distance++ < spread)
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
    return spread;
}

const dfImage DistanceField::getDfImage() const
{
    typedef std::pair<bool, int> ppair;
    typedef Pixel<int, 1> px;

    std::vector<px> newPixelArray{m_originalImage.getResolution().getProduct()};
    int maxPos{0}, maxNeg{0}, dist;
    for (size_t i{0}; i < m_originalImage.getResolution().getProduct(); ++i)
    {
        dist = getDistanceToNearestOpposite(m_originalImage, i, 10);

        if (dist > maxPos)
            maxPos = dist;
        else if (dist < maxNeg)
            maxNeg = dist;

        newPixelArray[i] = {{dist}};
    }

    dfImage newImg(m_originalImage.getResolution());
    float ratio;
    char alpha; uint32_t col;
    for (size_t i{0}; i < newImg.getResolution().getProduct(); ++i)
    {
        if (newPixelArray[i].m_channels[0] < 0)
        {
            ratio = (float(newPixelArray[i].m_channels[0]) / float(maxNeg));
            alpha = 0x80 - 0x80 * ratio;
        }
        else
        {
            ratio = (float(newPixelArray[i].m_channels[0]) / float(maxPos));
            alpha = 0x80 + 0x80 * ratio;
        }
        if (alpha)
            ;
        col = ((alpha << 24) & 0xff000000) | 0xff0000 | 0xff00 | 0xff;
        newImg.getNonConstPixels()[i] = {{col}};
    }

    return newImg.scaleLinear(m_targetResolution);
}

const dfImage &DistanceField::getOriginalImage() const
{ return m_originalImage; }

}
