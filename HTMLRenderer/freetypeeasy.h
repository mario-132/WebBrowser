#ifndef FREETYPEEASY_H
#define FREETYPEEASY_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <fstream>

#define fte freetypeeasy

namespace freetypeeasy{

    struct freetypeInst
    {
        FT_Library library;
        FT_Face face;
        FT_Face faceB;
        bool bold;

        float r, g, b;
    };

    struct glyphInfo
    {
        int width;
        int height;
        int bearingX;
        int bearingY;
        int advanceX;
        int advanceY;
    };

    freetypeInst* initFreetype(std::string fontF, std::string fontFBold, int fontHeight);
    glyphInfo getCharacterBounds(freetypeInst* inst, char character);
    glyphInfo drawCharacter(freetypeInst* inst, char character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight);
    glyphInfo drawCharacter(freetypeInst* inst, char character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight, bool preloaded);
    void drawString(freetypeInst* inst, char* str, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight);
    void setFontSize(freetypeInst* inst, int size);
    void makeBold(freetypeInst* inst, bool bold);
    void setTextColor(freetypeInst* inst, float r, float g, float b);
}
#endif // FREETYPEEASY_H
