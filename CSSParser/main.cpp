#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "cssparse.h"

std::string fileLoad(std::string path)
{
    std::ifstream htmlFile(path, std::ios::binary | std::ios::ate);
    std::streamsize size = htmlFile.tellg();
    if (size == 0)
    {
        std::cerr << "Failed to open file" << std::endl;
    }
    htmlFile.seekg(0, std::ios::beg);

    char *fileString = (char*)malloc(size);
    htmlFile.read(fileString, size);

    return std::string(fileString);
}

int main()
{
    // Load stylesheet from file:
    std::string css = fileLoad("/home/tim/Documents/Development/WebBrowserData/HTML/stress.css");
    //std::string css = fileLoad("/home/tim/Documents/Development/WebBrowserData/HTML/Wikipedia - Wikipedia_files/load.css");
    //std::string css = fileLoad("/home/tim/Documents/Development/WebBrowserData/HTML/es64f4.html");// Garbage
    //std::string css = fileLoad("/home/tim/Documents/Development/WebBrowserData/HTML/Home - MangaDex_files/all.css");
    //std::string css = fileLoad("/home/tim/Documents/Development/WebBrowserData/HTML/Home - MangaDex_files/theme.css");

    // Parse it:
    css::parseFromString(css);

    return 0;
}
