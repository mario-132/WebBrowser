#ifndef RENDERDOM_H
#define RENDERDOM_H
#include <gumbo.h>
#include <vector>
#include <iostream>

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

class RenderDOM/// Takes the html dom and css and puts it together
{
public:
    RenderDOM();
    std::string gumboTagToString(GumboTag tag);
    void parseGumboTree(GumboNode *node);
};

#endif // RENDERDOM_H
