#include "htmlrenderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "webservice.h"
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include "debugger.h"

HTMLRenderer::HTMLRenderer()
{
    switchPage = false;
}

RPosition HTMLRenderer::assembleRenderListV2(RenderDOMItem &root, freetypeeasy::freetypeInst *inst, RDocumentBox *documentBox, RenderDOMStyle activeStyle)
{
    RPosition itempos;
    itempos.x = 0;
    itempos.y = 0;
    itempos.w = 0;
    itempos.h = 0;

    if (documentBox->itemLines.size() == 0)
    {
        RItemLine line;
        line.lineH = 0;
        line.baselineH = 0;
        line.lineW = 0;
        line.lineX = documentBox->x;
        line.lineY = documentBox->y;
        documentBox->itemLines.push_back(line);
    }

    if (root.type == RENDERDOM_ELEMENT)
    {
        activeStyle = root.element.style;
        if (activeStyle.isLink)
        {
            for (unsigned int i = 0; i < root.element.attributes.size(); i++)
            {
                if (std::string("href") == root.element.attributes[i].name)
                {
                    _nextPageTemp = root.element.attributes[i].value;
                }
            }
        }

        if (activeStyle.display == "block" && (documentBox->itemLines.back().items.size() > 0 || root.element.tag == GUMBO_TAG_BR))
        {
            RItemLine nline;
            nline.lineH = activeStyle.font_size*activeStyle.line_height;
            nline.baselineH = 0;
            nline.lineW = 0;
            nline.lineX = documentBox->x;
            nline.lineY = documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineH;
            documentBox->itemLines.push_back(nline);

            if (documentBox->y + documentBox->h < nline.lineY + nline.lineH)
                documentBox->h = (nline.lineY + nline.lineH)-documentBox->y;
        }

        if (root.element.tag == GUMBO_TAG_HR)
        {
            RItem item;
            item.type = RITEM_COLORED_SQUARE;
            item.position.x = documentBox->x + 5;
            item.position.y = documentBox->itemLines.back().lineY + yScroll;
            item.position.w = documentBox->w - 10;
            item.position.h = 1;
            item.squareColor.r = 0;
            item.squareColor.g = 0;
            item.squareColor.b = 0;
            item.squareColor.a = 255;
            RenderItems.push_back(item);
            documentBox->itemLines.back().items.push_back(&RenderItems.back());
            documentBox->itemLines.back().lineW = item.position.w;
        }

        if (root.element.tag == GUMBO_TAG_IMG || root.element.tag == GUMBO_TAG_IMAGE)
        {
            RItem item;
            item.type = RITEM_IMAGE;// Item is an image
            item.img.isValid = false;// Mark invalid, will be marked valid later if it is.

            // Store image dimenstions
            int imgW = 50;
            int imgH = 50;

            // Find img dimensions in html
            for (unsigned int i = 0; i < root.element.attributes.size(); i++)
            {
                if (std::string("width") == root.element.attributes[i].name)
                {
                    imgW = std::stoi(root.element.attributes[i].value);
                }
                if (std::string("height") == root.element.attributes[i].name)
                {
                    imgH = std::stoi(root.element.attributes[i].value);
                }
            }

            if (root.element.image.decoded)// If image has been downloaded and decoded
            {
                imgW = root.element.image.imgW;
                imgH = root.element.image.imgH;
                item.img.isValid = true;
                item.img.w = imgW;
                item.img.h = imgH;
                item.img.comp = root.element.image.comp;
                item.img.imageData = root.element.image.imageData;
            }

            item.position.w = imgW;
            item.position.h = imgH;

            RItemLine &line = documentBox->itemLines.back();
            if (line.baselineH < imgH)
            {
                changeBaselineHeight(imgH, line);
            }


            item.position.x = line.lineX + line.lineW;
            item.position.y = ((line.lineY + line.baselineH)-imgH) + yScroll;

            itempos.x = line.lineX + line.lineW;
            itempos.y = ((line.lineY + line.baselineH)-imgH) + yScroll;
            itempos.w = imgW;
            itempos.h = imgH;

            line.lineW += imgW;
            if (line.lineH < imgH)
            {
                line.lineH = imgH;
            }

            RenderItems.push_back(item);
            line.items.push_back(&RenderItems.back());

            if (documentBox->y + documentBox->h < line.lineY + line.lineH)
                documentBox->h = (line.lineY + line.lineH)-documentBox->y;
        }

        // Parse the child elements.
        if (activeStyle.display == "block")
        {
            unsigned int _off = 0;
            if (Debugger::getCheckboxEnabled("debug_docbox_1px"))
                _off = 1;
            RDocumentBox docBox;
            docBox.x = documentBox->x+_off;
            docBox.y = documentBox->y+_off+documentBox->h;
            docBox.w = documentBox->w-_off-_off;
            docBox.h = _off;

            documentBox->childBoxes.push_back(docBox);
            for (unsigned int i = 0; i < root.children.size(); i++)
            {
                RPosition pos = assembleRenderListV2(root.children[i], inst, &documentBox->childBoxes.back(), activeStyle);
            }

            RItemLine nline;
            nline.lineH = documentBox->childBoxes.back().h;
            nline.baselineH = 0;
            nline.lineW = 0;
            nline.lineX = documentBox->x;
            nline.lineY = documentBox->y+1+documentBox->h;//documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineH;
            documentBox->itemLines.push_back(nline);
            if (documentBox->y + documentBox->h < nline.lineY + nline.lineH)
                documentBox->h = (nline.lineY + nline.lineH)-documentBox->y;
            RItem item;
            item.type = RITEM_NONE;
            if (activeStyle.background_color.a != 0)
            {

                item.type = RITEM_COLORED_SQUARE;
                item.position.x = documentBox->childBoxes.back().x;
                item.position.y = documentBox->childBoxes.back().y + yScroll;
                item.position.w = documentBox->childBoxes.back().w;
                item.position.h = documentBox->childBoxes.back().h;
                item.squareColor.r = activeStyle.background_color.r;
                item.squareColor.g = activeStyle.background_color.g;
                item.squareColor.b = activeStyle.background_color.b;
                item.squareColor.a = activeStyle.background_color.a;
                //RenderItems.push_back(item);
            }
            RenderItems.insert(RenderItems.begin(), item);
            documentBox->itemLines.back().items.push_back(&RenderItems.front());// We add the bg item(even if RITEM_NONE) to the line containing the block so that it causes a newline by the code below.
        }
        else
        {
            for (unsigned int i = 0; i < root.children.size(); i++)
            {
                RPosition pos = assembleRenderListV2(root.children[i], inst, documentBox, activeStyle);
            }
        }

        if (activeStyle.display == "block" && documentBox->itemLines.back().items.size() > 0)
        {
            RItemLine nline;
            nline.lineH = 0;
            nline.baselineH = 0;
            nline.lineW = 0;
            nline.lineX = documentBox->x;
            nline.lineY = documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineH;
            documentBox->itemLines.push_back(nline);

            if (documentBox->y + documentBox->h < nline.lineY + nline.lineH)
                documentBox->h = (nline.lineY + nline.lineH)-documentBox->y;
        }
    }
    else if (root.type == RENDERDOM_TEXT)
    {
        if (activeStyle.display != "none")
        {
            int fontsize = activeStyle.font_size;
            fte::setFontSize(inst, fontsize);

            int cX = documentBox->itemLines.back().lineX + documentBox->itemLines.back().lineW;
            //int cY = documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineH;

            int charactersPreWritten = 0;

            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wide = converter.from_bytes(root.text);
            for (unsigned int i = 0; i < wide.size(); i++)
            {
                fte::makeBold(inst, activeStyle.bold);
                fte::glyphInfo inf = fte::getCharacterBounds(inst, wide[i]);
                cX += inf.advanceX/64;
                if (cX > documentBox->w || i == wide.size()-1)
                {
                    RItemLine &line = documentBox->itemLines.back();
                    int lineHeightSpacing = ((activeStyle.font_size*activeStyle.line_height)-activeStyle.font_size);
                    if (line.lineH < activeStyle.font_size+lineHeightSpacing)
                    {
                        line.lineH = activeStyle.font_size+lineHeightSpacing;
                        //changeLineHeightTop(activeStyle.font_size+lineHeightSpacing, line);
                        //line.lineH = activeStyle.font_size;
                    }
                    if (documentBox->y + documentBox->h < line.lineY + line.lineH)
                        documentBox->h = (line.lineY + line.lineH)-documentBox->y;
                    if (line.baselineH < activeStyle.font_size+(lineHeightSpacing/2))
                    {
                        changeBaselineHeight(activeStyle.font_size+(lineHeightSpacing/2), line);
                        line.baselineH = activeStyle.font_size+(lineHeightSpacing/2);
                        //line.lineH = activeStyle.font_size;
                    }
                    RItem item;
                    item.type = RITEM_TEXT;
                    item.text.text = std::wstring((&wide[0])+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = line.lineX + line.lineW;
                    item.position.y = (line.lineY + line.baselineH + yScroll);
                    item.position.w = cX-(line.lineX + line.lineW);
                    item.position.h = line.lineH;
                    item.text.textSize = activeStyle.font_size;
                    item.text.bold = activeStyle.bold;
                    item.text.isLink = activeStyle.isLink;

                    item.text.color = activeStyle.color;
                    item.text.background_color = activeStyle.background_color;

                    itempos.x = line.lineX + line.lineW;
                    itempos.y = (line.lineY + line.baselineH + yScroll)-line.lineH;
                    itempos.w = cX-(line.lineX + line.lineW);
                    itempos.h = line.lineH;

                    RenderItems.push_back(item);
                    line.lineW += cX-(line.lineX + line.lineW);

                    line.items.push_back(&RenderItems.back());

                    if (cX > documentBox->w)
                    {
                        RItemLine nline;
                        nline.lineH = 0;
                        nline.baselineH = 0;
                        nline.lineW = 0;
                        nline.lineX = documentBox->x;
                        nline.lineY = line.lineY + line.lineH;
                        documentBox->itemLines.push_back(nline);
                        cX = nline.lineX;

                        if (documentBox->y + documentBox->h < nline.lineY + nline.lineH)
                            documentBox->h = (nline.lineY + nline.lineH)-documentBox->y;
                    }

                    charactersPreWritten = i+1;

                    if (mouseX > item.position.x && mouseY < item.position.y && mouseX < item.position.x + item.position.w && mouseY > item.position.y - item.position.h)
                    {
                        if (activeStyle.isLink && pressed)
                        {
                            std::cout << "Clicked link: " << _nextPageTemp << std::endl;
                            nextpage = _nextPageTemp;
                            switchPage = true;
                        }
                        //std::cout << activeStyle.background_color.r << std::endl;
                        std::string selectorsDebug;
                        for (unsigned int i = 0; i < activeStyle.cssdbg.matchingSelectorStrings.size(); i++)
                        {
                            selectorsDebug += activeStyle.cssdbg.matchingSelectorStrings[i] + "\n";
                        }
                        Debugger::setTextBoxText("NodeInfoBox2", selectorsDebug);
                    }
                }
            }
        }
    }
    return itempos;
}

void HTMLRenderer::renderRenderList(freetypeeasy::freetypeInst *inst, std::vector<RItem> items)
{
    for (unsigned int i = 0; i < items.size(); i++)
    {
        if (items[i].type == RITEM_TEXT  && items[i].position.y < framebufferHeight+items[i].position.h && items[i].position.y > 0)
        {
            fte::makeBold(inst, items[i].text.bold);
            fte::setFontSize(inst, items[i].text.textSize);

            //fte::setTextColor(inst, (255-items[i].text.color.r)/255.0f, (255-items[i].text.color.g)/255.0f, (255-items[i].text.color.b)/255.0f);
            fte::setTextColor(inst, (items[i].text.color.r)/255.0f, (items[i].text.color.g)/255.0f, (items[i].text.color.b)/255.0f);

            if (items[i].text.background_color.a != 0)
            {
                for (int y = items[i].position.y+(items[i].text.textSize/3); y >= (items[i].position.y - items[i].position.h)+(items[i].text.textSize/3); y--)
                {
                    for (int x = items[i].position.x; x < items[i].position.x + items[i].position.w; x++)
                    {
                        if (x < 0 || y < 0 || x > framebufferWidth-1 || y > framebufferHeight-1)
                        {
                            continue;
                        }
                        else
                        {
                            //framebuffer[(y*framebufferWidth*3)+((x)*3)] =   items[i].text.background_color.r;
                            //framebuffer[(y*framebufferWidth*3)+((x)*3)+1] = items[i].text.background_color.g;
                            //framebuffer[(y*framebufferWidth*3)+((x)*3)+2] = items[i].text.background_color.b;
                            framebuffer[(y*framebufferWidth*3)+((x)*3)+0] = ((framebuffer[(y*framebufferWidth*3)+((x)*3)+0]/255.f) * (255-items[i].text.background_color.a)) + ((items[i].text.background_color.r/255.f) * items[i].text.background_color.a);
                            framebuffer[(y*framebufferWidth*3)+((x)*3)+1] = ((framebuffer[(y*framebufferWidth*3)+((x)*3)+1]/255.f) * (255-items[i].text.background_color.a)) + ((items[i].text.background_color.g/255.f) * items[i].text.background_color.a);
                            framebuffer[(y*framebufferWidth*3)+((x)*3)+2] = ((framebuffer[(y*framebufferWidth*3)+((x)*3)+2]/255.f) * (255-items[i].text.background_color.a)) + ((items[i].text.background_color.b/255.f) * items[i].text.background_color.a);
                        }
                    }
                }
            }

            int x = 0;
            if (items[i].position.y < framebufferHeight)// Make sure we are not trying to write a character off screen
            {
                for (unsigned int j = 0; j < items[i].text.text.size(); j++)
                {
                    auto inf = fte::drawCharacter(inst, items[i].text.text[j], items[i].position.x + x, items[i].position.y, framebuffer, framebufferWidth, framebufferHeight, false);// Draw character, saving info about it
                    x += inf.advanceX/64;// Advance x position for next character
                }
            }
        }
        else if (items[i].type == RITEM_IMAGE && items[i].position.y < framebufferHeight && items[i].position.y > -items[i].position.h)
        {
            if (items[i].img.isValid)
            {
                for (int y = 0; y < items[i].img.h; y++)
                {
                    for (int x = 0; x < items[i].img.w; x++)
                    {
                        int xp = x+items[i].position.x;
                        int yp = y+items[i].position.y;
                        if (xp < 0 || yp < 0 || xp > framebufferWidth-1 || yp > framebufferHeight-1)
                        {
                            continue;
                        }
                        if (items[i].img.comp == 3)
                        {
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+0] = items[i].img.imageData[(y*items[i].img.w*3)+((x)*3)];
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = items[i].img.imageData[(y*items[i].img.w*3)+((x)*3)+1];
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = items[i].img.imageData[(y*items[i].img.w*3)+((x)*3)+2];
                        }
                        if (items[i].img.comp == 4)
                        {
                            /*if (items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4] != 0)
                            {
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)] =   items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)];
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+1];
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+2];
                            }*/
                            // Lol wtf is this blending my guy
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+0] = ((framebuffer[(yp*framebufferWidth*3)+((xp)*3)+0]/255.f) * (255-items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+3])) + ((items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+0]/255.f) * items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+3]);
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = ((framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1]/255.f) * (255-items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+3])) + ((items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+1]/255.f) * items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+3]);
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = ((framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2]/255.f) * (255-items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+3])) + ((items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+2]/255.f) * items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+3]);
                        }
                        /*if (x == 0 || y == 0 || x == items[i].img.w-1 || y == items[i].img.h-1)
                        {
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)] = 255;
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = 128;
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = 128;
                        }*/
                    }
                }
            }
            else
            {
                for (int y = items[i].position.y; y < items[i].position.y+items[i].position.h; y++)
                {
                    for (int x = items[i].position.x; x < items[i].position.x+items[i].position.w; x++)
                    {
                        int xp = x;
                        int yp = y;
                        if (xp < 0 || yp < 0 || xp > framebufferWidth-1 || yp > framebufferHeight-1)
                        {
                            continue;
                        }
                        framebuffer[(y*framebufferWidth*3)+((x)*3)] = 128;
                        framebuffer[(y*framebufferWidth*3)+((x)*3)+1] = 128;
                        framebuffer[(y*framebufferWidth*3)+((x)*3)+2] = 128;
                    }
                }
            }
        }
        else if (items[i].type == RITEM_COLORED_SQUARE && items[i].position.y < framebufferHeight+items[i].position.h && items[i].position.y+items[i].position.h > 0)
        {
            for (int y = items[i].position.y; y < items[i].position.y+items[i].position.h; y++)
            {
                for (int x = items[i].position.x; x < items[i].position.x+items[i].position.w; x++)
                {
                    int xp = x;
                    int yp = y;
                    if (xp < 0 || yp < 0 || xp > framebufferWidth-1 || yp > framebufferHeight-1)
                    {
                        continue;
                    }
                    framebuffer[(y*framebufferWidth*3)+((x)*3)+0] = ((framebuffer[(y*framebufferWidth*3)+((x)*3)+0]/255.f) * (255-items[i].squareColor.a)) + ((items[i].squareColor.r/255.f) * items[i].squareColor.a);
                    framebuffer[(y*framebufferWidth*3)+((x)*3)+1] = ((framebuffer[(y*framebufferWidth*3)+((x)*3)+1]/255.f) * (255-items[i].squareColor.a)) + ((items[i].squareColor.g/255.f) * items[i].squareColor.a);
                    framebuffer[(y*framebufferWidth*3)+((x)*3)+2] = ((framebuffer[(y*framebufferWidth*3)+((x)*3)+2]/255.f) * (255-items[i].squareColor.a)) + ((items[i].squareColor.b/255.f) * items[i].squareColor.a);
                }
            }
        }
    }
}

void HTMLRenderer::changeBaselineHeight(int newHeight, RItemLine &line)
{
    for (unsigned int i = 0; i < line.items.size(); i++)
    {
        line.items[i]->position.y += (newHeight - line.baselineH);
    }
    line.baselineH = newHeight;
}

void HTMLRenderer::changeLineHeightBottom(int newHeight, RItemLine &line)
{
    //line.lineHBottom = newHeight;
}
