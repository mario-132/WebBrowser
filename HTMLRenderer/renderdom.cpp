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

bool RenderDOM::checkSelectorMatch(std::string selector, const DOMStackItem &item)
{
    if (selector.size()>0)
    {
        if (selector[0] == '.')
        {
            selector.erase(0, 1);
            if (checkClassMatch(selector, item.unparsedClasses))
                return true;
        }
        else if (selector[0] == '#')
        {
            selector.erase(0, 1);
            if (checkIDMatch(selector, item.unparsedIDs))
                return true;
        }
        else
        {
            return selector == item.tag;
        }
    }
    return false;
}

std::vector<std::string> RenderDOM::parseSpaceSeparatedString(std::string str)
{
    std::vector<std::string> items;

    int pos = 0;
    int max = str.size();

    while(pos < max)
    {
        items.push_back("");
        while(str[pos] != ' ' && pos < max)
        {
            items.back() += str[pos];
            pos++;
        }
        while(str[pos] == ' ' && pos < max)
        {
            pos++;
        }
    }

    return items;
}

bool RenderDOM::checkClassMatch(std::string classname, std::string items)
{
    std::vector<std::string> classes = parseSpaceSeparatedString(items);
    for (int l = 0; l < classes.size(); l++)
    {
        if (classes[l] == classname)
        {
            return true;
        }
    }
    return false;
}

bool RenderDOM::checkIDMatch(std::string idname, std::string items)
{
    std::vector<std::string> ids = parseSpaceSeparatedString(items);
    for (int l = 0; l < ids.size(); l++)
    {
        if (ids[l] == idname)
        {
            return true;
        }
    }
    return false;
}

