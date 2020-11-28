#include "freetypeeasy.h"
#include <iostream>
#include <deque>
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
        FT_Select_Charmap(inst->face , ft_encoding_unicode);
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
        FT_Select_Charmap(inst->faceB , ft_encoding_unicode);
        inst->bold = false;
        inst->r = 1;
        inst->g = 1;
        inst->b = 1;
        inst->fontsize = fontHeight;
        return inst;
    }

    glyphInfo getCharacterBounds(freetypeInst* inst, int character)
    {
        if (inst->glyphCache.find(inst->bold) != inst->glyphCache.end() &&
            inst->glyphCache[inst->bold].find(inst->fontsize) != inst->glyphCache[inst->bold].end() &&
            inst->glyphCache[inst->bold][inst->fontsize].find(character) != inst->glyphCache[inst->bold][inst->fontsize].end())
        {
            return inst->glyphCache[inst->bold][inst->fontsize][character].info;
        }

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
        glInf.btop = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_top;
        glInf.bleft = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_left;

        return glInf;
    }

    glyphInfo drawCharacter(freetypeInst* inst, int character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight, bool preloaded)
    {
        if (inst->glyphCache.find(inst->bold) != inst->glyphCache.end() &&
            inst->glyphCache[inst->bold].find(inst->fontsize) != inst->glyphCache[inst->bold].end() &&
            inst->glyphCache[inst->bold][inst->fontsize].find(character) != inst->glyphCache[inst->bold][inst->fontsize].end())
        {
            unsigned char* data = inst->glyphCache[inst->bold][inst->fontsize][character].glyphData;
            int width = inst->glyphCache[inst->bold][inst->fontsize][character].info.width;
            int height = inst->glyphCache[inst->bold][inst->fontsize][character].info.height;

            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    if (data[(y*width)+x] > 0)
                    {
                        int xp = x + xPos + inst->glyphCache[inst->bold][inst->fontsize][character].info.bleft;
                        int yp = y + (yPos - inst->glyphCache[inst->bold][inst->fontsize][character].info.btop);
                        if (xp < 0 || yp < 0 || xp >= bufferWidth || yp >= bufferHeight)
                        {
                            continue;
                        }
                        framebuffer[(yp*bufferWidth*3)+(xp*3)+0] = (inst->r * data[(y*width)+x]) + (framebuffer[(yp*bufferWidth*3)+(xp*3)+0] * (((float)(255-data[(y*width)+x]))/255));
                        framebuffer[(yp*bufferWidth*3)+(xp*3)+1] = (inst->g * data[(y*width)+x]) + (framebuffer[(yp*bufferWidth*3)+(xp*3)+1] * (((float)(255-data[(y*width)+x]))/255));
                        framebuffer[(yp*bufferWidth*3)+(xp*3)+2] = (inst->b * data[(y*width)+x]) + (framebuffer[(yp*bufferWidth*3)+(xp*3)+2] * (((float)(255-data[(y*width)+x]))/255));

                        //framebuffer[(yp*bufferWidth*3)+(xp*3)+0] = 255-(data[(y*width)+x]*inst->r);
                        //framebuffer[(yp*bufferWidth*3)+(xp*3)+1] = 255-(data[(y*width)+x]*inst->g);
                        //framebuffer[(yp*bufferWidth*3)+(xp*3)+2] = 255-(data[(y*width)+x]*inst->b);
                    }
                }
            }

            return inst->glyphCache[inst->bold][inst->fontsize][character].info;
        }
        else
        {
            if (!preloaded)
            {
                int glyph_index = FT_Get_Char_Index( (inst->bold?inst->faceB:inst->face), character );
                FT_Load_Glyph(
                          (inst->bold?inst->faceB:inst->face),          /* handle to face object */
                          glyph_index,   /* glyph index           */
                          0 );  /* load flags, see below */
            }


            int retCode = FT_Render_Glyph((inst->bold?inst->faceB:inst->face)->glyph,   /* glyph slot  */
                        FT_RENDER_MODE_NORMAL ); /* render mode */
            if (retCode != 0)
            {
                std::cerr << "Failed to render glyph! Error code: 0x" << std::hex << retCode << std::dec << " glyph font size: " << inst->fontsize << std::endl;
            }


            if ((inst->bold?inst->faceB:inst->face)->glyph->format == FT_GLYPH_FORMAT_BITMAP)
            {
                for (unsigned int x = 0; x < (inst->bold?inst->faceB:inst->face)->glyph->bitmap.width; x++)
                {
                    for (unsigned int y = 0; y < (inst->bold?inst->faceB:inst->face)->glyph->bitmap.rows; y++)
                    {
                        if ((inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x])
                        {
                            int xp = x + xPos + (inst->bold?inst->faceB:inst->face)->glyph->bitmap_left;
                            int yp = y + (yPos - (inst->bold?inst->faceB:inst->face)->glyph->bitmap_top);
                            if (xp < 0 || yp < 0 || xp >= bufferWidth || yp >= bufferHeight)
                            {
                                continue;
                            }
                            framebuffer[(yp*bufferWidth*3)+(xp*3)+0] = 255-((inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x]*inst->r);
                            framebuffer[(yp*bufferWidth*3)+(xp*3)+1] = 255-((inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x]*inst->g);
                            framebuffer[(yp*bufferWidth*3)+(xp*3)+2] = 255-((inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer[(y*(inst->bold?inst->faceB:inst->face)->glyph->bitmap.width)+x]*inst->b);
                        }
                    }
                }
            }
            else
            {
                std::cerr << "glyph is not a bitmap!" << std::endl;
                glyphInfo glInf;
                glInf.width = 0;
                glInf.height = 0;

                inst->glyphCache[inst->bold][inst->fontsize][character].info = glInf;
                return glInf;
            }
            glyphInfo glInf;
            glInf.width = (inst->bold?inst->faceB:inst->face)->glyph->bitmap.width;
            glInf.height = (inst->bold?inst->faceB:inst->face)->glyph->bitmap.rows;
            glInf.bearingX = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_left;
            glInf.bearingY = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_top;
            glInf.advanceX = (inst->bold?inst->faceB:inst->face)->glyph->advance.x;
            glInf.advanceY = (inst->bold?inst->faceB:inst->face)->glyph->advance.y;
            glInf.btop = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_top;
            glInf.bleft = (inst->bold?inst->faceB:inst->face)->glyph->bitmap_left;

            inst->glyphCache[inst->bold][inst->fontsize][character].info = glInf;
            inst->glyphCache[inst->bold][inst->fontsize][character].glyphData = (unsigned char*)malloc(glInf.width * glInf.height);
            memcpy(inst->glyphCache[inst->bold][inst->fontsize][character].glyphData, (inst->bold?inst->faceB:inst->face)->glyph->bitmap.buffer, glInf.width * glInf.height);

            return glInf;
        }
    }

    glyphInfo drawCharacter(freetypeInst* inst, char character, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight)
    {
        return drawCharacter(inst, character, xPos, yPos, framebuffer, bufferWidth, bufferHeight, false);
    }

    void drawString(freetypeInst* inst, char* str, int xPos, int yPos, unsigned char* framebuffer, int bufferWidth, int bufferHeight)
    {
        int xOffset = xPos;
        for (unsigned int i = 0; i < strlen(str); i++)
        {
            xOffset += drawCharacter(inst, str[i], xOffset, yPos, framebuffer, bufferWidth, bufferHeight).advanceX/64;
        }
    }

    void setFontSize(freetypeInst* inst, int size)
    {
        if (size < 1)
            return;
        inst->fontsize = size;
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
