#ifndef HTMLRENDERER_H
#define HTMLRENDERER_H
#include <string>
#include <vector>
#include "freetypeeasy.h"
#include "renderdom.h"

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
    bool isValid;
    unsigned char* imageData;
    int w;
    int h;
    int comp;
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

class HTMLRenderer
{
public:
    HTMLRenderer();

    int framebufferWidth;
    int framebufferHeight;
    unsigned char* framebuffer;

    //void applyStyle(GumboNode* node, RStyle &style);

    // Make stylebox more like a textbox and make it a reference so they can all access it, then when a div occurs you make a new stylebox/textbox and give it that.
    std::vector<RItem> assembleRenderList(RenderDOMItem &root, fte::freetypeInst *inst, RDocumentBox *documentBox, RenderDOMStyle style);

    void renderRenderList(fte::freetypeInst *inst, std::vector<RItem> items);
};

#endif // HTMLRENDERER_H