void RenderDOM::applyItemToStyle(css::CSSItem item, RenderDOMStyle &style)
{
    if (item.attribute.attributeAsString == "color")
    {
        if (item.value.type == css::CSS_TYPE_COLOR)
        {
            style.color.r = item.value.color.r;
            style.color.g = item.value.color.g;
            style.color.b = item.value.color.b;
        }
    }
    if (item.attribute.attributeAsString == "background-color")
    {
        if (item.value.type == css::CSS_TYPE_COLOR)
        {
            style.background_color.r = item.value.color.r;
            style.background_color.g = item.value.color.g;
            style.background_color.b = item.value.color.b;
        }
    }
    if (item.attribute.attributeAsString == "font-size")
    {
        if (item.value.type == css::CSS_TYPE_PX)
        {
            style.font_size = item.value.numberValue;
        }
        if (item.value.type == css::CSS_TYPE_EM)///Todo: @mario-132 Make sure this is actually correct
        {
            style.font_size = style.font_size*item.value.numberValue;
        }
    }
    if (item.attribute.attributeAsString == "display")
    {
        if (item.value.valueAsString == "block")
        {
            style.display = "block";
        }
        if (item.value.valueAsString == "flex")
        {
            style.display = "block";
        }
        if (item.value.valueAsString == "inline")
        {
            style.display = "inline";
        }
        if (item.value.valueAsString == "inline-block")
        {
            style.display = "inline";
        }
        if (item.value.valueAsString == "none")
        {
            style.display = "none";
        }
    }
    if (item.attribute.attributeAsString == "font-weight")
    {
        if (item.value.type == css::CSS_TYPE_NUMBER)
        {
            style.bold = (item.value.numberValue > 550);
        }
        if (item.value.valueAsString == "bold")
        {
            style.bold = true;
        }
        if (item.value.valueAsString == "bolder")
        {
            style.bold = true;
        }
        if (item.value.valueAsString == "normal")
        {
            style.bold = false;
        }
        if (item.value.valueAsString == "lighter")
        {
            style.bold = false;
        }
    }
    if (item.attribute.attributeAsString == "text-align")
    {
        style.text_align = item.value.valueAsString;
    }
    if (item.attribute.attributeAsString == "--wb-islink")
    {
        style.isLink = (item.value.valueAsString == "link");
    }
    if (item.attribute.attributeAsString == "line-height")
    {
        if (item.value.type == css::CSS_TYPE_NUMBER)
        {
            style.line_height = item.value.numberValue;
        }
    }
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

        // Get the tag, class and id string and add them to stackItem for later use in the parsing of the css stylesheet nodes to see if they apply to this node.
        DOMStackItem stackitem;
        stackitem.tag = gumboTagToString(node->v.element.tag);
        for (int i = 0; i < item.element.attributes.size(); i++)
        {
            if (std::string("class") == item.element.attributes[i].name)
            {
                stackitem.unparsedClasses = item.element.attributes[i].value;
            }
            if (std::string("id") == item.element.attributes[i].name)
            {
                stackitem.unparsedIDs = item.element.attributes[i].value;
            }
        }

        // Display is not inherited so always reset it to inline, unless the display is none, in which case we want to keep the elements inside it hidden.
        if (style.display != "none")
        {
            style.display = "inline";
        }

        // Look if there are any selectors in the global stylesheets that apply to this node.
        for (int i = 0; i < css.size(); i++)
        {
            for (int j = 0; j < css[i].selectors.size(); j++)
            {
                if (checkSelectorMatch(css[i].selectors[j].additionals.back().name, stackitem))
                {
                    bool match = true;
                    int selectorOffset = domCallStack.size()-1;
                    if (selectorOffset < 0)
                    {
                        break;
                    }
                    // Check if the additional class or id specifiers also apply to this css selector block(e.g. make sure the .classa in div.classa{} is present in this node).
                    for (int k = 0; k < css[i].selectors[j].additionals.back().matchingClasses.size(); k++)
                    {
                        std::string sel = css[i].selectors[j].additionals.back().matchingClasses[k];
                        if (sel[0] == '.')
                        {
                            sel.erase(0, 1);
                            if (!checkClassMatch(sel, stackitem.unparsedClasses))
                                match = false;
                        }
                        /*if (sel[0] == '#')
                        {
                            sel.erase(0, 1);
                            if (!checkIDMatch(sel, stackitem.unparsedIDs))
                                match = false;
                        }*/
                    }
                    //css[i].selectors[j].additionals.back().matchingClasses

                    /*for (int i = 0; i < domCallStack.size(); i++)
                    {
                        std::cout << domCallStack[i] << ". ";
                    }
                    std::cout << css[i].selectors[j].additionals.size() << "::";
                    for (int k = css[i].selectors[j].additionals.size()-1; k >= 0; k--)
                    {
                        std::cout << css[i].selectors[j].additionals[k].name << css[i].selectors[j].additionals[k].selectorOp << ", ";
                    }
                    std::cout << std::endl;*/
                    for (int k = css[i].selectors[j].additionals.size()-2; k >= 0; k--)// We skip the last element because it is checked in the if already
                    {
                        //if (selectorOffset >= 0)
                        //    std::cout << gumboTagToString(node->v.element.tag) << "=" << domCallStack[selectorOffset] << ", " << css[i].selectors[j].additionals[k].name << std::endl;
                        if (css[i].selectors[j].additionals[k].selectorOp == css::CSS_DIRECT_CHILD_OF)
                        {
                            if (!checkSelectorMatch(css[i].selectors[j].additionals[k].name, domCallStack[selectorOffset]))
                            {
                                match = false;
                                break;
                            }
                            if (selectorOffset > 0)
                            {
                                selectorOffset--;
                            }
                        }
                        if (css[i].selectors[j].additionals[k].selectorOp == css::CSS_INSIDE_OF)
                        {
                            bool found = false;
                            for (; selectorOffset >= 0; selectorOffset--)
                            {
                                if (checkSelectorMatch(css[i].selectors[j].additionals[k].name, domCallStack[selectorOffset]))
                                {
                                    found = true;
                                    if (selectorOffset > 0)
                                    {
                                        selectorOffset--;
                                    }
                                    break;
                                }
                            }
                            if (!found)
                            {
                                match = false;
                            }
                        }
                        if (css[i].selectors[j].additionals[k].selectorOp == css::CSS_ADJECENT_TO)/// Todo: @mario-132 Implement these 2 selectors
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
                            applyItemToStyle(css[i].items[k], style);
                        }
                    }
                }
            }
        }

        // Parse all inline style sheets
        for (int i = 0; i < item.element.attributes.size(); i++)
        {
            if (std::string("style") == item.element.attributes[i].name)
            {
                std::string css = item.element.attributes[i].value;
                std::vector<css::CSSItem> inlineItems = css::parseInlineFromString(css);
                for (int k = 0; k < inlineItems.size(); k++)
                {
                    applyItemToStyle(inlineItems[k], style);
                }
            }
        }

        item.element.style = style;// Save computed style to the item.

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

        domCallStack.push_back(stackitem);
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
