#include <iostream>
#include <fstream>
#include <gumbo.h>
#include "freetypeeasy.h"
#include "x11window.h"
#include "htmlrenderer.h"
#include "webservice.h"
#include "renderdom.h"
#include "cssparse.h"
#include "debugger.h"
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

void renderDocumentBox(RDocumentBox &documentBox, int yScroll)
{
    unsigned char r = rand()%255;
    unsigned char g = rand()%255;
    unsigned char b = rand()%255;
    for (int x = documentBox.X; x < documentBox.X + documentBox.W; x++)
    {
        for (int y = documentBox.Y + yScroll; y < documentBox.Y + documentBox.H + yScroll; y++)
        {
            if (x == documentBox.X || y == documentBox.Y + yScroll ||
                    x == (documentBox.X + documentBox.W) - 1 ||
                    y == (documentBox.Y + documentBox.H + yScroll) - 1)
            {
                if (x < 0 || y < 0 || x >= FRAMEBUFFER_WIDTH || y >= FRAMEBUFFER_HEIGHT)
                    continue;
                framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)] = r;
                framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+1] = g;
                framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+2] = b;
            }
        }
    }
    for (int i = 0; i < documentBox.childBoxes.size(); i++)
    {
        renderDocumentBox(documentBox.childBoxes[i], yScroll);
    }
}

class WebPage
{
public:
    std::string pageTitle;// Temporary way to get page title.
    RenderDOM dom;
    RenderDOMItem rootDomItem;
    HTMLRenderer htmlrenderer;
    GumboOutput* output;
    ~WebPage()
    {
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }

