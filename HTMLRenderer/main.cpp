#include <iostream>
#include <fstream>
#include <gumbo.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include "freetypeeasy.h"
#include "x11window.h"

#define FRAMEBUFFER_WIDTH 1920
#define FRAMEBUFFER_HEIGHT 1080

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

enum RTType
{
    DOM_ELEMENT,
    DOM_TEXT
};

struct RTElement
{

};

struct RTText
{

};

struct RTItem
{
    RTType type;
    RTElement element;
    RTText text;
};

/// Container for the text in a RItem, includes info like lineID and textSize.
struct RText
{
    int lineID;
    int textSize;
    std::string text;

    bool bold;
    bool isLink;
};

/// Position and dimensions of this RItem.
struct RPosition
{
    int x;
    int y;
    int w;
    int h;
};

struct RImage
{

};

/// Contains thee type of item in a RItem. the names speak for themselves.
enum RType
{
    RITEM_UNKNOWN,
    RITEM_NONE,
    RITEM_TEXT,
    RITEM_IMAGE
};

/// The RItem, contains all the data needed to draw it. Type variable stores what type of renderable this is,
/// e.g. RITEM_TEXT for text and RITEM_IMAGE for an image.
struct RItem
{
    RText text;
    RImage img;
    RPosition position;

    RType type;
};

struct RStyle
{
    std::string display;
    bool visible;
    //bool blockOrInline;// 0 = inline 1 = bloc
    int font_size;
    float line_height;

    bool bold;
    bool isLink;
};

/// Stores the dimensions of the current working box, this box will be the size of the window at program start, but
/// might shrink when parsing a div with e.g. width: 50%; It also contains the current document cursor, so the
/// render generator know where to put the elements.
struct RDocumentBox
{
    int x;
    int y;
    int w;
    int h;

    int textStartX;
    int textStartY;
};

