#ifndef RENDERDOM_H
#define RENDERDOM_H
#include <gumbo.h>
#include <vector>
#include <iostream>
#include "css.h"

/// What to use is set in the cmake
//#define USEMAGICK true
//#define USESTBI true

#ifdef USESTBI
#include "stb_image.h"
#endif
#ifdef USEMAGICK
#include <ImageMagick-6/Magick++.h>
#endif

struct RenderDOMColor
{
    unsigned int r;
    unsigned int g;
    unsigned int b;
    unsigned int a;
};

enum RenderDOMType
{
    RENDERDOM_UNKNOWN,
    RENDERDOM_ELEMENT,
    RENDERDOM_TEXT
};

enum RenderDOMCSSProp
{
    RENDERDOM_NONE,
    RENDERDOM_VALUE,
    RENDERDOM_INHERIT
};

struct RenderDOMStyle
{
    css_display_e display;// Display

    RenderDOMColor color;// Color
    RenderDOMCSSProp color_type;

    RenderDOMColor background_color;// Background-color
    RenderDOMCSSProp background_color_type;

    int font_size;// Font-size, in px
    RenderDOMCSSProp font_size_type;
};

struct RenderDOMItem
{
    RenderDOMType type;
    std::vector<RenderDOMItem> children;

    RenderDOMStyle style;
    std::string tag;
};

class RenderDOM/// Takes the html dom and css and puts it together
{
public:
    RenderDOM();
    RenderDOMItem parseGumboTree(GumboNode *node, CSS *css, RenderDOMStyle prev);
};

#endif // RENDERDOM_H
