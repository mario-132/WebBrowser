#include <iostream>
#include "x11window.h"
#include "freetypeeasy.h"
#include <vector>

class UIItem
{
public:
    virtual void render(fte::freetypeInst *a, unsigned char* fb, int fbw, int fbh)
    {

    }
    virtual void collision(int mouseX, int mouseY, bool mousePressed)
    {

    }
    int x;
    int y;
    int w;
    int h;
};

class Button : public UIItem
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
    std::string text;
};

class ScrollBar : public UIItem
{
public:
    ScrollBar(int x, int y, int w, int h, int scrollerSize, int pos)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->scrollerSize = scrollerSize;
        this->pos = pos;
    }
    void render(fte::freetypeInst *a, unsigned char* fb, int fbw, int fbh)
    {
        for (int yp = y; yp < y+h; yp++)
        {
            for (int xp = x; xp < x+w; xp++)
            {
                int scrollerHeight = scrollerSize;
                int scrollBarY = pos+y+w;

                unsigned char col = 220;
                if (yp < y+w)
                    col = 255;
                if (yp > y+h-w)
                    col = 255;
                if (yp == y || xp == x || yp == y+h-1 || xp == x+w-1 || yp == y+w || yp == y+h-w)
                    col = 150;
                if (yp > scrollBarY && yp < scrollBarY+scrollerHeight)
                    col = 30;
                if (xp >= 0 && yp >= 0 && xp < fbw && yp < fbh)
                {
                    fb[(yp*fbw*3)+(xp*3)+0] = col;
                    fb[(yp*fbw*3)+(xp*3)+1] = col;
                    fb[(yp*fbw*3)+(xp*3)+2] = col;
                }
            }
        }
    }
    void collision(int mouseX, int mouseY, bool mousePressed)
    {
        int scrollerHeight = scrollerSize;
        int scrollBarY = pos+y+w;

        if (mouseX > x && mouseX < x+w)
        {
            if (mouseY > scrollBarY && mouseY < scrollBarY+scrollerHeight)
            {
                if (mousePressed)
                {
                    dragging = true;
                }
            }
        }

        if (!mousePressed)
        {
            dragging = false;
        }

        if (dragging)
        {
            int a = ((mouseY-(scrollerHeight/2))-y-w);
            pos = a;
            std::cout << normalizedPos << std::endl;
        }
        if (pos < 0)
            pos=0;
        if (pos > (h-w-w-scrollerHeight))
            pos = (h-w-w-scrollerHeight);
        normalizedPos = (((float)pos)/(h-w-w))+((normalizedPos*scrollerHeight)/(h-w-w));
    }
    int scrollerSize;
    int pos;
    float normalizedPos;
    bool dragging;
};

int main()
{
    unsigned char* framebuffer = (unsigned char*)malloc(1920 * 1080 * 3);
    memset(framebuffer, 255, 1920*1080*3);

    X11Window window;
    window.createWindow("GUI", 1920, 1080, 1920, 1080);

    fte::freetypeInst *a = fte::initFreetype("/usr/share/fonts/truetype/ubuntu/Ubuntu-L.ttf", "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 16);
    std::vector<UIItem*> buttons;
    for (int i = 0; i < 100; i++)
    {
        Button *submit = new Button(rand() % 1920, rand() % 1080, 40+(rand()%50), 15+(rand()%30), "submit");
        buttons.push_back(submit);
    }
    ScrollBar *scroll = new ScrollBar(800, 50, 10, 800, 30, 100);
    buttons.push_back(scroll);

    ScrollBar *scrollw = new ScrollBar(850, 50, 10, 800, 30, 100);
    buttons.push_back(scrollw);

    ScrollBar *scroll2 = new ScrollBar(900, 50, 10, 500, 100, 100);
    buttons.push_back(scroll2);

    while(1)
    {
        scroll2->w = scrollw->normalizedPos*500;
        scroll2->scrollerSize = scroll->normalizedPos*400;
        scroll2->x = window.width-scroll2->w;
        scroll2->y = 0;
        scroll2->h = window.height;
        memset(framebuffer, 255, 1920*1080*3);

        for (int i = 0; i < buttons.size(); i++)
        {
            buttons[i]->collision(window.mousex, window.mousey, window.mousePressed);
            buttons[i]->render(a, framebuffer, 1920, 1080);
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
