#include "renderdom.h"
#include "webservice.h"
#include "stb_image.h"

RenderDOM::RenderDOM()
{

}

std::string gumboTagToString(GumboTag tag)
{
    std::string name;

    if (tag == GUMBO_TAG_P)
    {
        name = "p";
    }
    else if (tag == GUMBO_TAG_IMG)
    {
        name = "img";
    }
    else if (tag == GUMBO_TAG_H1)
    {
        name = "h1";
    }
    else if (tag == GUMBO_TAG_H2)
    {
        name = "h2";
    }
    else if (tag == GUMBO_TAG_H3)
    {
        name = "h3";
    }
    else if (tag == GUMBO_TAG_H4)
    {
        name = "h4";
    }
    else if (tag == GUMBO_TAG_H5)
    {
        name = "h5";
    }
    else if (tag == GUMBO_TAG_H6)
    {
        name = "h6";
    }
    else if (tag == GUMBO_TAG_BODY)
    {
        name = "body";
    }
    else if (tag == GUMBO_TAG_HTML)
    {
        name = "html";
    }
    else if (tag == GUMBO_TAG_B)
    {
        name = "b";
    }
    else if (tag == GUMBO_TAG_BR)
    {
        name = "br";
    }
    else if (tag == GUMBO_TAG_DIV)
    {
        name = "div";
    }
    else if (tag == GUMBO_TAG_SPAN)
    {
        name = "span";
    }
    else if (tag == GUMBO_TAG_A)
    {
        name = "a";
    }
    else if (tag == GUMBO_TAG_HEAD)
    {
        name = "head";
    }
    else if (tag == GUMBO_TAG_STYLE)
    {
        name = "style";
    }
    else if (tag == GUMBO_TAG_CENTER)
    {
        name = "center";
    }
    else if (tag == GUMBO_TAG_BIG)
    {
        name = "big";
    }
    else if (tag == GUMBO_TAG_STRONG)
    {
        name = "strong";
    }
    else if (tag == GUMBO_TAG_SCRIPT)
    {
        name = "script";
    }
    else if (tag == GUMBO_TAG_FORM)
    {
        name = "form";
    }
    else if (tag == GUMBO_TAG_BUTTON)
    {
        name = "button";
    }
    else if (tag == GUMBO_TAG_SECTION)
    {
        name = "section";
    }
    else if (tag == GUMBO_TAG_LINK)
    {
        name = "link";
    }

    return name;
}

