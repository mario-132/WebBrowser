#ifndef RENDERDOM_H
#define RENDERDOM_H
#include <gumbo.h>
#include <vector>
#include <iostream>
#include "cssparse.h"

struct RenderDOMColor
{
    unsigned int r;
    unsigned int g;
    unsigned int b;
};

struct RenderDOMStyle
{
    std::string display;
    //bool visible;
    //bool blockOrInline;// 0 = inline 1 = bloc
    int font_size;
    float line_height;
    std::string text_align;

    RenderDOMColor color;
    RenderDOMColor background_color;

    bool bold;
    bool isLink;
};

enum RenderDOMType
{
    RENDERDOM_ELEMENT,
    RENDERDOM_TEXT,
    RENDERDOM_NONE
};

struct RenderDOMAttribute
{
    std::string name;
    std::string value;
};

struct RenderDOMImage
{
    std::string imageDataRaw;
    unsigned char* imageData;
    int imgW;
    int imgH;
    int comp;

    bool downloaded;
    bool decoded;
};

struct RenderDOMElement
{
    GumboTag tag;
    std::vector<RenderDOMAttribute> attributes;
    RenderDOMStyle style;

    RenderDOMImage image;// Only use this if tag == GUMBO_TAG_IMG
};

struct RenderDOMItem
{
    RenderDOMElement element;
    std::string text;
    RenderDOMType type;

    std::vector<RenderDOMItem> children;
};

struct DOMStackItem
{
    std::string tag;
    std::string unparsedClasses;
    std::string unparsedIDs;
};

class RenderDOM
{
public:
    RenderDOM();
    RenderDOMItem parseGumboTree(GumboNode *node, RenderDOMStyle style, std::string baseURL, std::vector<css::CSSSelectorBlock> &css);
    std::vector<DOMStackItem> domCallStack;
    void applyStyle(RenderDOMItem &item, RenderDOMStyle &style);
    bool checkSelectorMatch(std::string selector, const DOMStackItem &item);
};

#endif // RENDERDOM_H
