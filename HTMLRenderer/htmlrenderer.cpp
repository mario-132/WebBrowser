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
        if (item.style.display == CSS_DISPLAY_BLOCK)// Make sure block starts at a new line
        {
            addNewEmptyRenderline(activeDocBox, 0);
            // Give the new line no height since its not a br and we want the items to come directly after it
        }

        if (item.tag == "br")
        {
            addNewEmptyRenderline(activeDocBox, item.style.font_size);
            // br creates a new line and gives it a start height equal to font size.
        }

        if (item.style.display == CSS_DISPLAY_BLOCK)
        {
            addNewEmptyRenderline(activeDocBox, 0);
            RDocumentBox docBox2(activeDocBox->renderlines.back()->lineX,
                                 activeDocBox->renderlines.back()->lineY,
                                 0,
                                 0,
                                 false,
                                 false);
            for (int i = 0; i < item.children.size(); i++)
            {
                assembleRenderList(items, &docBox2, item.children[i], freetypeeasy);// Iterate over children
            }

            RItem bg(RITEM_COLORED_SQUARE, activeDocBox->renderlines.back(), RITEM_POS_FIXED, docBox2.x, docBox2.y);
            bg.w = docBox2.w;
            bg.h = docBox2.h;
            bg.textcolor = {item.style.background_color.r,
                              item.style.background_color.g,
                              item.style.background_color.b,
                              item.style.background_color.a};
            items->insert(items->begin(), bg);

            activeDocBox->renderlines.back()->lineHResize(docBox2.h);
            activeDocBox->renderlines.back()->lineW = docBox2.w;
            activeDocBox->updateWandH((activeDocBox->renderlines.back()->lineX + activeDocBox->renderlines.back()->lineW)-activeDocBox->x,
                                      (activeDocBox->renderlines.back()->lineY + activeDocBox->renderlines.back()->lineH)-activeDocBox->y);
        }
        else
        {
            for (int i = 0; i < item.children.size(); i++)
            {
                assembleRenderList(items, activeDocBox, item.children[i], freetypeeasy);// Iterate over children
            }
        }

        if (item.style.display == CSS_DISPLAY_BLOCK)// Make sure the items after block start at a new line
        {
            addNewEmptyRenderline(activeDocBox, 0);
            // Give the new line no height since its not a br and we want the items to come directly after it
        }
    }
    else if (item.type == RENDERDOM_TEXT)
    {
        if (item.style.display != CSS_DISPLAY_NONE)
        {
            std::string itemText = item.text;
            stringReplaceAll(itemText, "\n", "");
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wText = converter.from_bytes(itemText);

            // Get rid of the line direction characters:
            wchar_t chR = 8234;
            std::wstring sR;
            sR.push_back(chR);
            wstringReplaceAll(wText, sR, std::wstring());

            wchar_t chR2 = 8236;
            std::wstring sR2;
            sR2.push_back(chR2);
            wstringReplaceAll(wText, sR2, std::wstring());

            RRenderLine *line = activeDocBox->renderlines.back();
            int xP = line->lineX + line->lineW;

            std::wstring textOut;

            fte::makeBold(freetypeeasy, (item.style.font_weight > 600));
            fte::setFontSize(freetypeeasy, item.style.font_size);

            for (unsigned int i = 0; i < wText.size(); i++)
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
                    ritem.isBold = (item.style.font_weight > 600);
                    ritem.textcolor = {item.style.color.r,
                                      item.style.color.g,
                                      item.style.color.b,
                                      item.style.color.a};
                    ritem.textbgcolor = {item.style.background_color.r,
                                         item.style.background_color.g,
                                         item.style.background_color.b,
                                         item.style.background_color.a};
                    ritem.w = xP-(line->lineX + line->lineW);// This is so the actual renderer knows where to render the bg

                    if (line->lineH < item.style.font_size)
                        line->lineHResize(item.style.font_size);
                    if (line->lineTextBaselineH < item.style.font_size)
                        line->lineTextBaselineHResize(item.style.font_size);
                    line->lineW += xP-(line->lineX + line->lineW);

                    activeDocBox->updateWandH((activeDocBox->renderlines.back()->lineX + activeDocBox->renderlines.back()->lineW)-activeDocBox->x,
                                              (activeDocBox->renderlines.back()->lineY + activeDocBox->renderlines.back()->lineH)-activeDocBox->y);

                    if (xP > (activeDocBox->x + activeDocBox->w))
                    {
                        activeDocBox->nextLineYOff += line->lineH;
                        activeDocBox->renderlines.push_back(new RRenderLine());
                        activeDocBox->renderlines.back()->lineX = activeDocBox->x;
                        activeDocBox->renderlines.back()->lineY = activeDocBox->nextLineYOff + activeDocBox->y;
                        line = activeDocBox->renderlines.back();
                        xP = line->lineX + line->lineW;

                        activeDocBox->updateWandH((activeDocBox->renderlines.back()->lineX + activeDocBox->renderlines.back()->lineW)-activeDocBox->x,
                                                  (activeDocBox->renderlines.back()->lineY + activeDocBox->renderlines.back()->lineH)-activeDocBox->y);
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

void HTMLRenderer::renderRenderList(const std::vector<RItem> &items, fte::freetypeInst *freetypeeasy, unsigned char *fb, int w, int h, float scale)
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].type == RITEM_TEXT)
        {
            fte::makeBold(freetypeeasy, items[i].isBold);
            fte::setFontSize(freetypeeasy, items[i].font_size*scale);
            fte::setTextColor(freetypeeasy, (items[i].textcolor.r)/255.0f, (items[i].textcolor.g)/255.0f, (items[i].textcolor.b)/255.0f);

            int itX;
            int itY;
            calcItemXY(items[i], itX, itY);
            itX *= scale;
            itY *= scale;

            if (items[i].textbgcolor.a > 10)
            {
                for (int x = itX; x < itX+items[i].w; x++)
                {
                    for (int y = itY+yScroll; y < itY+items[i].font_size+yScroll; y++)
                    {
                        if (x < 0 || y < 0 || x > w || y > h)
                            continue;
                        fb[(w*y*3)+(x*3)+0] = items[i].textbgcolor.r;
                        fb[(w*y*3)+(x*3)+1] = items[i].textbgcolor.g;
                        fb[(w*y*3)+(x*3)+2] = items[i].textbgcolor.b;
                    }
                }
            }

            int x = 0;
            for (unsigned int j = 0; j < items[i].text.size(); j++)
            {
                if (itX + x > w || itY+items[i].font_size+yScroll > h)
                    continue;

                auto inf = fte::drawCharacter(freetypeeasy, items[i].text[j], itX + x, itY+items[i].font_size+yScroll, fb, w, h, false);// Draw character, saving info about it
                x += inf.advanceX/64;// Advance x position for next character
            }
        }
        else if (items[i].type == RITEM_COLORED_SQUARE)
        {
            int itX;
            int itY;
            calcItemXY(items[i], itX, itY);
            itX *= scale;
            itY *= scale;
            if (items[i].textcolor.a > 10)
            {
                for (int x = itX; x < itX+items[i].w; x++)
                {
                    for (int y = itY+yScroll; y < itY+items[i].h+yScroll; y++)
                    {
                        if (x < 0 || y < 0 || x >= w || y >= h)
                            continue;
                        fb[(w*y*3)+(x*3)+0] = items[i].textcolor.r;
                        fb[(w*y*3)+(x*3)+1] = items[i].textcolor.g;
                        fb[(w*y*3)+(x*3)+2] = items[i].textcolor.b;
                    }
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

void HTMLRenderer::addNewEmptyRenderline(RDocumentBox *activeDocBox, int h)
{
    activeDocBox->nextLineYOff += activeDocBox->renderlines.back()->lineH;
    activeDocBox->renderlines.push_back(new RRenderLine());
    activeDocBox->renderlines.back()->lineX = activeDocBox->x;
    activeDocBox->renderlines.back()->lineY = activeDocBox->nextLineYOff + activeDocBox->y;
    activeDocBox->renderlines.back()->lineH = h;

    activeDocBox->updateWandH((activeDocBox->renderlines.back()->lineX + activeDocBox->renderlines.back()->lineW)-activeDocBox->x,
                              (activeDocBox->renderlines.back()->lineY + activeDocBox->renderlines.back()->lineH)-activeDocBox->y);
}

void HTMLRenderer::stringReplaceAll(std::string &str, const std::string &oldStr, const std::string &newStr)
{
    std::string::size_type pos = 0u;
    while((pos = str.find(oldStr, pos)) != std::string::npos){
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

void HTMLRenderer::wstringReplaceAll(std::wstring &str, const std::wstring &oldStr, const std::wstring &newStr)
{
    std::wstring::size_type pos = 0u;
    while((pos = str.find(oldStr, pos)) != std::wstring::npos){
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}
