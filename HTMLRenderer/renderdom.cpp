#include "renderdom.h"
#include "webservice.h"
#include "stb_image.h"

RenderDOM::RenderDOM()
{

}

RenderDOMItem RenderDOM::parseGumboTree(GumboNode *node, RenderDOMStyle style, std::string baseURL)
{
    RenderDOMItem item;
    item.type = RENDERDOM_NONE;

    if (node->type == GUMBO_NODE_ELEMENT)
    {
        item.type = RENDERDOM_ELEMENT;
        item.element.tag = node->v.element.tag;
        for (int i = 0; i < node->v.element.attributes.length; i++)
        {
            RenderDOMAttribute attr;
            attr.name = ((GumboAttribute*)node->v.element.attributes.data[i])->name;
            attr.value = ((GumboAttribute*)node->v.element.attributes.data[i])->value;
            item.element.attributes.push_back(attr);
        }

        style.display = "inline";
        applyStyle(item, style);
        item.element.style = style;

        if (item.element.tag == GUMBO_TAG_IMG)
        {
            int w = 0;
            int h = 0;
            std::string imgSource;
            for (int i = 0; i < item.element.attributes.size(); i++)
            {
                if (std::string("src") == item.element.attributes[i].name)
                {
                    imgSource = item.element.attributes[i].value;
                }
                if (std::string("width") == item.element.attributes[i].name)
                {
                    w = std::stoi(item.element.attributes[i].value);
                }
                if (std::string("height") == item.element.attributes[i].name)
                {
                    h = std::stoi(item.element.attributes[i].value);
                }
            }
            std::string newsrc;
            if (imgSource.find("http") != imgSource.npos || imgSource.find(".com") != imgSource.npos)
            {
                newsrc = imgSource;
            }
            else
            {
                if (imgSource.size() > 1)
                {
                    newsrc = baseURL + imgSource;
                }
            }

            int imgW, imgH, comp;
            std::string out;
            if (newsrc.size() > 1)
            {
                out = WebService::htmlFileDownloader(newsrc);
            }
            unsigned char* data = stbi_load_from_memory((unsigned char*)&out[0], out.size(), &imgW, &imgH, &comp, 0);
            if (data != 0)
            {
                item.element.image.downloaded = true;
                item.element.image.decoded = true;

                item.element.image.imgH = imgH;
                item.element.image.imgW = imgW;
                item.element.image.comp = comp;
                item.element.image.imageData = data;
            }
            else
            {
                std::cout << "Failed to load: " << newsrc << std::endl;
            }
            if (w == 0 || h == 0)
            {
                w = 100;
                h = 100;
            }
        }

        for (int i = 0; i < node->v.element.children.length; i++)
        {
            item.children.push_back(parseGumboTree((GumboNode*)node->v.element.children.data[i], style, baseURL));
        }
    }
    else if (node->type == GUMBO_NODE_TEXT)
    {
        item.type = RENDERDOM_TEXT;
        item.text = node->v.text.text;
    }

    return item;
}

void RenderDOM::applyStyle(RenderDOMItem &item, RenderDOMStyle &style)
{
    // Hide certain elements so the scripts and such don't appear as text.
    if (item.element.tag == GUMBO_TAG_BODY)
    {
        style.visible = true;
    }
    if (item.element.tag == GUMBO_TAG_HEAD)
    {
        style.visible = false;
    }
    if (item.element.tag == GUMBO_TAG_SCRIPT)
    {
        style.visible = false;
    }
    if (item.element.tag == GUMBO_TAG_STYLE)
    {
        style.visible = false;
    }

    // Default font sizes
    if (item.element.tag == GUMBO_TAG_H1)
        style.font_size = 16*2;
    if (item.element.tag == GUMBO_TAG_H2)
        style.font_size = 16*1.5;
    if (item.element.tag == GUMBO_TAG_H3)
        style.font_size = 16*1.17;
    if (item.element.tag == GUMBO_TAG_H4)
        style.font_size = 16;
    if (item.element.tag == GUMBO_TAG_H5)
        style.font_size = 16*0.83;
    if (item.element.tag == GUMBO_TAG_H6)
        style.font_size = 16*0.67;
    if (item.element.tag == GUMBO_TAG_P)
        style.font_size = 16;

    if ((item.element.tag == GUMBO_TAG_H1 ||
         item.element.tag == GUMBO_TAG_H2 ||
         item.element.tag == GUMBO_TAG_H3 ||
         item.element.tag == GUMBO_TAG_H4 ||
         item.element.tag == GUMBO_TAG_H5 ||
         item.element.tag == GUMBO_TAG_H6 ||
         item.element.tag == GUMBO_TAG_B  ||
         item.element.tag == GUMBO_TAG_STRONG))
    {
        style.bold = true;
    }

    if (item.element.tag == GUMBO_TAG_A)
        style.isLink = true;

    // Give it block so it causes a newline.
    if ((item.element.tag == GUMBO_TAG_H1 ||
         item.element.tag == GUMBO_TAG_H2 ||
         item.element.tag == GUMBO_TAG_H3 ||
         item.element.tag == GUMBO_TAG_H4 ||
         item.element.tag == GUMBO_TAG_H5 ||
         item.element.tag == GUMBO_TAG_H6 ||
         item.element.tag == GUMBO_TAG_P  ||
         item.element.tag == GUMBO_TAG_DIV||
         item.element.tag == GUMBO_TAG_LI ||
         item.element.tag == GUMBO_TAG_BR))
    {
        style.display = "block";
    }
}
