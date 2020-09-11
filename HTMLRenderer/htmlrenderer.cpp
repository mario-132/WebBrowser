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
        line.lineH = 0;
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
            nline.lineH = 0;
            nline.lineW = 0;
            nline.lineX = documentBox->x;
            nline.lineY = documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineH;
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
            if (line.lineH < imgH)
            {
                changeLineHeight(imgH, line);
                //line.lineH = imgH;
            }

            item.position.x = line.lineX + line.lineW;
            item.position.y = (line.lineY + line.lineH)-imgH;

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
            nline.lineH = 0;
            nline.lineW = 0;
            nline.lineX = documentBox->x;
            nline.lineY = documentBox->itemLines.back().lineY + documentBox->itemLines.back().lineH;
            documentBox->itemLines.push_back(nline);
        }
    }
    else if (root.type == RENDERDOM_TEXT)
    {
        if (activeStyle.visible)
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
                    if (line.lineH < activeStyle.font_size)
                    {
                        changeLineHeight(activeStyle.font_size, line);
                        //line.lineH = activeStyle.font_size;
                        /// Todo: update location of previous text on this line. //DONE
                    }
                    RItem item;
                    item.type = RITEM_TEXT;
                    item.text.text = std::wstring((&wide[0])+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = line.lineX + line.lineW;
                    item.position.y = line.lineY + line.lineH;
                    item.text.textSize = activeStyle.font_size;
                    item.text.bold = activeStyle.bold;
                    item.text.isLink = activeStyle.isLink;

                    RenderItems.push_back(item);
                    line.lineW += cX-(line.lineX + line.lineW);

                    line.items.push_back(&RenderItems.back());

                    if (cX > documentBox->w)
                    {
                        RItemLine nline;
                        nline.lineH = 0;
                        nline.lineW = 0;
                        nline.lineX = documentBox->x;
                        nline.lineY = line.lineY + line.lineH;
                        documentBox->itemLines.push_back(nline);
                        cX = nline.lineX;
                    }

                    charactersPreWritten = i+1;
                }
            }
        }
    }
}

void HTMLRenderer::renderRenderList(freetypeeasy::freetypeInst *inst, std::vector<RItem> items)
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].type == RITEM_TEXT)
        {
            fte::makeBold(inst, items[i].text.bold);
            fte::setFontSize(inst, items[i].text.textSize);

            if (items[i].text.isLink)
                fte::setTextColor(inst, (255-26)/255.0f, (255-13)/255.0f, (255-171)/255.0f);
            else
                fte::setTextColor(inst, 1, 1, 1);

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
        else if (items[i].type == RITEM_IMAGE)
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
                            if (items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4] > 50)
                            {
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)] =   items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)];
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+1];
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+2];
                            }
                        }
                        if (x == 0 || y == 0 || x == items[i].img.w-1 || y == items[i].img.h-1)
                        {
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)] = 255;
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = 128;
                            framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = 128;
                        }
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

void HTMLRenderer::changeLineHeight(int newHeight, RItemLine &line)
{
    for (int i = 0; i < line.items.size(); i++)
    {
        line.items[i]->position.y += (newHeight - line.lineH);
    }
    line.lineH = newHeight;
}
