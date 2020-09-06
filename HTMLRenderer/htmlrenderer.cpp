#include "htmlrenderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

HTMLRenderer::HTMLRenderer()
{

}

void HTMLRenderer::applyStyle(GumboNode *node, RStyle &style)
{
    // Hide certain elements so the scripts and such don't appear as text.
    if (node->v.element.tag == GUMBO_TAG_BODY)
    {
        style.visible = true;
    }
    if (node->v.element.tag == GUMBO_TAG_HEAD)
    {
        style.visible = false;
    }
    if (node->v.element.tag == GUMBO_TAG_SCRIPT)
    {
        style.visible = false;
    }
    if (node->v.element.tag == GUMBO_TAG_STYLE)
    {
        style.visible = false;
    }

    // Default font sizes
    if (node->v.element.tag == GUMBO_TAG_H1)
        style.font_size = 16*2;
    if (node->v.element.tag == GUMBO_TAG_H2)
        style.font_size = 16*1.5;
    if (node->v.element.tag == GUMBO_TAG_H3)
        style.font_size = 16*1.17;
    if (node->v.element.tag == GUMBO_TAG_H4)
        style.font_size = 16;
    if (node->v.element.tag == GUMBO_TAG_H5)
        style.font_size = 16*0.83;
    if (node->v.element.tag == GUMBO_TAG_H6)
        style.font_size = 16*0.67;
    if (node->v.element.tag == GUMBO_TAG_P)
        style.font_size = 16;

    if ((node->v.element.tag == GUMBO_TAG_H1 ||
         node->v.element.tag == GUMBO_TAG_H2 ||
         node->v.element.tag == GUMBO_TAG_H3 ||
         node->v.element.tag == GUMBO_TAG_H4 ||
         node->v.element.tag == GUMBO_TAG_H5 ||
         node->v.element.tag == GUMBO_TAG_H6 ||
         node->v.element.tag == GUMBO_TAG_B))
    {
        style.bold = true;
    }

    if (node->v.element.tag == GUMBO_TAG_A)
        style.isLink = true;

    // Give it block so it causes a newline.
    if ((node->v.element.tag == GUMBO_TAG_H1 ||
         node->v.element.tag == GUMBO_TAG_H2 ||
         node->v.element.tag == GUMBO_TAG_H3 ||
         node->v.element.tag == GUMBO_TAG_H4 ||
         node->v.element.tag == GUMBO_TAG_H5 ||
         node->v.element.tag == GUMBO_TAG_H6 ||
         node->v.element.tag == GUMBO_TAG_P  ||
         node->v.element.tag == GUMBO_TAG_DIV||
         node->v.element.tag == GUMBO_TAG_LI))
    {
        style.display = "block";
    }
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

std::vector<RItem> HTMLRenderer::assembleRenderList(GumboNode *node, freetypeeasy::freetypeInst *inst, RStyle style, RDocumentBox *documentBox)
{
    std::vector<RItem> RenderItems;
    if (node->type == GUMBO_NODE_ELEMENT)
    {

        style.display = "inline";
        applyStyle(node, style);

        if (style.display == "block")
        {
            if (documentBox->textStartX != documentBox->x)
            {
                documentBox->textStartX = documentBox->x;
                documentBox->textStartY += style.font_size*style.line_height;
            }
        }

        if (node->v.element.tag == GUMBO_TAG_IMG)
        {
            RItem item;
            item.img.isValid = false;
            int w = 0;
            int h = 0;
            std::string imgSource;
            for (int i = 0; i < node->v.element.attributes.length; i++)
            {
                if (std::string("src") == std::string(((GumboAttribute*)node->v.element.attributes.data[i])->name))
                {
                    imgSource = ((GumboAttribute*)node->v.element.attributes.data[i])->value;
                }
                if (std::string("width") == std::string(((GumboAttribute*)node->v.element.attributes.data[i])->name))
                {
                    w = std::stoi(((GumboAttribute*)node->v.element.attributes.data[i])->value);
                }
                if (std::string("height") == std::string(((GumboAttribute*)node->v.element.attributes.data[i])->name))
                {
                    h = std::stoi(((GumboAttribute*)node->v.element.attributes.data[i])->value);
                }
            }
            //findAndReplaceAll(imgSource, "%20", " ");
            int imgW, imgH, comp;
            unsigned char* data = stbi_load(("/home/tim/Documents/Development/WebBrowserData/HTML/" + imgSource).c_str(), &imgW, &imgH, &comp, 0);
            if (data != 0)
            {
                item.img.isValid = true;
                item.img.w = imgW;
                item.img.h = imgH;
                //if (w == 0 || h == 0)
                {
                    w = imgW;
                    h = imgH;
                }
                item.img.comp = comp;
                item.img.imageData = data;
            }
            if (w == 0 || h == 0)
            {
                w = 100;
                h = 100;
            }
            item.type = RITEM_IMAGE;
            item.position.x = documentBox->textStartX;
            item.position.y = documentBox->textStartY;
            item.position.w = w;
            item.position.h = h;
            documentBox->textStartY += h;
            documentBox->textStartX += w;
            RenderItems.push_back(item);
        }

        // Parse the child elements.
        for (int i = 0; i < node->v.element.children.length; i++)
        {
            std::vector<RItem> returnRenderList = assembleRenderList((GumboNode*)node->v.element.children.data[i], inst, style, documentBox);
            for (int j = 0; j < returnRenderList.size(); j++)
            {
                RenderItems.push_back(returnRenderList[j]);
            }
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
    else if (node->type == GUMBO_NODE_TEXT)
    {
        if (style.visible)// Only draw it if it's visible at all.
        {
            fte::setFontSize(inst, style.font_size);
            int tX = documentBox->textStartX;
            int tY = documentBox->textStartY;

            int temptX = tX;
            int temptY = tY;

            int charactersPreWritten = 0;
            for (int i = 0; i < strlen(node->v.text.text); i++)
            {
                fte::makeBold(inst, style.bold);
                fte::glyphInfo inf = fte::getCharacterBounds(inst, node->v.text.text[i]);
                temptX += inf.advanceX/64;
                if (temptX > documentBox->w || i == strlen(node->v.text.text)-1)
                {
                    RItem item;
                    item.type = RITEM_TEXT;
                    item.text.text = std::string(node->v.text.text+charactersPreWritten, (i+1)-charactersPreWritten);
                    item.position.x = tX;
                    item.position.y = tY+(style.font_size*0.85)+((style.font_size*(style.line_height-1))/2);// font is scaled by 0.85 to better center the text
                    item.text.textSize = style.font_size;

                    item.text.bold = style.bold;
                    item.text.isLink = style.isLink;

                    RenderItems.push_back(item);

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
    return RenderItems;
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
                for (int x = 0; x < items[i].img.w; x++)
                {
                    for (int y = 0; y < items[i].img.h; y++)
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
                            if (items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+4] > 10)
                            {
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)] =   items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)];
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)+1] = items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+1];
                                framebuffer[(yp*framebufferWidth*3)+((xp)*3)+2] = items[i].img.imageData[(y*items[i].img.w*4)+((x)*4)+2];
                            }
                        }
                    }
                }
            }
            else
            {
                for (int x = items[i].position.x; x < items[i].position.x+items[i].position.w; x++)
                {
                    for (int y = items[i].position.y; y < items[i].position.y+items[i].position.h; y++)
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
