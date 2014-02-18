#include <iostream>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#define FAILED_TO_LOAD_IMAGE 1
#define FAILED_TO_WRITE_IMAGE 2

#define DISTANCE_FIELD_DISTANCE 20

void showHelp(const char *msg)
{
    if (msg)
        std::cout << msg << "\n\n";

    std::cout << ("Usage:\n"
               "\t-h Help\n"
               "\t-i The input file\n"
               "\t-o The output file\n");
}

bool check(const void *arr, const std::string &errMsg, const std::string &successMsg)
{
    if (!arr)
        std::cerr << errMsg << "\n";
    else
        std::cout << successMsg << "\n";

    return arr;
}

bool checkPixel(unsigned char pixel[4])
{
    return pixel[0] || pixel[1] || pixel[2];
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

bool checkIsIn(const std::vector<unsigned char> &pixels, const int &width, const int &heigt, const int &channels, const int &i)
{}

int getDistanceToOpposing(const std::vector<unsigned char> &pixels, const int &width, const int &heigt, const int &channels, const int &i)
{
}

std::vector<float> createSignedDistanceField(const std::vector<unsigned char> &pixels, const int &width, const int &heigt, const int &channels, const int &dfW, const int &dfH)
{
    std::vector<std::pair<bool,int> > signedDistanceField(dfW * dfH, 0);

    int tCnt{dfW*dfH};
    for (size_t i{0}; i < tCnt; ++i)
    {
        signedDistanceField[i] = {checkIsIn(pixels, width, heigt, channels, i), getDistanceToOpposing(pixels, width, heigt, channels, i)};
    }
}

int main(int argc, char **argv)
{
    if (argc != 7)
    {
        if (argc < 2)
            showHelp(0);
        else
            showHelp("Wrong number of arguments!");
        return 0;
    }

    std::string in_file, out_file, minification_str;

    for (int i = 0; i < argc - 1; ++i)
    {
        if (!strcmp(argv[i], "-i"))
            in_file = std::string(argv[i+1]);
        else if (!strcmp(argv[i], "-o"))
            out_file = std::string(argv[i+1]);
        else if (!strcmp(argv[i], "-m"))
            minification_str = std::string(argv[i+1]);
    }


    if (in_file.empty() || out_file.empty() || minification_str.empty())
    {
        showHelp("You have to specify an input file, an output file and a minification factor!");
        return 0;
    }

    int minification = std::stoi(minification_str);
    if (minification <= 0)
    {
        showHelp("The minification factor has to be a value > 0");
        return 0;
    }

    int width, heigt, channels;
    unsigned char *img_data = stbi_load(in_file.c_str(), &width, &heigt, &channels, STBI_rgb_alpha);
    if (!check(img_data, "Failed to load image", std::string{"Loaded image: "} + in_file))
        return FAILED_TO_LOAD_IMAGE;

    std::vector<unsigned char> pixels(width * heigt * channels);
    int res;
    std::copy(pixels.data(), img_data, &res);

    int dfW{width / minification}, dfH{heigt / minification};
    std::vector<float> signedDistanceField{createSignedDistanceField(pixels, width, heigt, channels, dfW, dfH)};

    std::cout << "Done with filter\n";

//    int s1 = stbi_write_png(std::string("e_" + out_file).c_str(), width, heigt, channels, fill, 0);
//    int s2 = stbi_write_png(out_file.c_str(), width, heigt, channels, distanceField, 0);

//    if (!(s1 && s2))
//    {
//        std::cerr << "Failed to write: " << out_file << "\nTerminating...\n";
//        return FAILED_TO_WRITE_IMAGE;
//    }

    std::cout << "Wrote: " << out_file << "\n";
    std::cout << "Done!\n";
    return 0;
}
