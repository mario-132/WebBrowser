#include "htmlrenderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "webservice.h"
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>

HTMLRenderer::HTMLRenderer()
{
    switchPage = false;
}

/*void findAndReplaceAll(std::string& data, const std::string& match, const std::string& replace)
{
   // Get the first occurrence
   size_t pos = data.find(match);
   // Repeat till end is reached
   while( pos != std::string::npos)
    {
        data.replace(pos, match.size(), replace);
       // Get the next occurrence from the current position
        pos = data.find(match, pos+replace.size());
    }
}*/

/*void HTMLRenderer::assembleRenderList(RenderDOMItem &root, freetypeeasy::freetypeInst *inst, RDocumentBox *documentBox, RenderDOMStyle style)
{

    if (root.type == RENDERDOM_ELEMENT)
    {
        style = root.element.style;
        if (style.display == "block")
        {
            if (documentBox->textStartX != documentBox->x)
            {
                documentBox->textStartX = documentBox->x;
                documentBox->textStartY += style.font_size*style.line_height;
            }
        }

        if (root.element.tag == GUMBO_TAG_IMG)
        {
            RItem item;
            item.img.isValid = false;

            int w = 10;
            int h = 10;

            for (int i = 0; i < root.element.attributes.size(); i++)
            {
                if (std::string("width") == root.element.attributes[i].name)
                {
                    w = std::stoi(root.element.attributes[i].value);
                }
                if (std::string("height") == root.element.attributes[i].name)
                {
                    h = std::stoi(root.element.attributes[i].value);
                }
            }

            if (root.element.image.decoded)
            {
                w = root.element.image.imgW;
                h = root.element.image.imgH;
                item.img.isValid = true;
                item.img.w = w;
                item.img.h = h;
                item.img.comp = root.element.image.comp;
                item.img.imageData = root.element.image.imageData;
            }

            item.type = RITEM_IMAGE;
            item.position.x = documentBox->textStartX;
            item.position.y = documentBox->textStartY;
            item.position.w = w;
            item.position.h = h;
            documentBox->textStartY += h;
            documentBox->textStartX += w;
            RenderItems.push_back(item);
            if (documentBox->itemLines.size() != 0)
            {
                //if (documentBox->itemLines.back().lineX + documentBox->itemLines.back().lineW + w < documentBox->x + documentBox->w)
                {

                    if (documentBox->itemLines.back().lineH < h)
                    {
                        for (int j = 0; j < documentBox->itemLines.back().items.size(); j++)
                        {
                            documentBox->itemLines.back().items[j]->position.y += (h - documentBox->itemLines.back().lineH);
                            //std::cout << "moved down by: " << (h - documentBox->itemLines.back().lineH) << std::endl;
                        }
                        documentBox->itemLines.back().lineH = h;
                    }
                    documentBox->itemLines.back().items.push_back(&RenderItems[RenderItems.size() - 1]);
                }
            }
            else
            {
                RItemLine line;
                line.items.push_back(&RenderItems[RenderItems.size() - 1]);
                line.lineH = h;
                line.lineW = w;
                line.lineX = item.position.x;
                line.lineY = item.position.y;
                documentBox->itemLines.push_back(line);
            }
        }

        // Parse the child elements.
        for (int i = 0; i < root.children.size(); i++)
        {
            assembleRenderList(root.children[i], inst, documentBox, style);
        }

        if (style.display == "block")
        {
            if (documentBox->textStartX != documentBox->x)
            {
                documentBox->textStartX = documentBox->x;
                documentBox->textStartY += style.font_size*style.line_height;
            }
        }
    }
    else if (root.type == RENDERDOM_TEXT)
    {
        if (style.visible)// Only draw it if it's visible at all.
        {
            fte::setFontSize(inst, style.font_size);
            int tX = documentBox->textStartX;
            int tY = documentBox->textStartY;

            int temptX = tX;
            int temptY = tY;

            int charactersPreWritten = 0;
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wide = converter.from_bytes(root.text);
            for (int i = 0; i < wide.size(); i++)
            {
                fte::makeBold(inst, style.bold);
                fte::glyphInfo inf = fte::getCharacterBounds(inst, wide[i]);
                temptX += inf.advanceX/64;
                if (temptX > documentBox->w || i == wide.size()-1)
                {
                    RItem item;
                    item.type = RITEM_TEXT;
                    item.text.text = std::wstring((&wide[0])+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = tX;
                    item.position.y = tY+(style.font_size*0.85)+((style.font_size*(style.line_height-1))/2);// font size is scaled by 0.85 to better center the text
                    item.text.textSize = style.font_size;

                    item.text.bold = style.bold;
                    item.text.isLink = style.isLink;

                    RenderItems.push_back(item);

                    if (documentBox->itemLines.size() != 0)
                    {
                        //if ( temptX < documentBox->x + documentBox->w)
                        {

                            if (documentBox->itemLines.back().lineH < style.font_size)
                            {
                                for (int j = 0; j < documentBox->itemLines.back().items.size(); j++)
                                {
                                    documentBox->itemLines.back().items[j]->position.y += (style.font_size - documentBox->itemLines.back().lineH);
                                }
                                documentBox->itemLines.back().lineH = style.font_size;
                            }
                            documentBox->itemLines.back().items.push_back(&RenderItems[RenderItems.size() - 1]);
                        }
                    }
                    else
                    {
                        RItemLine line;
                        line.items.push_back(&RenderItems[RenderItems.size() - 1]);
                        line.lineH = style.font_size;
                        line.lineW = temptX;
                        line.lineX = item.position.x;
                        line.lineY = item.position.y;
                        documentBox->itemLines.push_back(line);
                    }

                    if (temptX > documentBox->w)
                    {
                        tX = documentBox->x;
                        tY += style.font_size*style.line_height;
                    }
                    else
                    {
                        tX = temptX;
                    }

                    temptX = tX;
                    temptY = tY;

                    charactersPreWritten = i+1;
                }
            }

            documentBox->textStartX = tX;
            documentBox->textStartY = tY;
        }
    }
    //return RenderItems;
}*/

