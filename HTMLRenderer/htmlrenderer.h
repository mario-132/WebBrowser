#ifndef HTMLRENDERER_H
#define HTMLRENDERER_H
#include <vector>
#include "renderdom.h"

struct RRenderLine
{
    int lineX;
    int lineY;
    int lineW;
    int lineH;

    int lineTextBaselineH;
};

enum RItemType
{
    RITEM_UNKNOWN,
    RITEM_TEXT,
    RITEM_COLORED_SQUARE
};

enum RItemPos
{
    RITEM_POS_FIXED,        // Position is based on the x and y
    RITEM_POS_ON_BASELINE   // Position is bottom on the top of the textbaselineh in &renderline.
};

struct RItem
{
    RItemType type;

    RRenderLine &renderline;
    std::string text;

    int x;
    int y;
};

struct RDocumentBox
{

};

class HTMLRenderer
{
public:
    HTMLRenderer();

    void assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem item);
    void renderRenderList(const std::vector<RItem> &items);
};

#endif // HTMLRENDERER_H
