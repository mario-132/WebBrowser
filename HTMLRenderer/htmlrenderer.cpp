#include "htmlrenderer.h"

HTMLRenderer::HTMLRenderer()
{

}

void HTMLRenderer::assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem &item, fte::freetypeInst *freetypeeasy)
{
    if (activeDocBox->activeRenderLineItemsCombo == 0)
    {
        activeDocBox->newRenderLineAndMakeActive(activeDocBox->x, activeDocBox->y);
    }

    if (item.type == RENDERDOM_ELEMENT)
    {
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
            std::wstring wide = converter.from_bytes(item.text);
            std::cout << item.text;

            std::wstring textToDisplay;
            int currentX = activeDocBox->activeRenderLineItemsCombo->renderLine.lineW +
                    activeDocBox->activeRenderLineItemsCombo->renderLine.lineX;
            item.style.font_size*=4;

            for (int i = 0; i < wide.size(); i++)
            {
                auto chrret = fte::getCharacterBounds(freetypeeasy, wide[i]);
                currentX += chrret.advanceX/64;
                textToDisplay += wide[i];
                if (currentX > activeDocBox->w || i == wide.size()-1)
                {
                    RItem ritem;
                    ritem.isBold = false;
                    ritem.font_size = item.style.font_size;
                    ritem.textcolor.a = 255;
                    ritem.textcolor.r = 0;
                    ritem.textcolor.g = 0;
                    ritem.textcolor.b = 0;
                    ritem.pos = RITEM_POS_BASELINE_RELATIVE;

                    RInternalRenderLine *acrl = &activeDocBox->activeRenderLineItemsCombo->renderLine;
                    ritem.type = RITEM_TEXT;
                    ritem.renderline = &activeDocBox->activeRenderLineItemsCombo->renderLine;
                    if (acrl->lineH < item.style.font_size)
                        acrl->lineH = item.style.font_size;
                    if (acrl->lineTextBaselineH < item.style.font_size)
                        acrl->lineTextBaselineH = item.style.font_size;

                    ritem.x = activeDocBox->activeRenderLineItemsCombo->renderLine.lineW;
                    acrl->lineW = currentX-activeDocBox->activeRenderLineItemsCombo->renderLine.lineX;

                    ritem.y = 0;

                    ritem.text = textToDisplay;
                    items->push_back(ritem);
                    if (currentX > activeDocBox->w)
                    {
                        activeDocBox->newRenderLineAndMakeActive(activeDocBox->x,
                                                                 activeDocBox->activeRenderLineItemsCombo->renderLine.lineY +
                                                                 activeDocBox->activeRenderLineItemsCombo->renderLine.lineH);
                        currentX = activeDocBox->activeRenderLineItemsCombo->renderLine.lineW +
                                activeDocBox->activeRenderLineItemsCombo->renderLine.lineX;
                    }
                    textToDisplay.clear();
                }

            }
            item.style.font_size/=4;
        }
    }
    else
    {
        // Unknown
        std::cerr << "Unknown RenderDOMItem type!" << std::endl;
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

                    auto inf = fte::drawCharacter(freetypeeasy, items[i].text[j], itX + x, itY-items[i].font_size, fb, w, h, false);// Draw character, saving info about it
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
        resY = item.renderline->lineY + item.y;
    }
    else
    {
        resX = -1;
        resY = -1;
    }
}
