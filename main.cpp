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
    char amount = 0x20;

    size_t size = width * heigt * channels;
    std::cout << "size: "<<size<<"\n";
    unsigned char *newPixels = new unsigned char[size];
    memset(newPixels, 0, size);
    for (size_t i = 0; i < width * heigt * channels; i += 4)
    {
        newPixels[i] = 0x00;
        newPixels[i+1] = 0x00;
        newPixels[i+2] = 0x00;
        newPixels[i+3] = 0xff;

        if (checkPixel(pixels+i))
        {
            newPixels[i] = 0x80;
            newPixels[i+1] = 0x80;
            newPixels[i+2] = 0x80;
        }
        else
        {

            for (int j = 0; j < DISTANCE_FIELD_DISTANCE; ++j)
                if (checkNeighbors(pixels, width, heigt, i, j))
                {
                    float ratio{float(DISTANCE_FIELD_DISTANCE - j) / float(DISTANCE_FIELD_DISTANCE)};
                    newPixels[i] += ratio * amount;
                    newPixels[i+1] += ratio * amount;
                    newPixels[i+2] += ratio * amount;
                }
        }

//        newPixels[i] = pixels[i]; // R
//        newPixels[i+1] = pixels[i+1]; // G
//        newPixels[i+2] = pixels[i+2]; // B
//        newPixels[i+3] = pixels[i+3]; // A
    }

    std::cout << "Done with filter\n";

    int status = stbi_write_png(out_file, width, heigt, channels, newPixels, 0);
    delete newPixels;

    if (!status)
    {
        std::cerr << "Failed to write: " << out_file << "\nTerminating...\n";
        return FAILED_TO_WRITE_IMAGE;
    }

    std::cout << "Wrote: " << out_file << "\n";
    std::cout << "Done!\n";
    return 0;
}
