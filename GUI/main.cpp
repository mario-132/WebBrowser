#include <iostream>
#include "x11window.h"
#include "freetypeeasy.h"
#include <vector>

class Button
{
public:
    Button(int x, int y, int w, int h, std::string text)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->text = text;
    }
    void render(fte::freetypeInst *a, unsigned char* fb, int fbw, int fbh)
    {
        int bgCol = 239;
        if (mouseOver)
        {
            bgCol = 230;
        }
        if (pressed)
        {
            bgCol = 245;
        }
        for (int yp = y; yp < y+h; yp++)
        {
            for (int xp = x; xp < x+w; xp++)
            {
                char col = bgCol;
                if (yp == y || xp == x || yp == y+h-1 || xp == x+w-1)
                {
                    col = 74;
                }
                if (xp >= 0 && yp >= 0 && xp < fbw && yp < fbh)
                {
                    fb[(yp*fbw*3)+(xp*3)+0] = col;
                    fb[(yp*fbw*3)+(xp*3)+1] = col;
                    fb[(yp*fbw*3)+(xp*3)+2] = col;
                }
            }
        }
        int Sw = fte::getStringWidth(a, (char*)text.c_str());
        fte::drawString(a, (char*)text.c_str(), x+((w-Sw)/2), y+h-((h-16)/2)-(16/5), fb, fbw, fbh);
    }
    void collision(int mouseX, int mouseY, bool mousePressed)
    {
        if (mouseX > x && mouseY > y  && mouseX < x+w && mouseY < y+h)
        {
            mouseOver = true;
            if (!miss)
            {
                pressed = mousePressed;
            }
            if (!mousePressed)
            {
                miss = false;
            }
        }
        else
        {
            mouseOver = false;
            pressed = false;
            miss = true;
        }
    }
    bool mouseOver;
    bool pressed;
    bool miss;
    int x;
    int y;
    int w;
    int h;
    std::string text;
};

int main()
{
    unsigned char* framebuffer = (unsigned char*)malloc(1920 * 1080 * 3);
    memset(framebuffer, 255, 1920*1080*3);

    X11Window window;
    window.createWindow("GUI", 1920, 1080, 1920, 1080);

    fte::freetypeInst *a = fte::initFreetype("/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 16);
    std::vector<Button> buttons;
    for (int i = 0; i < 100; i++)
    {
        Button submit(rand() % 1920, rand() % 1080, 40+(rand()%50), 15+(rand()%30), "submit");
        buttons.push_back(submit);
    }


    while(1)
    {
        memset(framebuffer, 255, 1920*1080*3);
        //fte::drawString(a, "hello", window.mousex, window.mousey, framebuffer, 1920, 1080);

        for (int i = 0; i < buttons.size(); i++)
        {
            buttons[i].collision(window.mousex, window.mousey, window.mousePressed);
            buttons[i].render(a, framebuffer, 1920, 1080);
        }


        for (int i = 0; i < 1920 * 1080; i++)
        {
            window.displayBuffer[(i*4)+0] = framebuffer[(i*3)+0];
            window.displayBuffer[(i*4)+1] = framebuffer[(i*3)+1];
            window.displayBuffer[(i*4)+2] = framebuffer[(i*3)+2];
        }
        window.processWindowEvents();
    }

    return 0;
}