    void init(std::string html, std::string fullURL)
    {

        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/gumbo-parser/docs/html/index.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/simple_text.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/Wikipedia.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/Wikipedia - Wikipedia.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/HTML/Google.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/es64f4.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/Home - MangaDex.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/reddit_ the front page of the internet.html");
        //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/mario-132 · GitHub.html");

        //std::string htmlFile = WebService::htmlFileDownloader("https://www.w3schools.com/w3css/tryw3css_templates_band.htm");
        //std::string htmlFile = WebService::htmlFileDownloader("https://lightboxengine.com/codeview.html");
        //std::string htmlFile = WebService::htmlFileDownloader("https://lightboxengine.com/multiselector.html");
        std::string htmlFile = html;
        //std::string htmlFile = WebService::htmlFileDownloader("https://htmlyoutube.lightboxengine.com");
        //std::string htmlFile = WebService::htmlFileDownloader("https://myanimelist.net/animelist/timl132?status=7");

        findAndReplaceAll( htmlFile, "&nbsp;", " ");

        output = gumbo_parse(htmlFile.c_str());


        getTitleFromDOM(output->root, pageTitle, false);

        RenderDOMStyle style;
        style.display = "inline";
        style.font_size = Debugger::getAdjustmentGetValue("textsize_drag");
        style.line_height = 1.2;
        style.bold = false;
        style.isLink = false;
        style.color.r = 0;
        style.color.g = 0;
        style.color.b = 0;
        style.color.a = 255;
        style.background_color.r = 255;
        style.background_color.g = 255;
        style.background_color.b = 255;
        style.background_color.a = 0;

        std::string css = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/default.css");
        std::vector<std::string> stylesheetPaths;
        getStyleFromDOM(output->root, css, stylesheetPaths);
        for (unsigned int i = 0; i < stylesheetPaths.size(); i++)
        {
            std::string newsrc = resolvePath(stylesheetPaths[i], fullURL);
            css += WebService::htmlFileDownloader(newsrc);
            std::cout << "Downloaded stylesheet: " << newsrc << std::endl;
        }

        css = css::commentRemover(css);
        findAndReplaceAll(css, "!important", "");
        std::vector<css::CSSSelectorBlock> cssOut = css::parseFromString(css);

        dom.domCallStack.clear();
        rootDomItem = dom.parseGumboTree(output->root, style, fullURL, cssOut);

        if (Debugger::getCheckboxEnabled("debug_log_css"))
        {
            std::string cssString;
            for (unsigned int i = 0; i < cssOut.size(); i++)
            {
                cssString += "<";
                for (int j = 0; j < cssOut[i].selectors.size(); j++)
                {
                    cssString += cssOut[i].selectors[j].additionals.back().name;
                    if (cssOut[i].selectors[j].additionals.back().matchingClasses.size())
                    {
                        cssString += "(";
                        for (int k = 0; k < cssOut[i].selectors[j].additionals.back().matchingClasses.size(); k++)
                        {
                            cssString += cssOut[i].selectors[j].additionals.back().matchingClasses[k];
                        }
                        cssString += ")";
                    }
                    cssString += ", ";
                }
                cssString += ">\n";
                for (int j = 0; j < cssOut[i].items.size(); j++)
                {
                    cssString += "  " + cssOut[i].items[j].attribute.attributeAsString;
                    if (cssOut[i].items[j].value.type == css::CSS_TYPE_PX)
                        cssString += " " + std::to_string(cssOut[i].items[j].value.numberValue) + "px" + "\n";
                    else if (cssOut[i].items[j].value.type == css::CSS_TYPE_EM)
                        cssString += " " + std::to_string(cssOut[i].items[j].value.numberValue) + "em" + "\n";
                    else if (cssOut[i].items[j].value.type == css::CSS_TYPE_PERCENT)
                        cssString += " " + std::to_string(cssOut[i].items[j].value.numberValue) + "%" + "\n";

                    else if (cssOut[i].items[j].attribute.attributeAsString == "display")
                    {
                        if (cssOut[i].items[j].value.valueAsString == "block")
                        {
                            cssString += " block\n";
                        }
                        else if (cssOut[i].items[j].value.valueAsString == "inline")
                        {
                            cssString += " inline\n";
                        }
                        else if (cssOut[i].items[j].value.valueAsString == "inline-block")
                        {
                            cssString += " inline-block\n";
                        }
                        else if (cssOut[i].items[j].value.valueAsString == "none")
                        {
                            cssString += " none\n";
                        }
                        else
                        {
                            cssString += " UNKNOWN\n";
                        }
                    }
                    else if (cssOut[i].items[j].value.type == css::CSS_TYPE_UNKNOWN)
                        cssString += " UNKNOWN\n";
                    else if (cssOut[i].items[j].value.type == css::CSS_TYPE_NUMBER)
                        cssString += " " + std::to_string(cssOut[i].items[j].value.numberValue) + "\n";
                    else if (cssOut[i].items[j].value.type == css::CSS_TYPE_COLOR)
                        cssString += " " + std::to_string((int)cssOut[i].items[j].value.color.r) + "," + std::to_string((int)cssOut[i].items[j].value.color.g) + "," + std::to_string((int)cssOut[i].items[j].value.color.b) + "\n";
                }
            }
            Debugger::setTextBoxText("css_textview", cssString);
        }


        htmlrenderer.framebuffer = framebuffer;
        htmlrenderer.framebufferWidth = FRAMEBUFFER_WIDTH;
        htmlrenderer.framebufferHeight = FRAMEBUFFER_HEIGHT;
    }
    void loop(X11Window &window, fte::freetypeInst *inst)
    {
        RDocumentBox documentBox;
        documentBox.X = Debugger::getAdjustmentGetValue("docbox_offset");
        documentBox.Y = Debugger::getAdjustmentGetValue("docbox_offset");//window.height/3;
        documentBox.W = window.width-(Debugger::getAdjustmentGetValue("docbox_offset")*2);
        documentBox.maxW = documentBox.W;
        documentBox.wLocked = true;
        documentBox.hLocked = false;
        documentBox.H = 0;
        if (Debugger::getCheckboxEnabled("debug_docbox_1px"))
            documentBox.H = 1;
        //documentBox.textStartX = 0;
        //documentBox.textStartY = 0;//window.height/3;

        htmlrenderer.RenderItems.clear();
        //htmlrenderer.tX = 0;
        //htmlrenderer.tY = 100;
        if (window.mousePressed && window.mousex > 0 && window.mousey > 0 && window.mousex < window.width && window.mousey < window.height)
        {
            htmlrenderer.mouseX = window.mousex;
            htmlrenderer.mouseY = window.mousey;
        }
        htmlrenderer.pressed = window.mousePressed && window.mousex > 0 && window.mousey > 0 && window.mousex < window.width && window.mousey < window.height && !Debugger::getCheckboxEnabled("debug_link_disable");
        htmlrenderer.assembleRenderListV2(rootDomItem, inst, &documentBox, RenderDOMStyle());
        /*std::string a;
        for (int i = 0; i < rootDomItem.element.style.cssdbg.matchingSelectorStrings.size(); i++)
        {
            a += rootDomItem.element.style.cssdbg.matchingSelectorStrings[i];
        }
        Debugger::setTextBoxText("NodeInfoTextBox", a);*/
        /*if (window.mousePressed && window.mousex > 0 && window.mousey > 0 && window.mousex < window.width && window.mousey < window.height)
        {
            for (unsigned int i = 0; i < htmlrenderer.RenderItems.size(); i++)
            {
                if (    window.mousex > htmlrenderer.RenderItems[i].position.x &&
                        window.mousey < htmlrenderer.RenderItems[i].position.y &&
                        window.mousex < htmlrenderer.RenderItems[i].position.x + htmlrenderer.RenderItems[i].position.w &&
                        window.mousey > htmlrenderer.RenderItems[i].position.y - htmlrenderer.RenderItems[i].position.h
                        )
                {
                    //std::cout << activeStyle.background_color.r << std::endl;
                    std::string selectorsDebug;
                    for (unsigned int i = 0; i < htmlrenderer.RenderItems[i].cssdbg.matchingSelectorStrings.size(); i++)
                    {
                        selectorsDebug += htmlrenderer.RenderItems[i].cssdbg.matchingSelectorStrings[i] + "\n";
                    }
                    Debugger::setTextBoxText("NodeInfoBox2", selectorsDebug);
                }
            }
        }*/

        htmlrenderer.renderRenderList(inst, htmlrenderer.RenderItems);

        // Draws a square outline on the document box for debug
        srand(2);
        if (Debugger::getCheckboxEnabled("debug_docbox"))
            renderDocumentBox(documentBox, htmlrenderer.yScroll);
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
    //std::string currentWebPage = "https://i.reddit.com/";
    std::string currentWebPage = "https://lightboxengine.com/wbtests/";

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
        webpage->htmlrenderer.yScroll = scrpos;
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

        if (webpage->htmlrenderer.switchPage)
        {
            Debugger::setSpinnerEnabled("loadingSpinner", true);
            Debugger::loop();
            std::string href = webpage->htmlrenderer.nextpage;
            delete webpage;
            webpage = new WebPage();
            Debugger::loop();
            currentWebPage = resolvePath(href, currentWebPage);
            std::cout << "Fetching: " << currentWebPage << std::endl;
            std::string html = WebService::htmlFileDownloader(currentWebPage);
            webpage->init(html, currentWebPage);
            window.scrollPos = 0;
            Debugger::setEntryText("UrlBox", currentWebPage);
            Debugger::loop();
            Debugger::setSpinnerEnabled("loadingSpinner", false);
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
