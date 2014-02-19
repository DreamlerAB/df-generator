#include <iostream>
#include <vector>
#include <cstring>

#include <DistanceField.h>

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

    dfgenerator::DistanceField df{in_file, {64,64}};
    df.getDfImage().saveImage(out_file);

    std::cout << "Done!" << std::endl;
    return 0;
}
