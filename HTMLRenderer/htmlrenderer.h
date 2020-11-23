#ifndef HTMLRENDERER_H
#define HTMLRENDERER_H
#include <vector>
#include "renderdom.h"

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

struct RRenderLine
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

    RRenderLine &renderline;
    std::string text;

    RItemPos pos;
    int x;
    int y;
    int w;
    int h;
};

struct RRenderLineItemsCombo
{
    RRenderLine renderLine;
    std::vector<RItem*> renderItemPointers;
};

struct RDocumentBox
{
    std::vector<RRenderLineItemsCombo> renderLinesCombos;
    std::vector<RItem*> renderItemPointers;

    std::vector<RDocumentBox> childDocBoxes;
};

class HTMLRenderer
{
public:
    HTMLRenderer();

    void assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem &item);
    void renderRenderList(const std::vector<RItem> &items);
};

#endif // HTMLRENDERER_H
