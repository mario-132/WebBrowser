#include "renderdom.h"
#include "webservice.h"
#include <memory.h>

RenderDOM::RenderDOM()
{
#ifdef USEMAGICK
    Magick::InitializeMagick(0);
#endif
}

void RenderDOM::parseGumboTree(GumboNode *node)
{
}
