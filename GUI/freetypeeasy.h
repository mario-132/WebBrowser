#ifndef FREETYPEEASY_H
#define FREETYPEEASY_H
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <string>
#include <fstream>
#include <map>

#define fte freetypeeasy

namespace freetypeeasy{

    struct glyphInfo
    {
        int width;
        int height;
        int bearingX;
        int bearingY;
        int advanceX;
        int advanceY;

        int bleft;
        int btop;
    };

    /// Store glyph pixels and glyph size
    struct cachedGlyph
    {
        unsigned char* glyphData;// Must be manually deleted
        glyphInfo info;
    };

    struct freetypeInst
    {
        FT_Library library;
        FT_Face face;
        FT_Face faceB;
        bool bold;
        int fontsize;

        float r, g, b;
        std::map<bool, std::map<int, std::map<int, cachedGlyph>>> glyphCache;
    };

    freetypeInst* initFreetype(std::string fontF, std::string fontFBold, int fontHeight);
    glyphInfo getCharacterBounds(freetypeInst* inst, int character);
    glyphInfo drawCharacter(freetypeInst* inst, int character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight);
    glyphInfo drawCharacter(freetypeInst* inst, int character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight, bool preloaded);
    void drawString(freetypeInst* inst, char* str, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight);
    int getStringWidth(freetypeInst *inst, char *str);
    void setFontSize(freetypeInst* inst, int size);
    void makeBold(freetypeInst* inst, bool bold);
    void setTextColor(freetypeInst* inst, float r, float g, float b);
}
#endif // FREETYPEEASY_H
