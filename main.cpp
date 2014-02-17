#include <iostream>
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define FAILED_TO_LOAD_IMAGE 1
#define FAILED_TO_WRITE_IMAGE 2

#define DISTANCE_FIELD_DISTANCE 10

void showHelp(const char *msg)
{
    if (msg)
        std::cout << msg << "\n\n";

    std::cout << ("Usage:\n"
               "\t-h Help\n"
               "\t-i The input file\n"
               "\t-o The output file\n");
}

bool checkPixel(unsigned char pixel[4])
{
    return pixel[0] && pixel[1] && pixel[2];
}

/* Assuming 4 channels */
bool checkNeighbors(unsigned char *pixels, const int &width, const int &height, const size_t &index, const int &distance)
{
    int channels = 4;
    size_t arrSize = width * height * channels;

    // TL -> TR
    for (size_t i = index - (distance * width * channels) - distance * channels; i < index - (distance * width * channels) + distance * channels; i += 4)
        if (i > 0 && i < arrSize && checkPixel(pixels + i))
            return true;

    // BL -> BR
    for (size_t i = index + (distance * width * channels) - distance * channels; i < index + (distance * width * channels) + distance * channels; i += 4)
        if (i > 0 && i < arrSize && checkPixel(pixels + i))
            return true;

    // TL -> BL
    for (size_t i = index - (distance * width * channels) - distance * channels; i < index + (distance * width * channels) - distance * channels;
         i += width * channels)
        if (i > 0 && i < arrSize && checkPixel(pixels +i))
            return true;

    // TR -> BR
    for (size_t i = index - (distance * width * channels) + distance * channels; i < index + (distance * width * channels) + distance * channels;
         i += width * channels)
        if (i > 0 && i < arrSize && checkPixel(pixels + i))
            return true;

    return false;
}

bool getNeighborOutlineArray(unsigned char *input, const int &w, const int &h, unsigned char *output)
{
    assert(w >= 0);
    assert(h >= 0);
    static const uint channels{4};
    size_t arraySize{uint(w) * uint(h) * channels};

    for (size_t i{0}; i < arraySize; i += channels)
    {
        bool res = checkNeighbors(input, w, h, i, 1);
        bool me = checkPixel(input + i);
        output[i+0] = res == me ? 0x00 : 0xff;
        output[i+1] = res == me ? 0x00 : 0xff;
        output[i+2] = res == me ? 0x00 : 0xff;
        output[i+3] = 0xff;
    }
}

bool getDistanceField(unsigned char *input, const int &w, const int &h, unsigned char *output)
{
    assert(w >= 0);
    assert(h >= 0);
    static const uint channels{4};
    static char amount = 0x20;
    size_t arraySize{uint(w) * uint(h) * channels};

    for (size_t i = 0; i < arraySize; i += channels)
    {
        output[i] = 0x00;
        output[i+1] = 0x00;
        output[i+2] = 0x00;
        output[i+3] = 0xff;

        if (checkPixel(input+i))
        {
            output[i] = 0x80;
            output[i+1] = 0x80;
            output[i+2] = 0x80;
        }
        else
        {
            for (int j = 0; j < DISTANCE_FIELD_DISTANCE; ++j)
                if (checkNeighbors(input, w, h, i, j))
                {
                    float ratio{float(DISTANCE_FIELD_DISTANCE - j) / float(DISTANCE_FIELD_DISTANCE)};
                    output[i] += ratio * amount;
                    output[i+1] += ratio * amount;
                    output[i+2] += ratio * amount;
                }
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        showHelp(0);
        return 0;
    }

    if (argc != 5)
    {
        showHelp("Wrong number of arguments!");
        return 0;
    }

    const char *in_file{0}, *out_file{0};

    if (!strcmp(argv[1], "-i"))
        in_file = argv[2];
    else if (!strcmp(argv[1], "-o"))
        out_file = argv[2];

    if (!strcmp(argv[3], "-o"))
        out_file = argv[4];
    else if (!strcmp(argv[3], "-i"))
        in_file = argv[4];

    if (!(in_file && out_file))
    {
        showHelp("You have to specify an input file and an output file!");
        return 0;
    }

    int width, heigt, channels;
    unsigned char *pixels = stbi_load(in_file, &width, &heigt, &channels, STBI_rgb_alpha);
    if (!pixels)
    {
        std::cerr << "Failed to load: " << in_file << "\nTerminating...\n";
        return FAILED_TO_LOAD_IMAGE;
    }

    std::cout << "Loaded: " << in_file << "\n";

    size_t arrSize{uint(width) * uint(heigt) * channels};
    unsigned char *edgePixels = new unsigned char[arrSize];
    memset(edgePixels, 0, arrSize);
    getNeighborOutlineArray(pixels, width, heigt, edgePixels);

    unsigned char *distanceField = new unsigned char[arrSize];
    memset(distanceField, 0, arrSize);
    getDistanceField(edgePixels, width, heigt, distanceField);


    std::cout << "Done with filter\n";

    int status = stbi_write_png(out_file, width, heigt, channels, distanceField, 0);
    delete edgePixels;

    if (!status)
    {
        std::cerr << "Failed to write: " << out_file << "\nTerminating...\n";
        return FAILED_TO_WRITE_IMAGE;
    }

    std::cout << "Wrote: " << out_file << "\n";
    std::cout << "Done!\n";
    return 0;
}