void HTMLRenderer::assembleRenderListV2(RenderDOMItem &root, freetypeeasy::freetypeInst *inst, RDocumentBox *documentBox, RenderDOMStyle activeStyle)
{
    if (documentBox->itemLines.size() == 0)
    {
        RItemLine line;
        line.lineHTop = 0;
        line.lineHBottom = 0;
        line.lineW = 0;
        line.lineX = documentBox->x;
        line.lineY = documentBox->y;
        documentBox->itemLines.push_back(line);
    }

    if (root.type == RENDERDOM_ELEMENT)
    {
        activeStyle = root.element.style;

        if (activeStyle.display == "block")
        {
            RItemLine nline;
            nline.lineHTop = 0;
            nline.lineHBottom = 0;
            nline.lineW = 0;
            nline.lineX = documentBox->x;
            nline.lineY = documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineHTop + documentBox->itemLines.back().lineHBottom;
            documentBox->itemLines.push_back(nline);
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
            for (int i = 0; i < root.element.attributes.size(); i++)
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
            if (line.lineHTop < imgH)
            {
                changeLineHeightTop(imgH, line);
                //line.lineH = imgH;
            }

            item.position.x = line.lineX + line.lineW;
            item.position.y = ((line.lineY + line.lineHTop)-imgH) + yScroll;

            line.lineW += imgW;
            RenderItems.push_back(item);
        }

        // Parse the child elements.
        for (int i = 0; i < root.children.size(); i++)
        {
            assembleRenderListV2(root.children[i], inst, documentBox, activeStyle);
        }

        if (activeStyle.display == "block")
        {
            RItemLine nline;
            nline.lineHTop = 0;
            nline.lineHBottom = 0;
            nline.lineW = 0;
            nline.lineX = documentBox->x;
            nline.lineY = documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineHTop + documentBox->itemLines.back().lineHBottom;
            documentBox->itemLines.push_back(nline);
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
            for (int i = 0; i < wide.size(); i++)
            {
                fte::makeBold(inst, activeStyle.bold);
                fte::glyphInfo inf = fte::getCharacterBounds(inst, wide[i]);
                cX += inf.advanceX/64;
                if (cX > documentBox->w || i == wide.size()-1)
                {
                    RItemLine &line = documentBox->itemLines.back();
                    int lineHeightSpacing = ((activeStyle.font_size*activeStyle.line_height)-activeStyle.font_size)/2;
                    if (line.lineHTop < activeStyle.font_size+lineHeightSpacing)
                    {
                        changeLineHeightTop(activeStyle.font_size+lineHeightSpacing, line);
                        //line.lineH = activeStyle.font_size;
                    }
                    if (line.lineHBottom < lineHeightSpacing || line.items.size() == 0)
                    {
                        changeLineHeightBottom(lineHeightSpacing, line);
                        //line.lineH = activeStyle.font_size;
                    }
                    RItem item;
                    item.type = RITEM_TEXT;
                    item.text.text = std::wstring((&wide[0])+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = line.lineX + line.lineW;
                    item.position.y = line.lineY + line.lineHTop + yScroll;
                    item.position.w = cX-(line.lineX + line.lineW);
                    item.position.h = line.lineHTop + line.lineHBottom;
                    item.text.textSize = activeStyle.font_size;
                    item.text.bold = activeStyle.bold;
                    item.text.isLink = activeStyle.isLink;

                    item.text.color.r = activeStyle.color.r;
                    item.text.color.g = activeStyle.color.g;
                    item.text.color.b = activeStyle.color.b;

                    item.text.background_color.r = activeStyle.background_color.r;
                    item.text.background_color.g = activeStyle.background_color.g;
                    item.text.background_color.b = activeStyle.background_color.b;

                    RenderItems.push_back(item);
                    line.lineW += cX-(line.lineX + line.lineW);

                    line.items.push_back(&RenderItems.back());

                    if (cX > documentBox->w)
                    {
                        RItemLine nline;
                        nline.lineHTop = 0;
                        nline.lineHBottom = 0;
                        nline.lineW = 0;
                        nline.lineX = documentBox->x;
                        nline.lineY = line.lineY + line.lineHTop + line.lineHBottom;
                        documentBox->itemLines.push_back(nline);
                        cX = nline.lineX;
                    }

                    charactersPreWritten = i+1;

                    if (mouseX > item.position.x && mouseY < item.position.y && mouseX < item.position.x + item.position.w && mouseY > item.position.y - item.position.h)
                    {
                        if (activeStyle.isLink && pressed)
                        {
                            std::cout << "Clicked link" << std::endl;
                        }
                        //std::cout << activeStyle.background_color.r << std::endl;
                        for (int i = 0; i < activeStyle.cssdbg.matchingSelectorStrings.size(); i++)
                        {
                            //std::cout << activeStyle.cssdbg.matchingSelectorStrings[i] << std::endl;
                            RItem item;
                            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                            std::wstring wide = converter.from_bytes(activeStyle.cssdbg.matchingSelectorStrings[i]);
                            item.text.text = wide;
                            item.position.x = mouseX+10;
                            item.position.y = mouseY+40+(i*20);
                            item.position.h = 16;
                            item.position.w = 200;
                            item.type = RITEM_TEXT;

                            item.text.color.r = 0;
                            item.text.color.g = 0;
                            item.text.color.b = 0;

                            item.text.background_color.r = 180;
                            item.text.background_color.g = 180;
                            item.text.background_color.b = 150;

                            item.text.textSize = 16;
                            item.text.bold = true;
                            item.text.isLink = false;

                            RenderItems.push_back(item);
                        }
                    }
                }
            }
        }
    }
}

void HTMLRenderer::renderRenderList(freetypeeasy::freetypeInst *inst, std::vector<RItem> items)
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].type == RITEM_TEXT  && items[i].position.y < framebufferHeight+items[i].position.h && items[i].position.y > 0)
        {
            fte::makeBold(inst, items[i].text.bold);
            fte::setFontSize(inst, items[i].text.textSize);

            //fte::setTextColor(inst, (255-items[i].text.color.r)/255.0f, (255-items[i].text.color.g)/255.0f, (255-items[i].text.color.b)/255.0f);
            fte::setTextColor(inst, (items[i].text.color.r)/255.0f, (items[i].text.color.g)/255.0f, (items[i].text.color.b)/255.0f);

            if ((items[i].text.background_color.r != 255 || items[i].text.background_color.g != 255 || items[i].text.background_color.b != 255))
            {
                for (int x = items[i].position.x; x < items[i].position.x + items[i].position.w; x++)
                {
                    for (int y = items[i].position.y; y >= items[i].position.y - items[i].position.h; y--)
                    {
                        if (x < 0 || y < 0 || x > framebufferWidth-1 || y > framebufferHeight-1)
                        {
                            continue;
                        }
                        else
                        {
                            framebuffer[(y*framebufferWidth*3)+((x)*3)] =   items[i].text.background_color.r;
                            framebuffer[(y*framebufferWidth*3)+((x)*3)+1] = items[i].text.background_color.g;
                            framebuffer[(y*framebufferWidth*3)+((x)*3)+2] = items[i].text.background_color.b;
                        }
                    }
                }
            }

            int x = 0;
            if (items[i].position.y < framebufferHeight)// Make sure we are not trying to write a character off screen
            {
                for (int j = 0; j < items[i].text.text.size(); j++)
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
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)] =   items[i].img.imageData[(y*items[i].img.w*3)+((x)*3)];
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
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+0] = ((framebuffer[(yp*framebufferWidth*3)+((xp)*3)+0]/255.f) * (255-items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4])) + ((items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+0]/255.f) * items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4]);
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = ((framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1]/255.f) * (255-items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4])) + ((items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+1]/255.f) * items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4]);
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = ((framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2]/255.f) * (255-items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4])) + ((items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+2]/255.f) * items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4]);
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
    }
}

void HTMLRenderer::changeLineHeightTop(int newHeight, RItemLine &line)
{
    for (int i = 0; i < line.items.size(); i++)
    {
        line.items[i]->position.y += (newHeight - line.lineHTop);
    }
    line.lineHTop = newHeight;
}

void HTMLRenderer::changeLineHeightBottom(int newHeight, RItemLine &line)
{
    line.lineHBottom = newHeight;
}
