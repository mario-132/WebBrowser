#include <iostream>
#include <fstream>
#include <gumbo.h>
#include "freetypeeasy.h"
#include "x11window.h"
#include "webservice.h"
#include "renderdom.h"
#include "debugger.h"
#include "css.h"
#include <chrono>

#define FRAMEBUFFER_WIDTH 3840
#define FRAMEBUFFER_HEIGHT 2160

std::string htmlFileLoader(std::string path)
{
    std::ifstream htmlFile(path, std::ios::binary | std::ios::ate);
    std::streamsize size = htmlFile.tellg();
    htmlFile.seekg(0, std::ios::beg);

    char *fileString = (char*)malloc(size+1);
    htmlFile.read(fileString, size);
    std::string out = std::string(fileString, size);
    free(fileString);
    return out;
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

void getStyleFromDOM(GumboNode *dom, std::string &style, std::vector<std::string> &stylesheetPaths, bool _isInStyle = false)
{
    if (dom->type == GUMBO_NODE_ELEMENT)
    {
        if (dom->v.element.tag == GUMBO_TAG_LINK)
        {
            std::string href;
            std::string rel;
            std::string media;
            for (unsigned int i = 0; i < dom->v.element.attributes.length; i++)
            {
                if (std::string(((GumboAttribute*)dom->v.element.attributes.data[i])->name) == "href")
                {
                    href = ((GumboAttribute*)dom->v.element.attributes.data[i])->value;
                }
                if (std::string(((GumboAttribute*)dom->v.element.attributes.data[i])->name) == "rel")
                {
                    rel = ((GumboAttribute*)dom->v.element.attributes.data[i])->value;
                }
                if (std::string(((GumboAttribute*)dom->v.element.attributes.data[i])->name) == "media")
                {
                    media = ((GumboAttribute*)dom->v.element.attributes.data[i])->value;
                }
            }
            if ((rel == "stylesheet" || rel == "Stylesheet" || rel == "STYLESHEET") && media != "print" && media != "projection")
            {
                stylesheetPaths.push_back(href);
            }
        }
        for (unsigned int i = 0; i < dom->v.element.children.length; i++)
        {
            getStyleFromDOM((GumboNode*)dom->v.element.children.data[i], style, stylesheetPaths, dom->v.element.tag == GUMBO_TAG_STYLE);
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

void getTitleFromDOM(GumboNode *dom, std::string &title, bool _isInTitle = false)
{
    if (dom->type == GUMBO_NODE_ELEMENT)
    {
        for (unsigned int i = 0; i < dom->v.element.children.length; i++)
        {
            getTitleFromDOM((GumboNode*)dom->v.element.children.data[i], title, dom->v.element.tag == GUMBO_TAG_TITLE);
        }
    }
    else if (dom->type == GUMBO_NODE_TEXT)
    {
        if (_isInTitle)
        {
            title += dom->v.text.text;
        }
    }
}

std::string findBasePath(std::string path)
{
    std::string newPath;
    int slashAmount = 0;
    for (unsigned int i = 0; i < path.size(); i++)
    {
        if (path[i] == '/')
        {
            slashAmount++;
            if (slashAmount > 2)
            {
                break;
            }
        }
        newPath.push_back(path[i]);
    }
    return newPath;
}

std::string resolvePath(std::string path, std::string fullURL)
{
    std::string newPath;
    if (path.size()<1)
        return "";
    if (path.find("http") != std::string::npos)
    {
        return path;
    }
    else if (path[0] == '/')
    {
        if (path[1] == '/')
        {
            newPath = "https:" + path;
        }
        else
        {
            newPath = findBasePath(fullURL) + path;
        }
    }
    else
    {
        for (int i = fullURL.size(); i >= 0; i--)
        {
            if (fullURL[i] == '/')
            {
                fullURL.erase(i, fullURL.size()-i);
                break;
            }
        }
        newPath = fullURL + "/" + path;
    }

    return newPath;
}

class WebPage
{
public:
    std::string pageTitle;// Temporary way to get page title.
    RenderDOM dom;
    CSS css;

    GumboOutput* output;
    ~WebPage()
    {
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }

    void init(std::string html, std::string fullURL)
    {
        std::cout << "Webpage init" << std::endl;
        std::string htmlFile = html;

        findAndReplaceAll( htmlFile, "&nbsp;", " ");

        output = gumbo_parse(htmlFile.c_str());

        getTitleFromDOM(output->root, pageTitle, false);

        std::string tcss = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/default2.css");
        std::vector<std::string> stylesheetPaths;
        getStyleFromDOM(output->root, tcss, stylesheetPaths);
        for (unsigned int i = 0; i < stylesheetPaths.size(); i++)
        {
            std::string newsrc = resolvePath(stylesheetPaths[i], fullURL);
            tcss += WebService::htmlFileDownloader(newsrc);
            std::cout << "Downloaded stylesheet: " << newsrc << std::endl;
        }

        css.init(output->root, tcss);
        //GumboNode *node2;
        //node2 = ((GumboNode**)((GumboNode**)output->root->v.element.children.data)[4]->v.element.children.data)[2];
        //css.printNode(((GumboNode**)node2->v.element.children.data)[1]);
        //css.printNode(((GumboNode**)((GumboNode**)output->root->v.element.children.data)[2]->v.element.children.data)[3]);

        RenderDOMItem item = dom.parseGumboTree(output->root, &css, RenderDOMStyle());

        std::cout << "Hi" << std::endl;

        //if (Debugger::getCheckboxEnabled("debug_log_css"))
        {
            Debugger::setTextBoxText("css_textview", tcss);
        }
    }
    void loop(X11Window &window, fte::freetypeInst *inst)
    {

    }
};

int main()
{
    Debugger::start();

    framebuffer = (unsigned char*)malloc(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);
    memset(framebuffer, 255, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);

    //fte::freetypeInst *inst =  fte::initFreetype("/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 16);// Initialize with 16 as default size, will be changed if needed anyways.
    //fte::freetypeInst *inst =  fte::initFreetype("/home/tim/Downloads/KosugiMaru-Regular.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 16);// Initialize with 16 as default size, will be changed if needed anyways.
    fte::freetypeInst *inst =  fte::initFreetype("/home/tim/Documents/GitProjects/WebBrowser/HTMLRenderer/Font/NotoSansJP-Regular.otf", "/home/tim/Documents/GitProjects/WebBrowser/HTMLRenderer/Font/NotoSansJP-Bold.otf", 16);// Initialize with 16 as default size, will be changed if needed anyways.
    //fte::freetypeInst *inst =  fte::initFreetype("/home/tim/Documents/Development/WebBrowserData/Gothic_A1/GothicA1-Regular.ttf", "/home/tim/Documents/Development/WebBrowserData/Gothic_A1/GothicA1-Bold.ttf", 16);// Initialize with 16 as default size, will be changed if needed anyways.

    fte::makeBold(inst, false);

    WebService::init();

    X11Window window;
    window.createWindow("HTMLRenderer", 1920, 1080, 3840, 2160);

    Debugger::setSpinnerEnabled("loadingSpinner", true);
    Debugger::loop();
    std::string currentWebPage = "https://www.reddit.com/";
    //std::string currentWebPage = "http://lightboxengine.com/basiccss.html";

    std::string html = WebService::htmlFileDownloader(currentWebPage);
    //WebPage webpage;

    WebPage *webpage = new WebPage();
    webpage->init(html, findBasePath(currentWebPage));
    Debugger::setSpinnerEnabled("loadingSpinner", false);
    Debugger::setEntryText("UrlBox", currentWebPage);
    Debugger::loop();
    while(Debugger::windowIsOpen())
    {
        int scrpos = window.scrollPos*30;
        printFps();
        Debugger::loop();
        if (scrpos > 0)
        {
            scrpos = 0;
            window.scrollPos = 0;
        }
        //int memsetPos = scrpos;
        memset(framebuffer+(0*FRAMEBUFFER_WIDTH*3), 255, FRAMEBUFFER_WIDTH * window.height * 3);

        window.setTitle("WebBrowser - " + webpage->pageTitle);
        Debugger::loop();
        //std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
        webpage->loop(window, inst);
        //std::chrono::high_resolution_clock::time_point nowTime = std::chrono::high_resolution_clock::now();
        //std::cout << std::fixed << std::chrono::duration_cast<std::chrono::microseconds>( nowTime - lastTime ).count()/1000.0f << "ms" << std::endl;
        Debugger::loop();
        for (int y = 0; y < window.height; y++)
        {
            for (int x = 0; x < window.width; x++)
            {
                int ys = y;
                window.displayBuffer[(y*3840*4)+(x*4)+3] = 0;
                window.displayBuffer[(y*3840*4)+(x*4)+2] = framebuffer[(ys*FRAMEBUFFER_WIDTH*3)+(x*3)+0];
                window.displayBuffer[(y*3840*4)+(x*4)+1] = framebuffer[(ys*FRAMEBUFFER_WIDTH*3)+(x*3)+1];
                window.displayBuffer[(y*3840*4)+(x*4)+0] = framebuffer[(ys*FRAMEBUFFER_WIDTH*3)+(x*3)+2];
            }
        }

        if (Debugger::getWasGoButtonPressed())
        {
            Debugger::setSpinnerEnabled("loadingSpinner", true);
            Debugger::loop();
            std::string href = Debugger::getEntryText("UrlBox");
            delete webpage;
            webpage = new WebPage();
            Debugger::loop();
            currentWebPage = resolvePath(href, currentWebPage);
            std::cout << "Fetching: " << currentWebPage << std::endl;
            std::string html = WebService::htmlFileDownloader(currentWebPage);
            webpage->init(html, currentWebPage);
            window.scrollPos = 0;
            Debugger::loop();
            Debugger::setSpinnerEnabled("loadingSpinner", false);
        }

        Debugger::loop();

        window.processWindowEvents();
    }
    WebService::destroy();
    delete framebuffer;

    return 0;
}
