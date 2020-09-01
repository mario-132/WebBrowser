#include "freetypeeasy.h"
#include <iostream>
#include FT_SIZES_H

namespace freetypeeasy
{
    freetypeInst* initFreetype(std::string fontF, std::string fontFBold, int fontHeight)
    {
        freetypeInst* inst = new freetypeInst;
        /// Normal
        std::ifstream ff(fontF, std::ios::binary | std::ios::ate);
        std::streamsize size = ff.tellg();
        if (size == 0)
        {
            std::cout << "Failed to open font file: " << fontF << std::endl;
        }
        ff.seekg(0, std::ios::beg);

        unsigned char *fontbuffer = (unsigned char*)malloc(size);
        ff.read((char*)fontbuffer, size);

        /// Bold
        std::ifstream ffB(fontFBold, std::ios::binary | std::ios::ate);
        std::streamsize sizeB = ffB.tellg();
        if (sizeB == 0)
        {
            std::cout << "Failed to open font file: " << fontFBold << std::endl;
        }
        ffB.seekg(0, std::ios::beg);

        unsigned char *fontbufferB = (unsigned char*)malloc(sizeB);
        ffB.read((char*)fontbufferB, sizeB);

        /// Normal
        if (FT_Init_FreeType(&inst->library))
        {
            std::cout << "Failed to initialise freetype2" << std::endl;
        }
        if (FT_New_Memory_Face( inst->library,
                         fontbuffer,
                         size,
                         0,
                         &inst->face ))
        {
            std::cout << "[FreeType]Failed to decode font" << std::endl;
        }

        FT_Set_Pixel_Sizes(
                  inst->face,   /* handle to face object */
                  0,      /* pixel_width           */
                  fontHeight );   /* pixel_height          */
        ///Bold
        if (FT_New_Memory_Face( inst->library,
                         fontbufferB,
                         sizeB,
                         0,
                         &inst->faceB ))
        {
            std::cout << "[FreeType]Failed to decode font" << std::endl;
        }

        FT_Set_Pixel_Sizes(
                  inst->faceB,   /* handle to face object */
                  0,      /* pixel_width           */
                  fontHeight );   /* pixel_height          */
        inst->bold = false;
        inst->r = 1;
        inst->g = 1;
        inst->b = 1;
        return inst;
    }

    glyphInfo getCharacterBounds(freetypeInst* inst, char character)
    {
        int glyph_index = FT_Get_Char_Index( (inst->bold?inst->faceB:inst->face), character );

        FT_Load_Glyph(
                  (inst->bold?inst->faceB:inst->face),          /* handle to face object */
                  glyph_index,   /* glyph index           */
                  0 );  /* load flags, see below */

        glyphInfo glInf;
        glInf.width = (inst->bold?inst->faceB:inst->face)->glyph->bitmap.width;
        glInf.height = (inst->bold?inst->faceB:inst->face)->glyph->bitmap.rows;
        glInf.bearingX = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_left;
        glInf.bearingY = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_top;
        glInf.advanceX = (inst->bold?inst->faceB:inst->face)->glyph->advance.x;
        glInf.advanceY = (inst->bold?inst->faceB:inst->face)->glyph->advance.y;
        return glInf;
    }

    glyphInfo drawCharacter(freetypeInst* inst, char character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight, bool preloaded)
    {
        if (!preloaded)
        {
            int glyph_index = FT_Get_Char_Index( (inst->bold?inst->faceB:inst->face), character );

            FT_Load_Glyph(
                      (inst->bold?inst->faceB:inst->face),          /* handle to face object */
                      glyph_index,   /* glyph index           */
                      0 );  /* load flags, see below */
        }
        FT_Render_Glyph((inst->bold?inst->faceB:inst->face)->glyph,   /* glyph slot  */
                        FT_RENDER_MODE_NORMAL ); /* render mode */
        if ((inst->bold?inst->faceB:inst->face)->glyph->format == FT_GLYPH_FORMAT_BITMAP)
        {
            for (int x = 0; x < (inst->bold?inst->faceB:inst->face)->glyph->bitmap.width; x++)
            {
                for (int y = 0; y < (inst->bold?inst->faceB:inst->face)->glyph->bitmap.rows; y++)
                {
                    if ((inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x])
                    {
                        int xp = x + xPos + (inst->bold?inst->faceB:inst->face)->glyph->bitmap_left;
                        int yp = y + (yPos - (inst->bold?inst->faceB:inst->face)->glyph->bitmap_top);
                        if (xp < 0 || yp < 0 || xp > bufferWidth || yp > bufferHeight)
                        {
                            continue;
                        }
                        framebuffer[(yp*bufferWidth*3)+(xp*3)+0] = (255-(inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x]*inst->r);
                        framebuffer[(yp*bufferWidth*3)+(xp*3)+1] = (255-(inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x]*inst->g);
                        framebuffer[(yp*bufferWidth*3)+(xp*3)+2] = (255-(inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x]*inst->b);
                    }
                }
            }
        }
        else
        {
            std::cout << "glyph is not a bitmap!" << std::endl;
        }
        glyphInfo glInf;
        glInf.width = (inst->bold?inst->faceB:inst->face)->glyph->bitmap.width;
        glInf.height = (inst->bold?inst->faceB:inst->face)->glyph->bitmap.rows;
        glInf.bearingX = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_left;
        glInf.bearingY = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_top;
        glInf.advanceX = (inst->bold?inst->faceB:inst->face)->glyph->advance.x;
        glInf.advanceY = (inst->bold?inst->faceB:inst->face)->glyph->advance.y;
        return glInf;
    }

    glyphInfo drawCharacter(freetypeInst* inst, char character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight)
    {
        drawCharacter(inst, character, xPos, yPos, framebuffer, bufferWidth, bufferHeight, false);
    }

    void drawString(freetypeInst* inst, char* str, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight)
    {
        int xOffset = xPos;
        for (int i = 0; i < strlen(str); i++)
        {
            xOffset += drawCharacter(inst, str[i], xOffset, yPos, framebuffer, bufferWidth, bufferHeight).advanceX/64;
        }
    }

    void setFontSize(freetypeInst* inst, int size)
    {
        if (FT_Set_Pixel_Sizes( (inst->bold?inst->faceB:inst->face),
                              0,
                              size ) != 0)
        {
            std::cout << "Error setting font size" << std::endl;
        }
    }

    void makeBold(freetypeInst *inst, bool bold)
    {
        inst->bold = bold;
    }

    void setTextColor(freetypeInst *inst, float r, float g, float b)
    {
        inst->r = r;
        inst->g = g;
        inst->b = b;
    }

}
