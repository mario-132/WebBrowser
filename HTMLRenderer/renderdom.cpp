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

        css_select_results *style;
        css->selectNode(node, &style);

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
            item.style.font_size = unitToPx(font_size, font_size_unit, prev.font_size);
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
                item.style.height = unitToPx(height, height_unit, prev.height);
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
                item.style.width = unitToPx(width, width_unit, prev.width);
            }
            else if (width_type == CSS_WIDTH_AUTO)
                item.style.width_type = RENDERDOM_AUTO;
        }

        for (int i = 0; i < node->v.element.children.length; i++)
        {
            item.children.push_back(parseGumboTree(((GumboNode**)node->v.element.children.data)[i], css, item.style));
        }
    }
    else if (node->type == GUMBO_NODE_TEXT)
    {
        item.type = RENDERDOM_TEXT;
    }

    return item;
}

int RenderDOM::unitToPx(css_fixed fixed, css_unit unit, int prev)
{
    int pxRes = 0;
    if (unit == CSS_UNIT_PX)
        pxRes = FIXTOINT(fixed);
    else if (unit == CSS_UNIT_EM)
        pxRes = prev * FIXTOFLT(fixed);
    else if (unit == CSS_UNIT_PT)
        pxRes = (FIXTOFLT(fixed)*72.0)/96.0;// Convert to in, then to px
    else if (unit == CSS_UNIT_CM)
        pxRes = (FIXTOFLT(fixed)/2.54)/96.0;// convert to in, then to px
    else if (unit == CSS_UNIT_IN)
        pxRes = FIXTOFLT(fixed)/96.0;// convert to in, then to px
    else if (unit == CSS_UNIT_PCT)
        pxRes = (FIXTOFLT(fixed)/100) * FIXTOFLT(fixed);
    else
    {
        std::cerr << "Unable to translate unit!" << std::endl;
        pxRes = -1;
    }
    return pxRes;
}
