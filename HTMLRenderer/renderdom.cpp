#include "renderdom.h"
#include "webservice.h"
#include <memory.h>

RenderDOM::RenderDOM()
{
#ifdef USEMAGICK
    Magick::InitializeMagick(0);
#endif
}

RenderDOMItem RenderDOM::parseGumboTree(GumboNode *node, CSS *css, RenderDOMStyle prev)
{
    RenderDOMItem item;
    item.type = RENDERDOM_UNKNOWN;

    if (node->type == GUMBO_NODE_ELEMENT)
    {
        item.type = RENDERDOM_ELEMENT;

        item.tag = CSS::gumboTagToString(node->v.element.tag);

        bool hasInlineSheet = false;
        CSSStylesheet* inlinesheet;
        for (unsigned int i = 0; i < node->v.element.attributes.length; i++)
        {
            if (std::string(((GumboAttribute*)node->v.element.attributes.data[i])->name) == "style")
            {
                /*std::string cssData = item.tag;
                cssData += " { ";
                cssData += ((GumboAttribute*)node->v.element.attributes.data[i])->value;
                cssData += " }";*/
                inlinesheet = css->createStylesheet(0, ((GumboAttribute*)node->v.element.attributes.data[i])->value, true);
                //css->addToSelector(inlinesheet);
                hasInlineSheet = true;
                break;
            }
        }

        css_select_results *style;
        if (hasInlineSheet)
            css->selectNode(node, &style, inlinesheet);
        else
            css->selectNode(node, &style, 0);

        // display
        item.style.display = (css_display_e)css_computed_display(style->styles[CSS_PSEUDO_ELEMENT_NONE], false);

        // color
        {
            css_color color_shade;
            uint8_t color_type = css_computed_color(
                        style->styles[CSS_PSEUDO_ELEMENT_NONE],
                        &color_shade);
            if (color_type == CSS_COLOR_INHERIT)
            {
                item.style.color_type = prev.color_type;
                item.style.color = prev.color;
            }
            else
            {
                item.style.color_type = RENDERDOM_VALUE;
                item.style.color.a = (color_shade >> 24) & 0xff;
                item.style.color.r = (color_shade >> 16) & 0xff;
                item.style.color.g = (color_shade >> 8) & 0xff;
                item.style.color.b = (color_shade) & 0xff;
            }
            item.style._was_color = (css_color_e)color_type;
        }

        // background-color
        {
            css_color bgcolor_shade;
            uint8_t bgcolor_type = css_computed_background_color(
                        style->styles[CSS_PSEUDO_ELEMENT_NONE],
                        &bgcolor_shade);
            if (bgcolor_type == CSS_BACKGROUND_COLOR_INHERIT)
            {
                item.style.background_color_type = prev.background_color_type;
                item.style.background_color = prev.background_color;
            }
            else
            {
                item.style.background_color_type = RENDERDOM_VALUE;
                item.style.background_color.a = (bgcolor_shade >> 24) & 0xff;
                item.style.background_color.r = (bgcolor_shade >> 16) & 0xff;
                item.style.background_color.g = (bgcolor_shade >> 8) & 0xff;
                item.style.background_color.b = (bgcolor_shade) & 0xff;
            }
            item.style._was_bg_color = (css_background_color_e)bgcolor_type;
        }

        // font-size
        css_fixed font_size;
        css_unit font_size_unit;
        int fret = css_computed_font_size(style->styles[CSS_PSEUDO_ELEMENT_NONE], &font_size, &font_size_unit);
        if (fret == CSS_FONT_SIZE_DIMENSION)
        {
            // translate font size from whatever unit it is to px
            item.style.font_size_type = RENDERDOM_VALUE;
            item.style.font_size = unitToPx(font_size, font_size_unit, prev.font_size, prev.font_size);
        }
        else if (fret == CSS_FONT_SIZE_INHERIT)
        {
            item.style.font_size_type = prev.font_size_type;
            item.style.font_size = prev.font_size;
        }

        // height
        {
            css_fixed height;
            css_unit height_unit;
            int height_type = css_computed_height(style->styles[CSS_PSEUDO_ELEMENT_NONE], &height, &height_unit);
            if (height_type == CSS_HEIGHT_INHERIT)
            {
                item.style.height_type = prev.height_type;
                item.style.height = prev.height;
            }
            else if (height_type == CSS_HEIGHT_SET)
            {
                item.style.height_type = RENDERDOM_VALUE;
                item.style.height = unitToPx(height, height_unit, prev.height, prev.font_size);
            }
            else if (height_type == CSS_HEIGHT_AUTO)
                item.style.height_type = RENDERDOM_AUTO;
        }

        // width
        {
            css_fixed width;
            css_unit width_unit;
            int width_type = css_computed_width(style->styles[CSS_PSEUDO_ELEMENT_NONE], &width, &width_unit);
            if (width_type == CSS_WIDTH_INHERIT)
            {
                item.style.width_type = prev.width_type;
                item.style.width = prev.width;
            }
            else if (width_type == CSS_WIDTH_SET)
            {
                item.style.width_type = RENDERDOM_VALUE;
                item.style.width = unitToPx(width, width_unit, prev.width, prev.font_size);
            }
            else if (width_type == CSS_WIDTH_AUTO)
                item.style.width_type = RENDERDOM_AUTO;
        }

        // font-weight
        {
            int font_weight_ret = css_computed_font_weight(style->styles[CSS_PSEUDO_ELEMENT_NONE]);
            if (font_weight_ret == CSS_FONT_WEIGHT_INHERIT)
                item.style.font_weight = prev.font_weight;
            else if (font_weight_ret == CSS_FONT_WEIGHT_BOLD)
                item.style.font_weight = 700;
            else if (font_weight_ret == CSS_FONT_WEIGHT_NORMAL)
                item.style.font_weight = 300;
            else if (font_weight_ret == CSS_FONT_WEIGHT_BOLDER)
                item.style.font_weight = 900;
            else if (font_weight_ret == CSS_FONT_WEIGHT_LIGHTER)
                item.style.font_weight = 300;
            else
                item.style.font_weight = font_weight_ret;
        }

        for (unsigned int i = 0; i < node->v.element.attributes.length; i++)
        {
            item.attributes.push_back({((GumboAttribute*)node->v.element.attributes.data[i])->name,
                                       ((GumboAttribute*)node->v.element.attributes.data[i])->value});
        }

        for (int i = 0; i < node->v.element.children.length; i++)
        {
            item.children.push_back(parseGumboTree(((GumboNode**)node->v.element.children.data)[i], css, item.style));
        }
    }
    else if (node->type == GUMBO_NODE_TEXT)
    {
        item.style = prev;/// Todo: Maybe give text its own style class and always make this one the style of its parent element.
        item.type = RENDERDOM_TEXT;

        item.text = node->v.text.text;
    }

    return item;
}

int RenderDOM::unitToPx(css_fixed fixed, css_unit unit, int prev, int prevFnt)
{
    int pxRes = 0;
    if (unit == CSS_UNIT_PX)
        pxRes = FIXTOINT(fixed);
    else if (unit == CSS_UNIT_EM)
        pxRes = prevFnt * FIXTOFLT(fixed);
    else if (unit == CSS_UNIT_PT)
        pxRes = FIXTOFLT(fixed)*1.333333;
    else if (unit == CSS_UNIT_PC)
        pxRes = FIXTOFLT(fixed)*16;
    else if (unit == CSS_UNIT_CM)
        pxRes = FIXTOFLT(fixed)*37.795276;
    else if (unit == CSS_UNIT_MM)
        pxRes = FIXTOFLT(fixed)*3.7795276;
    else if (unit == CSS_UNIT_IN)
        pxRes = FIXTOFLT(fixed)*96;
    else if (unit == CSS_UNIT_PCT)
        pxRes = (FIXTOFLT(fixed)/100) * prev;
    else if (unit == CSS_UNIT_REM)
        pxRes = FIXTOFLT(fixed)*16;
    else
    {
        std::cerr << "Unable to translate unit!" << std::endl;
        pxRes = 1;
    }
    return pxRes;
}
