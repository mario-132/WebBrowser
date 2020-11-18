#include "renderdom.h"
#include "webservice.h"
#include <memory.h>

RenderDOM::RenderDOM()
{
#ifdef USEMAGICK
    Magick::InitializeMagick(0);
#endif
}

std::string RenderDOM::gumboTagToString(GumboTag tag)
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
    else if (tag == GUMBO_TAG_UL)
    {
        name = "ul";
    }
    else if (tag == GUMBO_TAG_LI)
    {
        name = "li";
    }
    else if (tag == GUMBO_TAG_OL)
    {
        name = "ol";
    }
    else if (tag == GUMBO_TAG_TABLE)
    {
        name = "table";
    }
    else if (tag == GUMBO_TAG_TBODY)
    {
        name = "tbody";
    }
    else if (tag == GUMBO_TAG_TR)
    {
        name = "tr";
    }
    else if (tag == GUMBO_TAG_TD)
    {
        name = "td";
    }
    else if (tag == GUMBO_TAG_NOSCRIPT)
    {
        name = "noscript";
    }
    else if (tag == GUMBO_TAG_NOFRAMES)
    {
        name = "noframes";
    }
    else if (tag == GUMBO_TAG_HEADER)
    {
        name = "header";
    }
    else if (tag == GUMBO_TAG_NAV)
    {
        name = "nav";
    }
    else if (tag == GUMBO_TAG_ARTICLE)
    {
        name = "article";
    }
    else if (tag == GUMBO_TAG_ASIDE)
    {
        name = "aside";
    }
    else if (tag == GUMBO_TAG_FOOTER)
    {
        name = "footer";
    }
    else if (tag == GUMBO_TAG_DETAILS)
    {
        name = "details";
    }
    else if (tag == GUMBO_TAG_SUMMARY)
    {
        name = "summary";
    }
    else if (tag == GUMBO_TAG_META)
    {
        name = "meta";
    }
    else if (tag == GUMBO_TAG_VIDEO)
    {
        name = "video";
    }
    else if (tag == GUMBO_TAG_AUDIO)
    {
        name = "audio";
    }
    else if (tag == GUMBO_TAG_HR)
    {
        name = "hr";
    }
    else if (tag == GUMBO_TAG_TEXTAREA)
    {
        name = "textarea";
    }
    else if (tag == GUMBO_TAG_PRE)
    {
        name = "pre";
    }

    return name;
}

void RenderDOM::parseGumboTree(GumboNode *node)
{
}
