#ifndef HTMLRENDERER_H
#define HTMLRENDERER_H
#include <vector>
#include "renderdom.h"
#include "freetypeeasy.h"
#include <codecvt>
#include <locale>

enum RItemType
{
    RITEM_UNKNOWN,
    RITEM_TEXT,
    RITEM_IMAGE,
    RITEM_COLORED_SQUARE
};

enum RItemPos
{
    RITEM_POS_FIXED,                // Position is based on the x and y(x and y represent top left of the item)
    RITEM_POS_BASELINE_RELATIVE     // Position is relative to the lineX and baselineh in renderline.
};

class RRenderLine
{
public:
    int lineX = 0;
    int lineY = 0;
    int lineW = 0;
    int lineH = 0;

    int lineTextBaselineH = 0;

    void lineHResize(int newH)
    {
        lineH = newH;
    }

    void lineTextBaselineHResize(int newtbl)
    {
        lineTextBaselineH = newtbl;
        if (lineTextBaselineH > lineH)
            lineH = lineTextBaselineH;
    }

    //bool isFull = 0;
};

class RItem
{
public:
    RItem(RItemType type, RRenderLine *renderline, RItemPos pos, int x, int y)
    {
        this->type = type;
        this->renderline = renderline;
        this->pos = pos;
        this->x = x;
        this->y = y;

        font_size = 16;
        isBold = false;
        textcolor = {0, 0, 0, 255};
    }
    RItemType type;

    RRenderLine *renderline;
    std::wstring text;
    int font_size;// font size in px
    bool isBold;
    RenderDOMColor textcolor;

    RItemPos pos;
    int x;
    int y;
    //int w;
    //int h;
};

class RDocumentBox
{
public:
    RDocumentBox(int x, int y, int w, int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        nextLineYOff = 0;
    }
    int x;
    int y;
    int w;
    int h;

    std::vector<RRenderLine*> renderlines;
    int nextLineYOff;
};

class HTMLRenderer
{
public:
    HTMLRenderer();

    int yScroll;

    void assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem &item, fte::freetypeInst *freetypeeasy);
    void renderRenderList(const std::vector<RItem> &items, fte::freetypeInst *freetypeeasy, unsigned char *fb, int w, int h);
    void calcItemXY(const RItem &item, int &resX, int &resY);

private:
    void addNewEmptyRenderline(RDocumentBox *activeDocBox, int h);
};

#endif // HTMLRENDERER_H
