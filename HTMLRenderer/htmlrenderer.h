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

class RDocumentBox;

class RRenderLine
{
public:
    RRenderLine(RDocumentBox *docBox, int x, int y)
    {
        this->docBox = docBox;
        lineX = x;
        lineY = y;
    }
    int lineX = 0;
    int lineY = 0;
    int lineW = 0;
    int lineH = 0;

    int lineTextBaselineH = 0;

    RDocumentBox *docBox;

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
    RenderDOMColor textbgcolor;

    RItemPos pos;
    int x;
    int y;
    int w;
    int h;
};

class RDocumentBox
{
public:
    RDocumentBox(RDocumentBox *dparent, int x, int y, int w, int h, bool wlocked, bool hlocked)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        wLocked = wlocked;
        hLocked = hlocked;
        nextLineYOff = 0;
        parent = dparent;
    }
    int x;
    int y;
    int w;
    int h;
    bool wLocked;
    bool hLocked;

    bool docboxIsRoot = false;
    RDocumentBox *parent;

    std::vector<RRenderLine*> renderlines;
    int nextLineYOff;

    std::vector<RDocumentBox*> childBoxes;

    void updateWandH(int w, int h)
    {
        if (!wLocked && w > this->w)
            this->w = w;
        if (!hLocked && h > this->h)
            this->h = h;
    }
};

class HTMLRenderer
{
public:
    HTMLRenderer();

    int yScroll;

    void assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem &item, fte::freetypeInst *freetypeeasy);
    void renderRenderList(const std::vector<RItem> &items, fte::freetypeInst *freetypeeasy, unsigned char *fb, int w, int h, float scale);
    //void calcItemXY(const RItem &item, int &resX, int &resY);

private:
    void addNewEmptyRenderline(RDocumentBox *activeDocBox, int h);

    void stringReplaceAll(std::string& str, const std::string& oldStr, const std::string& newStr);
    void wstringReplaceAll(std::wstring& str, const std::wstring& oldStr, const std::wstring& newStr);
public:

    int getGlobX(RRenderLine *line);
    int getGlobY(RRenderLine *line);

    int getGlobX(RDocumentBox *db);
    int getGlobY(RDocumentBox *db);

    int getGlobX(const RItem *item);
    int getGlobY(const RItem *item);
};

#endif // HTMLRENDERER_H
