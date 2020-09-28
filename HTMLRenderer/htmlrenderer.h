#ifndef HTMLRENDERER_H
#define HTMLRENDERER_H
#include <string>
#include <vector>
#include <deque>
#include "freetypeeasy.h"
#include "renderdom.h"

/// Container for the text in a RItem, includes info like lineID and textSize.
struct RText
{
    //int lineID;
    int textSize;
    std::wstring text;

    RenderDOMColor color;
    RenderDOMColor background_color;

    bool bold;
    bool isLink;
};

/// Position and dimensions
struct RPosition
{
    int x;
    int y;
    int w;
    int h;
};

/// An image
struct RImage
{
    bool isValid;
    unsigned char* imageData;
    int w;
    int h;
    int comp;
};

/// Contains the type of item in a RItem. the names speak for themselves.
enum RType
{
    RITEM_UNKNOWN,
    RITEM_NONE,
    RITEM_TEXT,
    RITEM_IMAGE,
    RITEM_COLORED_SQUARE
};

/// The RItem, contains all the data needed to draw it. Type variable stores what type of renderable this is,
/// e.g. RITEM_TEXT for text and RITEM_IMAGE for an image.
struct RItem
{
    RText text;
    RImage img;
    RPosition position;
    RenderDOMColor squareColor;

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

struct RItemLine
{
    int lineX;
    int lineY;
    int lineH;
    int baselineH;
    int lineW;
    std::vector<RItem*> items;// Item pointers aren't owned
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

    //int textStartX;
    //int textStartY;

    std::vector<RDocumentBox> childBoxes;

    std::vector<RItemLine> itemLines;
};

class HTMLRenderer
{
public:
    HTMLRenderer();

    int framebufferWidth;
    int framebufferHeight;
    unsigned char* framebuffer;

    //void applyStyle(GumboNode* node, RStyle &style);

    /// THESE SHOULD BE TEMPORARY FOR NOW, THEY ARE JUST FOR THE CSS STYLE DEBUGGER!
    int mouseX;
    int mouseY;
    bool pressed;
    /// This page switcher is TEMPORARY as well!! Should be implemented in a better way later both in .h and .cpp.
    std::string _nextPageTemp;
    std::string nextpage;
    bool switchPage;

    int yScroll;

    // Make stylebox more like a textbox and make it a reference so they can all access it, then when a div occurs you make a new stylebox/textbox and give it that.
    void assembleRenderList(RenderDOMItem &root, fte::freetypeInst *inst, RDocumentBox *documentBox, RenderDOMStyle style);
    RPosition assembleRenderListV2(RenderDOMItem &root, fte::freetypeInst *inst, RDocumentBox *documentBox, RenderDOMStyle style);
    void renderRenderList(fte::freetypeInst *inst, std::vector<RItem> items);
    std::vector<RItem> RenderItems;

private:
    void changeBaselineHeight(int newHeight, RItemLine &line);
    void changeLineHeightBottom(int newHeight, RItemLine &line);
};

#endif // HTMLRENDERER_H
