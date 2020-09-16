#include <iostream>
#include <fstream>
#include <gumbo.h>
#include "freetypeeasy.h"
#include "x11window.h"
#include "htmlrenderer.h"
#include "webservice.h"
#include "renderdom.h"
#include "cssparse.h"
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

void getStyleFromDOM(GumboNode *dom, std::string &style, bool _isInStyle = false)
{
    if (dom->type == GUMBO_NODE_ELEMENT)
    {
        for (int i = 0; i < dom->v.element.children.length; i++)
        {
            getStyleFromDOM((GumboNode*)dom->v.element.children.data[i], style, dom->v.element.tag == GUMBO_TAG_STYLE);
        }
    }
    else if (dom->type == GUMBO_NODE_TEXT)
    {
        if (_isInStyle)
        {
            style += dom->v.text.text;
        }
    }
}

int main()
{


    framebuffer = (unsigned char*)malloc(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);
    memset(framebuffer, 255, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);

    //fte::freetypeInst *inst =  fte::initFreetype("/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 16);// Initialize with 16 as default size, will be changed if needed anyways.
    //fte::freetypeInst *inst =  fte::initFreetype("/home/tim/Downloads/KosugiMaru-Regular.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 16);// Initialize with 16 as default size, will be changed if needed anyways.
    fte::freetypeInst *inst =  fte::initFreetype("/home/tim/Documents/GitProjects/WebBrowser/HTMLRenderer/Font/NotoSansJP-Regular.otf", "/home/tim/Documents/GitProjects/WebBrowser/HTMLRenderer/Font/NotoSansJP-Bold.otf", 16);// Initialize with 16 as default size, will be changed if needed anyways.

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

    //std::string htmlFile = WebService::htmlFileDownloader("https://lightboxengine.com/chartest.html");
    //std::string htmlFile = WebService::htmlFileDownloader("https://lightboxengine.com/imgtext.html");
    std::string htmlFile = WebService::htmlFileDownloader("https://www.reddit.com/");
    //std::string htmlFile = WebService::htmlFileDownloader("https://htmlyoutube.lightboxengine.com");
    //std::string htmlFile = WebService::htmlFileDownloader("https://github.com/mario-132/");

    findAndReplaceAll( htmlFile, "&nbsp;", " ");

    GumboOutput* output = gumbo_parse(htmlFile.c_str());

    RenderDOMStyle style;
    style.display = "inline";
    style.font_size = 16;
    style.line_height = 1.2;
    style.bold = false;
    style.isLink = false;
    style.cR = 0;
    style.cG = 0;
    style.cB = 0;

    std::string css = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/default.css");
    getStyleFromDOM(output->root, css);
    std::vector<css::CSSSelectorBlock> cssOut = css::parseFromString(css);

    RenderDOM dom;
    RenderDOMItem rootDomItem = dom.parseGumboTree(output->root, style, "https://reddit.com/", cssOut);

    for (int i = 0; i < cssOut.size() && false; i++)
    {
        std::cout << "<";
        for (int j = 0; j < cssOut[i].selectors.size(); j++)
        {
            std::cout << cssOut[i].selectors[j].additionals.back().name << ", ";
        }
        std::cout << ">\n";
        for (int j = 0; j < cssOut[i].items.size(); j++)
        {
            std::cout << "  " << cssOut[i].items[j].attribute.attributeAsString;
            if (cssOut[i].items[j].value.type == css::CSS_TYPE_PX)
                std::cout << " " << cssOut[i].items[j].value.numberValue << "px" << std::endl;
            else if (cssOut[i].items[j].value.type == css::CSS_TYPE_EM)
                std::cout << " " << cssOut[i].items[j].value.numberValue << "em" << std::endl;
            else if (cssOut[i].items[j].value.type == css::CSS_TYPE_PERCENT)
                std::cout << " " << cssOut[i].items[j].value.numberValue << "%" << std::endl;

            else if (cssOut[i].items[j].attribute.attributeAsString == "display")
            {
                if (cssOut[i].items[j].value.valueAsString == "block")
                {
                    std::cout << " block" << std::endl;
                }
                else if (cssOut[i].items[j].value.valueAsString == "inline")
                {
                    std::cout << " inline" << std::endl;
                }
                else if (cssOut[i].items[j].value.valueAsString == "inline-block")
                {
                    std::cout << " inline-block" << std::endl;
                }
                else if (cssOut[i].items[j].value.valueAsString == "none")
                {
                    std::cout << " none" << std::endl;
                }
                else
                {
                    std::cout << " UNKNOWN" << std::endl;
                }
            }
            else if (cssOut[i].items[j].value.type == css::CSS_TYPE_UNKNOWN)
                std::cout << " UNKNOWN" << std::endl;
            else if (cssOut[i].items[j].value.type == css::CSS_TYPE_NUMBER)
                std::cout << " " << cssOut[i].items[j].value.numberValue << std::endl;
        }
    }

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
        //documentBox.textStartX = 0;
        //documentBox.textStartY = 0;//window.height/3;

        htmlrenderer.RenderItems.clear();
        //htmlrenderer.tX = 0;
        //htmlrenderer.tY = 100;
        htmlrenderer.assembleRenderListV2(rootDomItem, inst, &documentBox, RenderDOMStyle());
        htmlrenderer.renderRenderList(inst, htmlrenderer.RenderItems);

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
