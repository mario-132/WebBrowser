/* This contains all code that isn't necesery anymore but might come in handy
 * Don't put actual code in here without commenting it
int yOffs = 32;
void printinfo(fte::freetypeInst *inst, unsigned char* framebuffer, GumboNode* node, int i, int jj)
{
    for (int k = 0; k < i; k++)
    {
        std::cout << "  ";
    }
    std::cout << "Item " << i << "." << jj << ": " << "Amount of children: " << node->v.element.children.length << " node type: ";
    if (node->type == GUMBO_NODE_TEXT)
    {
        std::cout << "text" << std::endl;
    }
    else if (node->type == GUMBO_NODE_ELEMENT)
    {
        std::cout << "element" << std::endl;
    }
    else if (node->type == GUMBO_NODE_DOCUMENT)
    {
        std::cout << "document" << std::endl;
    }
    else if (node->type == GUMBO_NODE_CDATA)
    {
        std::cout << "cdata" << std::endl;
    }
    else if (node->type == GUMBO_NODE_COMMENT)
    {
        std::cout << "comment" << std::endl;
    }
    else if (node->type == GUMBO_NODE_TEMPLATE)
    {
        std::cout << "template" << std::endl;
    }
    else if (node->type == GUMBO_NODE_WHITESPACE)
    {
        std::cout << "whitespace" << std::endl;
    }
    else
    {
        std::cout << "unknown" << std::endl;
    }
    if (node->v.element.tag == GUMBO_TAG_H1 || node->v.element.tag == GUMBO_TAG_BODY || node->v.element.tag == GUMBO_TAG_BR || node->v.element.tag == GUMBO_TAG_P)
    {
        for (int k = 0; k < i; k++)
        {
            std::cout << "  ";
        }
        if (node->v.element.tag == GUMBO_TAG_H1)
        {
            std::cout << "This node is H1" << std::endl;
        }
        if (node->v.element.tag == GUMBO_TAG_BODY)
        {
            std::cout << "This node is BODY" << std::endl;
        }
        if (node->v.element.tag == GUMBO_TAG_BR)
        {
            std::cout << "This node is BR" << std::endl;
        }
        if (node->v.element.tag == GUMBO_TAG_P)
        {
            std::cout << "This node is P" << std::endl;
        }
    }

    if (node->type == GUMBO_NODE_TEXT || node->type == GUMBO_NODE_COMMENT)
    {
        for (int k = 0; k < i; k++)
        {
            std::cout << "  ";
        }
        std::cout << "Text Content: " << (node->v.text.text) << std::endl;
        fte::drawString(inst, (char*)node->v.text.text, 10, yOffs, framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
        yOffs += FONTLINE;
    }
    if (node->type == GUMBO_NODE_ELEMENT)
    {
        for (unsigned int j = 0; j < node->v.element.children.length; j++)
        {
            printinfo(inst, framebuffer, (GumboNode*)node->v.element.children.data[j], i+1, j);
        }
    }
}

int htmlTextCursorX = 0;
int htmlTextCursorY = 20;

void htmlDrawCharacter(fte::freetypeInst *inst, unsigned char* framebuffer, char c)
{
    fte::glyphInfo inf = fte::getCharacterBounds(inst, c);
    if (htmlTextCursorX + inf.width > FRAMEBUFFER_WIDTH)
    {
        htmlTextCursorX = 0;
        htmlTextCursorY += FONTLINE;
    }
    fte::drawCharacter(inst, c, htmlTextCursorX, htmlTextCursorY, framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, true);
    htmlTextCursorX += inf.advanceX/64;
}

void traverseDOM(fte::freetypeInst *inst, unsigned char* framebuffer, GumboNode* node, int i, int jj, bool last, GumboTag parentElem)
{
    if (node->type == GUMBO_NODE_ELEMENT)
    {
        if (node->v.element.tag == GUMBO_TAG_BODY)
        {
            std::cout << "body" << std::endl;
        }
        if (node->v.element.tag == GUMBO_TAG_DIV)
        {
            std::cout << "div" << std::endl;
            htmlTextCursorX = 0;
            htmlTextCursorY += FONTLINE;
        }
        if (node->v.element.tag == GUMBO_TAG_BR)
        {
            std::cout << "br" << std::endl;
            htmlTextCursorX = 0;
            htmlTextCursorY += FONTLINE;
        }
        if (node->v.element.tag != GUMBO_TAG_SCRIPT)
        {
            for (unsigned int j = 0; j < node->v.element.children.length; j++)
            {
                traverseDOM(inst, framebuffer, (GumboNode*)node->v.element.children.data[j], i, j, (j+1 == node->v.element.children.length), node->v.element.tag);
            }
        }
    }

    if (node->type == GUMBO_NODE_TEXT)
    {
        for (int i = 0; i < strlen(node->v.text.text); i++)
        {
            //htmlDrawCharacter(inst, framebuffer, node->v.text.text[i]);
        }

    }
    if ((parentElem == GUMBO_TAG_H1 ||
         parentElem == GUMBO_TAG_H2 ||
         parentElem == GUMBO_TAG_H3 ||
         parentElem == GUMBO_TAG_H4 ||
         parentElem == GUMBO_TAG_H5 ||
         parentElem == GUMBO_TAG_H6) && last)
    {
        htmlTextCursorX = 0;
        htmlTextCursorY += FONTLINE;
    }
}\

    RStyle docStyle;
    docStyle.display = "inline";
    docStyle.visible = false;
    docStyle.font_size = 16;
    docStyle.line_height = 1.2;
    docStyle.bold = false;
    docStyle.isLink = false;

    RTextInfoBox textInfoBox;
    textInfoBox.x = 0;
    textInfoBox.y = 0;//window.height/3;
    textInfoBox.w = 1920;
    textInfoBox.h = 1080;
    textInfoBox.textStartX = 0;
    textInfoBox.textStartY = 0;//window.height/3;
    docStyle.bold = false;

    renderRenderList(inst, assembleRenderList(output->root, inst, docStyle, &textInfoBox));

    //drawString(inst, "Are you still there?", 10, 32, framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

    stbi_write_png("out.png", FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 3, framebuffer, FRAMEBUFFER_WIDTH*3);
*/
