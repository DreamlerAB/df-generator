#include <iostream>
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define FAILED_TO_LOAD_IMAGE 1
#define FAILED_TO_WRITE_IMAGE 2

void showHelp(const char *msg)
{
    if (msg)
        std::cout << msg << "\n\n";

    std::cout << ("Usage:\n"
               "\t-h Help\n"
               "\t-i The input file\n"
               "\t-o The output file\n");
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

    int status = stbi_write_png(out_file, width, heigt, channels, pixels, 0);

    if (!status)
    {
        std::cerr << "Failed to write: " << out_file << "\nTerminating...\n";
        return FAILED_TO_WRITE_IMAGE;
    }

    std::cout << "Wrote: " << out_file << "\n";
    std::cout << "Done!\n";
    return 0;
}
