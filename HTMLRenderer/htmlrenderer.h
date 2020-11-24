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

struct RInternalRenderLine
{
    int lineX;
    int lineY;
    int lineW;
    int lineH;

    int lineTextBaselineH;
};

struct RItem
{
    RItemType type;

    RInternalRenderLine *renderline;
    std::wstring text;
    int font_size;// font size in px
    bool isBold;
    RenderDOMColor textcolor;

    RItemPos pos;
    int x;
    int y;
    int w;
    int h;
};

struct RRenderLineItemsCombo
{
    RInternalRenderLine renderLine;
    std::vector<RItem*> renderItemPointers;
};

class RDocumentBox
{
public:
    int x;
    int y;
    int w;

    std::vector<RRenderLineItemsCombo> renderLinesCombos;
    RRenderLineItemsCombo *activeRenderLineItemsCombo = 0;
    void newRenderLineAndMakeActive(int x, int y)
    {
        RRenderLineItemsCombo rline;

        rline.renderLine.lineX = x;
        rline.renderLine.lineY = y;
        rline.renderLine.lineW = 0;
        rline.renderLine.lineH = 0;
        rline.renderLine.lineTextBaselineH = 0;

        renderLinesCombos.push_back(rline);
        activeRenderLineItemsCombo = &renderLinesCombos.back();
    }
    std::vector<RItem*> renderItemPointers;

    std::vector<RDocumentBox> childDocBoxes;
};

class HTMLRenderer
{
public:
    HTMLRenderer();

    void assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem &item, fte::freetypeInst *freetypeeasy);
    void renderRenderList(const std::vector<RItem> &items, fte::freetypeInst *freetypeeasy, unsigned char *fb, int w, int h);
    void calcItemXY(const RItem &item, int &resX, int &resY);
};

#endif // HTMLRENDERER_H
