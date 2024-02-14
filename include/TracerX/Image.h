#pragma once

#include <string>
#include <vector>


struct Image
{
public:
    int width;
    int height;
    std::vector<float> pixels;
    std::string name = "None";

    static Image empty;

    Image();

    static Image loadFromFile(const std::string& fileName);
    static void saveToDisk(const std::string& fileName, int width, int height, unsigned char* pixels);
private:
};