RenderDOMItem RenderDOM::parseGumboTree(GumboNode *node, RenderDOMStyle style, std::string baseURL, std::vector<css::CSSSelectorBlock> &css)
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
        if (style.display != "none")
        {
            style.display = "inline";
        }
        //applyStyle(item, style);

        for (int i = 0; i < css.size(); i++)
        {
            for (int j = 0; j < css[i].selectors.size(); j++)
            {
                if (css[i].selectors[j].additionals.back().name == gumboTagToString(node->v.element.tag))
                {
                    bool match = true;
                    int selectorOffset = domCallStack.size()-1;
                    for (int k = css[i].selectors[j].additionals.size()-2; k >= 0; k--)// We skip the last element because it is checked in the if already
                    {
                        if (css[i].selectors[j].additionals[k].selectorOp == css::CSS_DIRECT_CHILD_OF)
                        {
                            match = false;
                        }
                        if (css[i].selectors[j].additionals[k].selectorOp == css::CSS_INSIDE_OF)
                        {
                            bool found = false;
                            for (; selectorOffset >= 0; selectorOffset--)
                            {
                                if (domCallStack[selectorOffset] == css[i].selectors[j].additionals[k].name)
                                {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found)
                            {
                                match = false;
                            }
                        }
                        if (css[i].selectors[j].additionals[k].selectorOp == css::CSS_ADJECENT_TO)
                        {
                            match = false;
                        }
                        if (css[i].selectors[j].additionals[k].selectorOp == css::CSS_DIRECTLY_ADJECENT_TO)
                        {
                            match = false;
                        }
                    }
                    if (match)
                    {
                        for (int k = 0; k < css[i].items.size(); k++)
                        {
                            if (css[i].items[k].attribute.attributeAsString == "color")
                            {
                                if (css[i].items[k].value.type == css::CSS_TYPE_COLOR)
                                {
                                    style.color.r = css[i].items[k].value.color.r;
                                    style.color.g = css[i].items[k].value.color.g;
                                    style.color.b = css[i].items[k].value.color.b;
                                }
                            }
                            if (css[i].items[k].attribute.attributeAsString == "background-color")
                            {
                                if (css[i].items[k].value.type == css::CSS_TYPE_COLOR)
                                {
                                    style.background_color.r = css[i].items[k].value.color.r;
                                    style.background_color.g = css[i].items[k].value.color.g;
                                    style.background_color.b = css[i].items[k].value.color.b;
                                }
                            }
                            if (css[i].items[k].attribute.attributeAsString == "font-size")
                            {
                                if (css[i].items[k].value.type == css::CSS_TYPE_PX)
                                {
                                    style.font_size = css[i].items[k].value.numberValue;
                                }
                                if (css[i].items[k].value.type == css::CSS_TYPE_EM)///Todo: @mario-132 Make sure this is actually correct
                                {
                                    style.font_size = style.font_size*css[i].items[k].value.numberValue;
                                }
                            }
                            if (css[i].items[k].attribute.attributeAsString == "display")
                            {
                                if (css[i].items[k].value.valueAsString == "block")
                                {
                                    style.display = "block";
                                }
                                if (css[i].items[k].value.valueAsString == "inline")
                                {
                                    style.display = "inline";
                                }
                                if (css[i].items[k].value.valueAsString == "inline-block")
                                {
                                    style.display = "inline";
                                }
                                if (css[i].items[k].value.valueAsString == "none")
                                {
                                    style.display = "none";
                                }
                            }
                            if (css[i].items[k].attribute.attributeAsString == "font-weight")
                            {
                                if (css[i].items[k].value.type == css::CSS_TYPE_NUMBER)
                                {
                                    style.bold = (css[i].items[k].value.numberValue > 550);
                                }
                                if (css[i].items[k].value.valueAsString == "bold")
                                {
                                    style.bold = true;
                                }
                                if (css[i].items[k].value.valueAsString == "bolder")
                                {
                                    style.bold = true;
                                }
                                if (css[i].items[k].value.valueAsString == "normal")
                                {
                                    style.bold = false;
                                }
                                if (css[i].items[k].value.valueAsString == "lighter")
                                {
                                    style.bold = false;
                                }
                            }
                            if (css[i].items[k].attribute.attributeAsString == "text-align")
                            {
                                style.text_align = css[i].items[k].value.valueAsString;
                            }
                            if (css[i].items[k].attribute.attributeAsString == "--wb-islink")
                            {
                                style.isLink = (css[i].items[k].value.valueAsString == "link");
                            }
                            if (css[i].items[k].attribute.attributeAsString == "line-height")
                            {
                                if (css[i].items[k].value.type == css::CSS_TYPE_NUMBER)
                                {
                                    style.line_height = css[i].items[k].value.numberValue;
                                }
                            }
                        }
                    }
                }
            }
        }

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

            std::string newImgSrc;
            bool findingSlash = true;
            for (int i = 0; i < imgSource.size(); i++)
            {
                if (findingSlash && imgSource[i] == '/')
                {

                }
                else
                {
                    newImgSrc.push_back(imgSource[i]);
                    findingSlash = false;
                }
            }
            imgSource = newImgSrc;

            if (imgSource.find("http") != imgSource.npos || imgSource.find(".com") != imgSource.npos)
            {

                newsrc = imgSource;
                if (imgSource.find("http") == imgSource.npos)
                {
                    newsrc = "http://" + imgSource;
                }
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
                std::cout << "Downloading: " << newsrc << std::endl;
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
                item.element.image.downloaded = false;
                item.element.image.decoded = false;
            }
            if (w == 0 || h == 0)
            {
                w = 100;
                h = 100;
            }
        }

        domCallStack.push_back(gumboTagToString(node->v.element.tag));
        for (int i = 0; i < node->v.element.children.length; i++)
        {
            item.children.push_back(parseGumboTree((GumboNode*)node->v.element.children.data[i], style, baseURL, css));
        }
        domCallStack.pop_back();
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
        style.display = "block";
    }
    if (item.element.tag == GUMBO_TAG_HEAD)
    {
        style.display = "none";
    }
    if (item.element.tag == GUMBO_TAG_SCRIPT)
    {
        style.display = "none";
    }
    if (item.element.tag == GUMBO_TAG_STYLE)
    {
        style.display = "none";
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
