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
        css_color color_shade;
        uint8_t color_type = css_computed_color(
                    style->styles[CSS_PSEUDO_ELEMENT_NONE],
                    &color_shade);
        if (color_type == CSS_COLOR_INHERIT)
            item.style.color_type = RENDERDOM_INHERIT;
        else
        {
            item.style.color_type = RENDERDOM_VALUE;
            item.style.color.a = (color_shade >> 24) & 0xff;
            item.style.color.r = (color_shade >> 16) & 0xff;
            item.style.color.g = (color_shade >> 8) & 0xff;
            item.style.color.b = (color_shade) & 0xff;
        }

        // background-color
        css_color bgcolor_shade;
        uint8_t bgcolor_type = css_computed_background_color(
                    style->styles[CSS_PSEUDO_ELEMENT_NONE],
                    &bgcolor_shade);
        if (bgcolor_type == CSS_COLOR_INHERIT)
            item.style.background_color_type = RENDERDOM_INHERIT;
        else
        {
            item.style.background_color_type = RENDERDOM_VALUE;
            item.style.background_color.a = (bgcolor_shade >> 24) & 0xff;
            item.style.background_color.r = (bgcolor_shade >> 16) & 0xff;
            item.style.background_color.g = (bgcolor_shade >> 8) & 0xff;
            item.style.background_color.b = (bgcolor_shade) & 0xff;
        }

        // font-size
        css_fixed font_size;
        css_unit font_size_unit;
        int fret = css_computed_font_size(style->styles[CSS_PSEUDO_ELEMENT_NONE], &font_size, &font_size_unit);
        if (fret == CSS_FONT_SIZE_DIMENSION)
        {
            // translate font size from whatever unit it is to px
            item.style.font_size_type = RENDERDOM_VALUE;
            if (font_size_unit == CSS_UNIT_PX)
                item.style.font_size = FIXTOINT(font_size);
            else if (font_size_unit == CSS_UNIT_EM)
                item.style.font_size = prev.font_size * FIXTOFLT(font_size);
            else if (font_size_unit == CSS_UNIT_PT)
                item.style.font_size = (FIXTOFLT(font_size)*72.0)/96.0;// Convert to in, then to px
            else if (font_size_unit == CSS_UNIT_CM)
                item.style.font_size = (FIXTOFLT(font_size)/2.54)/96.0;// convert to in, then to px
            else if (font_size_unit == CSS_UNIT_IN)
                item.style.font_size = FIXTOFLT(font_size)/96.0;// convert to in, then to px
            else if (font_size_unit == CSS_UNIT_PCT)
                item.style.font_size = (FIXTOFLT(font_size)/100) * FIXTOFLT(font_size);
        }
        else if (fret == CSS_FONT_SIZE_INHERIT)
        {
            item.style.font_size_type = RENDERDOM_INHERIT;
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
