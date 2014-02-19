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

const Image<uint32_t,1> DistanceField::getDfImage() const
{
    Image<uint32_t,1> newImg(m_originalImage);

    newImg.filterAnd(Pixel<uint32_t,1>({0xffffffff}));
    return newImg;
}

const Image<uint32_t,1> &DistanceField::getOriginalImage() const
{ return m_originalImage; }

}
