#include "htmlrenderer.h"

HTMLRenderer::HTMLRenderer()
{

}

void HTMLRenderer::assembleRenderList(std::vector<RItem> *items, RDocumentBox *activeDocBox, RenderDOMItem &item)
{
    if (item.type == RENDERDOM_ELEMENT)
    {
        for (int i = 0; i < item.children.size(); i++)
        {
            assembleRenderList(items, activeDocBox, item.children[i]);
        }
    }
    else if (item.type == RENDERDOM_TEXT)
    {
        if (item.style.display == CSS_DISPLAY_BLOCK || item.style.display == CSS_DISPLAY_INLINE)
            std::cout << item.text;
    }
    else
    {
        // Unknown
        std::cerr << "Unknown RenderDOMItem type!" << std::endl;
    }
}
