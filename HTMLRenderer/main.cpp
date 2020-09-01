#include <iostream>
#include <fstream>
#include <gumbo.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include "freetypeeasy.h"
#include "x11window.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define FRAMEBUFFER_WIDTH 1920
#define FRAMEBUFFER_HEIGHT 1080

#define FONTSIZE 16
#define FONTLINE 18

std::string htmlFileLoader(std::string path)
{
    std::ifstream htmlFile(path, std::ios::binary | std::ios::ate);
    std::streamsize size = htmlFile.tellg();
    htmlFile.seekg(0, std::ios::beg);

    char *fileString = (char*)malloc(size);
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


struct RText
{
    int lineID;
    int textSize;
    std::string text;

    bool bold;
    bool isLink;
};

struct RPosition
{
    int x;
    int y;
    int w;
    int h;
};

struct RItem
{
    RText text;
    RPosition position;
};

struct RStyle
{
    std::string display;
    bool visible;
    //bool blockOrInline;// 0 = inline 1 = block
    int font_size;
    float line_height;

    bool bold;
    bool isLink;
};

struct RTextInfoBox
{
    int x;
    int y;
    int w;
    int h;

    int textStartX;
    int textStartY;
};

void applyStyle(GumboNode* node, RStyle &style)
{
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

std::vector<RItem> assembleRenderList(GumboNode* node, fte::freetypeInst *inst, RStyle style, RTextInfoBox *activeTextBox)// Make stylebox more like a textbox and make it a reference so they can all access it, then when a div occurs you make a new stylebox/textbox and give it that.
{
    std::vector<RItem> RenderItems;
    if (node->type == GUMBO_NODE_ELEMENT)
    {
        for (int i = 0; i < node->v.element.attributes.length; i++)
        {
            std::cout << ((GumboAttribute*)node->v.element.attributes.data[i])->name << " = " << ((GumboAttribute*)node->v.element.attributes.data[i])->value << std::endl;
        }

        style.display = "inline";
        applyStyle(node, style);

        if (style.display == "block")
        {
            /*if (htmlTextCursorX != 0)
            {
                htmlTextCursorX = 0;
                htmlTextCursorY += FONTLINE;
            }*/
            if (activeTextBox->textStartX != activeTextBox->x)
            {
                activeTextBox->textStartX = activeTextBox->x;
                activeTextBox->textStartY += style.font_size*style.line_height;
            }
        }

        for (int i = 0; i < node->v.element.children.length; i++)
        {
            std::vector<RItem> returnRenderList = assembleRenderList((GumboNode*)node->v.element.children.data[i], inst, style, activeTextBox);
            for (int j = 0; j < returnRenderList.size(); j++)
            {
                RenderItems.push_back(returnRenderList[j]);
            }
        }

        if (style.display == "block")
        {
            /*if (htmlTextCursorX != 0)
            {
                htmlTextCursorX = 0;
                htmlTextCursorY += FONTLINE;
            }*/
            if (activeTextBox->textStartX != activeTextBox->x)
            {
                activeTextBox->textStartX = activeTextBox->x;
                activeTextBox->textStartY += style.font_size*style.line_height;
            }
        }
    }
    else if (node->type == GUMBO_NODE_TEXT)
    {

        if (style.visible)
        {
            fte::setFontSize(inst, style.font_size);
            int tX = activeTextBox->textStartX;
            int tY = activeTextBox->textStartY;

            int temptX = tX;
            int temptY = tY;

            int charactersPreWritten = 0;

            /*for (int i = 0; i < strlen(node->v.text.text); i++)
            {
                htmlDrawCharacter(inst, framebuffer, node->v.text.text[i]);
            }*/
            for (int i = 0; i < strlen(node->v.text.text); i++)
            {
                fte::makeBold(inst, style.bold);
                fte::glyphInfo inf = fte::getCharacterBounds(inst, node->v.text.text[i]);
                temptX += inf.advanceX/64;
                if (temptX > activeTextBox->w)
                {
                    /*for (int xx = 0; xx < FRAMEBUFFER_HEIGHT; xx++)
                    {
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)] = 0;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+1] = 255;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+2] = 0;
                    }*/

                    //std::cout << "a" << std::endl;
                    RItem item;
                    item.text.text = std::string(node->v.text.text+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = tX;
                    item.position.y = tY+(style.font_size*0.85)+((style.font_size*(style.line_height-1))/2);// font is scaled by 0.85 to better center the text
                    item.text.textSize = style.font_size;

                    item.text.bold = style.bold;
                    item.text.isLink = style.isLink;

                    RenderItems.push_back(item);

                    tX = activeTextBox->x;
                    tY += style.font_size*style.line_height;
                    temptX = tX;
                    temptY = tY;

                    charactersPreWritten = i+1;

                    /*for (int xx = 0; xx < FRAMEBUFFER_HEIGHT; xx++)
                    {
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)] = 0;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+1] = 255;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+2] = 0;
                    }*/
                }
                else if (i == strlen(node->v.text.text)-1)
                {
                    /*for (int xx = 0; xx < FRAMEBUFFER_HEIGHT; xx++)
                    {
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)] = 0;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+1] = 255;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+2] = 0;
                    }*/

                    //std::cout << "b" << std::endl;
                    RItem item;
                    item.text.text = std::string(node->v.text.text+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = tX;
                    item.position.y = tY+(style.font_size*0.85)+((style.font_size*(style.line_height-1))/2);// font is scaled by 0.85 to better center the text
                    item.text.textSize = style.font_size;

                    item.text.bold = style.bold;
                    item.text.isLink = style.isLink;

                    RenderItems.push_back(item);

                    tX = temptX;
                    //tY += FONTLINE;
                    temptX = tX;
                    temptY = tY;

                    charactersPreWritten = i+1;

                    /*for (int xx = 0; xx < FRAMEBUFFER_HEIGHT; xx++)
                    {
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)] = 0;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+1] = 255;
                        framebuffer[(tY*FRAMEBUFFER_WIDTH*3)+((xx)*3)+2] = 0;
                    }*/
                }
            }

            activeTextBox->textStartX = tX;
            activeTextBox->textStartY = tY;
        }
    }
    return RenderItems;
}

void renderRenderList(fte::freetypeInst *inst, std::vector<RItem> items)
{
    for (int i = 0; i < items.size(); i++)
    {
        fte::makeBold(inst, items[i].text.bold);
        fte::setFontSize(inst, items[i].text.textSize);
        if (items[i].text.isLink)
            fte::setTextColor(inst, (255-26)/255.0f, (255-13)/255.0f, (255-171)/255.0f);
        else
            fte::setTextColor(inst, 1, 1, 1);
        int x = 0;
        if (items[i].position.y < FRAMEBUFFER_HEIGHT)
        {
            for (int j = 0; j < items[i].text.text.size(); j++)
            {
                auto inf = fte::drawCharacter(inst, items[i].text.text[j], items[i].position.x + x, items[i].position.y, framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, false);
                x += inf.advanceX/64;
                /*if (items[i].text.textSize == 128)
                {
                    std::cout << items[i].text.textSize << ": " << inf.height << std::endl;
                }*/
            }
        }
        /*for (int xx = 0; xx < FRAMEBUFFER_HEIGHT; xx++)
        {
            framebuffer[(items[i].position.y*FRAMEBUFFER_WIDTH*3)+((xx)*3)] = 255;
            framebuffer[(items[i].position.y*FRAMEBUFFER_WIDTH*3)+((xx)*3)+1] = 0;
            framebuffer[(items[i].position.y*FRAMEBUFFER_WIDTH*3)+((xx)*3)+2] = 0;
        }*/
    }
}


void findAndReplaceAll( std::string& data,
                        const std::string& match,
                        const std::string& replace)
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

    fte::freetypeInst *inst =  fte::initFreetype("/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", FONTSIZE);
    fte::makeBold(inst, false);
    //std::string htmlFile = htmlFileLoader("/home/tim/Documents/Development/gumbo-parser/docs/html/index.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/HTMLRenderer/HTML/simple_text.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/HTMLRenderer/HTML/Wikipedia.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/HTMLRenderer/HTML/Wikipedia - Wikipedia.html");
    //std::string htmlFile = htmlFileLoader("/home/tim/HTMLRenderer/HTML/Google.html");
    std::string htmlFile = htmlFileLoader("/home/tim/HTMLRenderer/HTML/GoogleOnChrome.html");

    findAndReplaceAll( htmlFile, "&nbsp;", " ");
    //GumboOutput* output = gumbo_parse("<body><h1>Hello, World!</h1></body><br><p>");
    GumboOutput* output = gumbo_parse(htmlFile.c_str());

    std::cout << "root has " << output->root->v.element.children.length << " children" << std::endl;
    //printinfo(inst, framebuffer, output->root, 0, 0);
    //traverseDOM(inst, framebuffer, output->root, 0, 0, true, GUMBO_TAG_UNKNOWN);

    RStyle docStyle;
    docStyle.display = "inline";
    docStyle.visible = false;
    docStyle.font_size = 16;
    docStyle.line_height = 1.2;
    docStyle.bold = false;
    docStyle.isLink = false;

    RTextInfoBox textInfoBox;
    textInfoBox.x = 0;
    textInfoBox.y = 0;//window.height/3;
    textInfoBox.w = 1920;
    textInfoBox.h = 1080;
    textInfoBox.textStartX = 0;
    textInfoBox.textStartY = 0;//window.height/3;
    docStyle.bold = false;

    renderRenderList(inst, assembleRenderList(output->root, inst, docStyle, &textInfoBox));

    //drawString(inst, "Are you still there?", 10, 32, framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

    stbi_write_png("out.png", FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 3, framebuffer, FRAMEBUFFER_WIDTH*3);

    X11Window window;
    window.createWindow("HTMLRenderer", 1920, 1080, 1920, 1080);
    //while(1){;}
    while (1)
    {
        window.processWindowEvents();
        RStyle docStyle;
        docStyle.display = "inline";
        docStyle.visible = false;
        docStyle.font_size = 16;
        docStyle.line_height = 1.2;
        docStyle.bold = false;
        docStyle.isLink = false;

        RTextInfoBox textInfoBox;
        textInfoBox.x = 0;
        textInfoBox.y = 0;//window.height/3;
        textInfoBox.w = window.width;
        textInfoBox.h = window.height;
        textInfoBox.textStartX = 0;
        textInfoBox.textStartY = 0;//window.height/3;
        memset(framebuffer, 255, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * 3);
        renderRenderList(inst, assembleRenderList(output->root, inst, docStyle, &textInfoBox));
        for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++)
        {
            framebuffer[(y*FRAMEBUFFER_WIDTH*3)+((textInfoBox.w-1)*3)] = 255;
            framebuffer[(y*FRAMEBUFFER_WIDTH*3)+((textInfoBox.w-1)*3)+1] = 0;
            framebuffer[(y*FRAMEBUFFER_WIDTH*3)+((textInfoBox.w-1)*3)+2] = 0;
        }


        for (int x = 0; x < FRAMEBUFFER_WIDTH; x++)
        {
            for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++)
            {
                window.displayBuffer[(y*1920*4)+(x*4)+2] = framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+0];
                window.displayBuffer[(y*1920*4)+(x*4)+1] = framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+1];
                window.displayBuffer[(y*1920*4)+(x*4)+0] = framebuffer[(y*FRAMEBUFFER_WIDTH*3)+(x*3)+2];
            }
        }

    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);

    return 0;
}
