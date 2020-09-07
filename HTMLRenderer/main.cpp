#include <iostream>
#include <fstream>
#include <gumbo.h>
#include "freetypeeasy.h"
#include "x11window.h"
#include "htmlrenderer.h"
#include "webservice.h"
#include "renderdom.h"
#include <chrono>

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

void printFps()
{
    static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    static float fps;

    std::chrono::high_resolution_clock::time_point nowTime = std::chrono::high_resolution_clock::now();
    fps+=1.0f;
    if(std::chrono::duration_cast<std::chrono::microseconds>( nowTime - lastTime ).count() > 1000*1000)
    {
        std::cout << "FPS: " << fps << std::endl;
        //Gfps = fps;
        fps = 0;
        lastTime = std::chrono::high_resolution_clock::now();
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
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/reddit_ the front page of the internet.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/mario-132 · GitHub.html");

    std::string htmlFile = WebService::htmlFileDownloader("http://lightboxengine.com/imgtext.html");
    //std::string htmlFile = WebService::htmlFileDownloader("https://github.com/mario-132/");

    findAndReplaceAll( htmlFile, "&nbsp;", " ");

    GumboOutput* output = gumbo_parse(htmlFile.c_str());

    RenderDOMStyle style;
    style.display = "inline";
    style.visible = false;
    style.font_size = 16;
    style.line_height = 1.2;
    style.bold = false;
    style.isLink = false;

    RenderDOM dom;
    RenderDOMItem rootDomItem = dom.parseGumboTree(output->root, style, "http://lightboxengine.com");

    X11Window window;
    window.createWindow("HTMLRenderer", 1920, 1080, 3840, 2160);

    HTMLRenderer htmlrenderer;
    htmlrenderer.framebuffer = framebuffer;
    htmlrenderer.framebufferWidth = FRAMEBUFFER_WIDTH;
    htmlrenderer.framebufferHeight = FRAMEBUFFER_HEIGHT;
    int a = 2;
    while(1)
    {
        int scrpos = window.scrollPos*-20;
        printFps();

        int memsetPos = scrpos;
        if (memsetPos < 0)
        {
            memsetPos = 0;
        }
        memset(framebuffer+(memsetPos*FRAMEBUFFER_WIDTH*3), 255, FRAMEBUFFER_WIDTH * window.height * 3);

        RDocumentBox documentBox;
        documentBox.x = 0;
        documentBox.y = 0;//window.height/3;
        documentBox.w = window.width;
        documentBox.h = window.height;
        documentBox.textStartX = 0;
        documentBox.textStartY = 0;//window.height/3;

        auto out = htmlrenderer.assembleRenderList(rootDomItem, inst, &documentBox, RenderDOMStyle());
        htmlrenderer.renderRenderList(inst, out);

        //std::cout << scrpos << std::endl;
        for (int y = 0; y < window.height; y++)
        {
            for (int x = 0; x < window.width; x++)
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