/// Applies a style to the element based on what element it is. This should be replaced by a proper css parser at some point.
void applyStyle(GumboNode* node, RStyle &style)
{
    // Hide certain elements so the scripts and such don't appear as text.
    if (node->v.element.tag == GUMBO_TAG_BODY)
    {
        style.visible = true;
    }
    if (node->v.element.tag == GUMBO_TAG_HEAD)
    {
        style.visible = false;
    }
    if (node->v.element.tag == GUMBO_TAG_SCRIPT)
    {
        style.visible = false;
    }
    if (node->v.element.tag == GUMBO_TAG_STYLE)
    {
        style.visible = false;
    }

    // Default font sizes
    if (node->v.element.tag == GUMBO_TAG_H1)
        style.font_size = 16*2;
    if (node->v.element.tag == GUMBO_TAG_H2)
        style.font_size = 16*1.5;
    if (node->v.element.tag == GUMBO_TAG_H3)
        style.font_size = 16*1.17;
    if (node->v.element.tag == GUMBO_TAG_H4)
        style.font_size = 16;
    if (node->v.element.tag == GUMBO_TAG_H5)
        style.font_size = 16*0.83;
    if (node->v.element.tag == GUMBO_TAG_H6)
        style.font_size = 16*0.67;
    if (node->v.element.tag == GUMBO_TAG_P)
        style.font_size = 16;

    if ((node->v.element.tag == GUMBO_TAG_H1 ||
         node->v.element.tag == GUMBO_TAG_H2 ||
         node->v.element.tag == GUMBO_TAG_H3 ||
         node->v.element.tag == GUMBO_TAG_H4 ||
         node->v.element.tag == GUMBO_TAG_H5 ||
         node->v.element.tag == GUMBO_TAG_H6 ||
         node->v.element.tag == GUMBO_TAG_B))
    {
        style.bold = true;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
        style.isLink = true;

    // Give it block so it causes a newline.
    if ((node->v.element.tag == GUMBO_TAG_H1 ||
         node->v.element.tag == GUMBO_TAG_H2 ||
         node->v.element.tag == GUMBO_TAG_H3 ||
         node->v.element.tag == GUMBO_TAG_H4 ||
         node->v.element.tag == GUMBO_TAG_H5 ||
         node->v.element.tag == GUMBO_TAG_H6 ||
         node->v.element.tag == GUMBO_TAG_P  ||
         node->v.element.tag == GUMBO_TAG_DIV||
         node->v.element.tag == GUMBO_TAG_LI))
    {
        style.display = "block";
    }
}

std::vector<RItem> assembleRenderList(GumboNode* node, fte::freetypeInst *inst, RStyle style, RDocumentBox *documentBox)// Make stylebox more like a textbox and make it a reference so they can all access it, then when a div occurs you make a new stylebox/textbox and give it that.
{
    std::vector<RItem> RenderItems;
    if (node->type == GUMBO_NODE_ELEMENT)
    {

        style.display = "inline";
        applyStyle(node, style);

        if (style.display == "block")
        {
            if (documentBox->textStartX != documentBox->x)
            {
                documentBox->textStartX = documentBox->x;
                documentBox->textStartY += style.font_size*style.line_height;
            }
        }

        if (node->v.element.tag == GUMBO_TAG_IMG)
        {
            int w = 100;
            int h = 100;
            for (int i = 0; i < node->v.element.attributes.length; i++)
            {
                if (std::string("width") == std::string(((GumboAttribute*)node->v.element.attributes.data[i])->name))
                {
                    w = std::stoi(((GumboAttribute*)node->v.element.attributes.data[i])->value);
                }
                if (std::string("height") == std::string(((GumboAttribute*)node->v.element.attributes.data[i])->name))
                {
                    h = std::stoi(((GumboAttribute*)node->v.element.attributes.data[i])->value);
                }
            }
            RItem item;
            item.type = RITEM_IMAGE;
            item.position.x = documentBox->textStartX;
            item.position.y = documentBox->textStartY;
            item.position.w = w;
            item.position.h = h;
            documentBox->textStartY += h;
            documentBox->textStartX += w;
            RenderItems.push_back(item);
        }

        // Parse the child elements.
        for (int i = 0; i < node->v.element.children.length; i++)
        {
            std::vector<RItem> returnRenderList = assembleRenderList((GumboNode*)node->v.element.children.data[i], inst, style, documentBox);
            for (int j = 0; j < returnRenderList.size(); j++)
            {
                RenderItems.push_back(returnRenderList[j]);
            }
        }

        if (style.display == "block")
        {
            if (documentBox->textStartX != documentBox->x)
            {
                documentBox->textStartX = documentBox->x;
                documentBox->textStartY += style.font_size*style.line_height;
            }
        }
    }
    else if (node->type == GUMBO_NODE_TEXT)
    {
        if (style.visible)// Only draw it if it's visible at all.
        {
            fte::setFontSize(inst, style.font_size);
            int tX = documentBox->textStartX;
            int tY = documentBox->textStartY;

            int temptX = tX;
            int temptY = tY;

            int charactersPreWritten = 0;
            for (int i = 0; i < strlen(node->v.text.text); i++)
            {
                fte::makeBold(inst, style.bold);
                fte::glyphInfo inf = fte::getCharacterBounds(inst, node->v.text.text[i]);
                temptX += inf.advanceX/64;
                if (temptX > documentBox->w || i == strlen(node->v.text.text)-1)
                {
                    RItem item;
                    item.type = RITEM_TEXT;
                    item.text.text = std::string(node->v.text.text+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = tX;
                    item.position.y = tY+(style.font_size*0.85)+((style.font_size*(style.line_height-1))/2);// font is scaled by 0.85 to better center the text
                    item.text.textSize = style.font_size;

                    item.text.bold = style.bold;
                    item.text.isLink = style.isLink;

                    RenderItems.push_back(item);

                    if (temptX > documentBox->w)
                    {
                        tX = documentBox->x;
                        tY += style.font_size*style.line_height;
                    }
                    else
                    {
                        tX = temptX;
                    }

                    temptX = tX;
                    temptY = tY;

                    charactersPreWritten = i+1;
                }
            }

            documentBox->textStartX = tX;
            documentBox->textStartY = tY;
        }
    }
    return RenderItems;
}

void renderRenderList(fte::freetypeInst *inst, std::vector<RItem> items)
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].type == RITEM_TEXT)
        {
            fte::makeBold(inst, items[i].text.bold);
            fte::setFontSize(inst, items[i].text.textSize);

            if (items[i].text.isLink)
                fte::setTextColor(inst, (255-26)/255.0f, (255-13)/255.0f, (255-171)/255.0f);
            else
                fte::setTextColor(inst, 1, 1, 1);

            int x = 0;
            if (items[i].position.y < FRAMEBUFFER_HEIGHT)// Make sure we are not trying to write a character off screen
            {
                for (int j = 0; j < items[i].text.text.size(); j++)
                {
                    auto inf = fte::drawCharacter(inst, items[i].text.text[j], items[i].position.x + x, items[i].position.y, framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, false);// Draw character, saving info about it
                    x += inf.advanceX/64;// Advance x position for next character
                }
            }
        }
        else if (items[i].type == RITEM_IMAGE)
        {
            for (int x = items[i].position.x; x < items[i].position.x+items[i].position.w; x++)
            {
                for (int y = items[i].position.y; y < items[i].position.y+items[i].position.h; y++)
                {
                    int xp = x;
                    int yp = y;
                    if (xp < 0 || yp < 0 || xp > FRAMEBUFFER_WIDTH-1 || yp > FRAMEBUFFER_HEIGHT-1)
                    {
                        continue;
                    }
                    framebuffer[(y*FRAMEBUFFER_WIDTH*3)+((x)*3)] = 128;
                    framebuffer[(y*FRAMEBUFFER_WIDTH*3)+((x)*3)+1] = 128;
                    framebuffer[(y*FRAMEBUFFER_WIDTH*3)+((x)*3)+2] = 128;
                }
            }
        }
    }
}


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
    std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/es64f4.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/WebBrowserData/HTML/Home - MangaDex.html");

    findAndReplaceAll( htmlFile, "&nbsp;", " ");

    GumboOutput* output = gumbo_parse(htmlFile.c_str());

    X11Window window;
    window.createWindow("HTMLRenderer", 1920, 1080, 1920, 1080);
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
        renderRenderList(inst, assembleRenderList(output->root, inst, docStyle, &documentBox));

        for (int x = 0; x < FRAMEBUFFER_WIDTH; x++)
        {
            for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++)
            {
                window.displayBuffer[(y*1920*4)+(x*4)+2] = framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+0];
                window.displayBuffer[(y*1920*4)+(x*4)+1] = framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+1];
                window.displayBuffer[(y*1920*4)+(x*4)+0] = framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+2];
            }
        }
        for (int i = 0; i < 10; i++)
        {
            window.processWindowEvents();
        }
    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);

    return 0;
}
