#include <iostream>
#include <fstream>
#include <gumbo.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include "freetypeeasy.h"
#include "x11window.h"
#include "htmlrenderer.h"

#define FRAMEBUFFER_WIDTH 3840
#define FRAMEBUFFER_HEIGHT 12690

std::string htmlFileLoader(std::string path)
{
    std::ifstream htmlFile(path, std::ios::binary | std::ios::ate);
    std::streamsize size = htmlFile.tellg();
    htmlFile.seekg(0, std::ios::beg);

    char *fileString = (char*)malloc(size+1);
    htmlFile.read(fileString, size);

    return std::string(fileString);
}

std::string htmlFileDownloader(std::string path)
{
    return std::string();
}

unsigned char *framebuffer;

void findAndReplaceAll(std::string& data, const std::string& match, const std::string& replace)
{
   // Get the first occurrence
   size_t pos = data.find(match);
   // Repeat till end is reached
   while( pos != std::string::npos)
    {
        data.replace(pos, match.size(), replace);
       // Get the next occurrence from the current position
        pos = data.find(match, pos+replace.size());
    }
}

int main()
{
    framebuffer = (unsigned char*)malloc(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);
    memset(framebuffer, 255, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);

    fte::freetypeInst *inst =  fte::initFreetype("/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 16);// Initialize with 16 as default size, will be changed if needed anyways.
    fte::makeBold(inst, false);
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/gumbo-parser/docs/html/index.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/simple_text.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/Wikipedia.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/Wikipedia - Wikipedia.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/HTML/Google.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/es64f4.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/Home - MangaDex.html");
    std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/reddit_ the front page of the internet.html");

    findAndReplaceAll( htmlFile, "&nbsp;", " ");

    GumboOutput* output = gumbo_parse(htmlFile.c_str());

    X11Window window;
    window.createWindow("HTMLRenderer", 1920, 1080, 3840, 2160);

    HTMLRenderer htmlrenderer;
    htmlrenderer.framebuffer = framebuffer;
    htmlrenderer.framebufferWidth = FRAMEBUFFER_WIDTH;
    htmlrenderer.framebufferHeight = FRAMEBUFFER_HEIGHT;

    //while(1){;}
    while (1)
    {
        // This is the base style that is applied on all html elements, this will be copied and modified as more elements
        // are parsed. This should ofc come from a default css stylesheet at some point as that's what they are made for and
        // it makes the c++ code much shorter.
        RStyle docStyle;
        docStyle.display = "inline";
        docStyle.visible = false;
        docStyle.font_size = 16;
        docStyle.line_height = 1.2;
        docStyle.bold = false;
        docStyle.isLink = false;

        RDocumentBox documentBox;
        documentBox.x = 0;
        documentBox.y = 0;//window.height/3;
        documentBox.w = window.width;
        documentBox.h = window.height;
        documentBox.textStartX = 0;
        documentBox.textStartY = 0;//window.height/3;
        memset(framebuffer, 255, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);
        auto renderlist = htmlrenderer.assembleRenderList(output->root, inst, docStyle, &documentBox);
        htmlrenderer.renderRenderList(inst, renderlist);
        int scrpos = window.scrollPos*10;
        std::cout << scrpos << std::endl;
        for (int x = 0; x < 3840; x++)
        {
            for (int y = 0; y < 2160; y++)
            {
                int ys = y+scrpos;
                window.displayBuffer[(y*3840*4)+(x*4)+2] = framebuffer[(ys*FRAMEBUFFER_WIDTH*3)+(x*3)+0];
                window.displayBuffer[(y*3840*4)+(x*4)+1] = framebuffer[(ys*FRAMEBUFFER_WIDTH*3)+(x*3)+1];
                window.displayBuffer[(y*3840*4)+(x*4)+0] = framebuffer[(ys*FRAMEBUFFER_WIDTH*3)+(x*3)+2];
            }
        }
        window.processWindowEvents();
    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);

    return 0;
}
