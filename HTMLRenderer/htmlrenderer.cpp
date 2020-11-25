#include "htmlrenderer.h"

HTMLRenderer::HTMLRenderer()
{

}

void HTMLRenderer::assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem &item, fte::freetypeInst *freetypeeasy)
{
    if (activeDocBox->renderlines.size() == 0)
    {
        activeDocBox->renderlines.push_back(new RRenderLine());
        activeDocBox->renderlines.back()->lineX = activeDocBox->x;
        activeDocBox->renderlines.back()->lineY = activeDocBox->y;
    }

    if (item.type == RENDERDOM_ELEMENT)
    {
        if (item.tag == "br")
        {
            activeDocBox->nextLineYOff += activeDocBox->renderlines.back()->lineH;
            activeDocBox->renderlines.push_back(new RRenderLine());
            activeDocBox->renderlines.back()->lineX = activeDocBox->x;
            activeDocBox->renderlines.back()->lineY = activeDocBox->nextLineYOff + activeDocBox->y;
            activeDocBox->renderlines.back()->lineH = item.style.font_size;
        }
        for (int i = 0; i < item.children.size(); i++)
        {
            assembleRenderList(items, activeDocBox, item.children[i], freetypeeasy);
        }
    }
    else if (item.type == RENDERDOM_TEXT)
    {
        if (item.style.display != CSS_DISPLAY_NONE)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wText = converter.from_bytes(item.text);
            std::cout << item.text;

            RRenderLine *line = activeDocBox->renderlines.back();
            int xP = line->lineX + line->lineW;

            std::wstring textOut;

            fte::makeBold(freetypeeasy, false);
            fte::setFontSize(freetypeeasy, item.style.font_size);

            for (int i = 0; i < wText.size(); i++)
            {
                auto chrret = fte::getCharacterBounds(freetypeeasy, wText[i]);
                xP += chrret.advanceX/64;
                textOut += wText[i];
                if (xP > (activeDocBox->x + activeDocBox->w) || i == wText.size()-1)
                {
                    RItem ritem(RITEM_TEXT,
                               line,
                               RITEM_POS_BASELINE_RELATIVE,
                               line->lineW,
                               -item.style.font_size);
                    ritem.text = textOut;
                    ritem.font_size = item.style.font_size;
                    ritem.isBold = false;
                    ritem.textcolor = {item.style.background_color.r,
                                      item.style.background_color.g,
                                      item.style.background_color.b,
                                      255};

                    if (line->lineH < item.style.font_size)
                        line->lineHResize(item.style.font_size);
                    if (line->lineTextBaselineH < item.style.font_size)
                        line->lineTextBaselineHResize(item.style.font_size);
                    line->lineW += xP-line->lineW;
                    if (xP > (activeDocBox->x + activeDocBox->w))
                    {
                        activeDocBox->nextLineYOff += line->lineH;
                        activeDocBox->renderlines.push_back(new RRenderLine());
                        activeDocBox->renderlines.back()->lineX = activeDocBox->x;
                        activeDocBox->renderlines.back()->lineY = activeDocBox->nextLineYOff + activeDocBox->y;
                        line = activeDocBox->renderlines.back();
                        xP = line->lineX + line->lineW;
                    }
                    textOut.clear();
                    items->push_back(ritem);
                }
            }
        }
    }
    else
    {
        // Unknown
        //std::cerr << "Unknown RenderDOMItem type!" << std::endl;
    }
}

void HTMLRenderer::renderRenderList(const std::vector<RItem> &items, fte::freetypeInst *freetypeeasy, unsigned char *fb, int w, int h)
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].type == RITEM_TEXT)
        {
            fte::makeBold(freetypeeasy, items[i].isBold);
            fte::setFontSize(freetypeeasy, items[i].font_size);
            fte::setTextColor(freetypeeasy, (items[i].textcolor.r)/255.0f, (items[i].textcolor.g)/255.0f, (items[i].textcolor.b)/255.0f);

            int x = 0;
            if (items[i].y < h && items[i].x < w)// Make sure we are not trying to write a character off screen
            {
                for (unsigned int j = 0; j < items[i].text.size(); j++)
                {
                    int itX;
                    int itY;
                    calcItemXY(items[i], itX, itY);

                    auto inf = fte::drawCharacter(freetypeeasy, items[i].text[j], itX + x, itY+items[i].font_size, fb, w, h, false);// Draw character, saving info about it
                    x += inf.advanceX/64;// Advance x position for next character
                }
            }
        }
    }
}

void HTMLRenderer::calcItemXY(const RItem &item, int &resX, int &resY)
{
    if (item.pos == RITEM_POS_FIXED)
    {
        resX = item.x;
        resY = item.y;
    }
    else if (item.pos == RITEM_POS_BASELINE_RELATIVE)
    {
        resX = item.renderline->lineX + item.x;
        resY = item.renderline->lineY + item.renderline->lineTextBaselineH + item.y;
    }
    else
    {
        resX = -1;
        resY = -1;
    }
}
