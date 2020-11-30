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
    RENDERDOM_NONE,     // Unknown value/value doesn't apply here
    RENDERDOM_VALUE,    // px value
    //RENDERDOM_INHERIT,  // Take value from previous element
    RENDERDOM_AUTO      // Resize with content
};

struct RenderDOMStyle
{
    css_display_e display;// Display

    RenderDOMColor color;// Color
    RenderDOMCSSProp color_type;
    css_color_e _was_color;

    RenderDOMColor background_color;// Background-color
    RenderDOMCSSProp background_color_type;
    css_background_color_e _was_bg_color;

    int font_size;// Font-size, in px
    RenderDOMCSSProp font_size_type;

    int height;// height, in px
    RenderDOMCSSProp height_type;

    int width;// width, in px
    RenderDOMCSSProp width_type;

    int font_weight;// Weight
};

struct RenderDOMAttribute
{
    std::string name;
    std::string value;
};

struct RenderDOMItem
{
    RenderDOMType type;
    std::vector<RenderDOMItem> children;

    RenderDOMStyle style;
    std::string tag;

    std::string text;// if RENDERDOM_TEXT

    std::vector<RenderDOMAttribute> attributes;
};

class RenderDOM/// Takes the html dom and css and puts it together
{
public:
    RenderDOM();
    RenderDOMItem parseGumboTree(GumboNode *node, CSS *css, RenderDOMStyle prev);
    int unitToPx(css_fixed fixed, css_unit unit, int prev, int prevFnt);
};

#endif // RENDERDOM_H
