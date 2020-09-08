#ifndef X11WINDOW_H
#define X11WINDOW_H
#include <string>
#include <X11/Xlib.h>

class X11Window
{
public:
    X11Window();

    XEvent e;
    Display * display;
    Visual* visual;
    Window window;
    XImage* ximage;

    int width;
    int height;

    int mousex;
    int mousey;
    int mousePressed;

    int scrollPos;

    Window inwin;      /* root window the pointer is in */
    Window inchildwin; /* child win the pointer is in */

    unsigned char *displayBuffer;
    void createWindow(std::string title, int width, int height, int framebufferWidth, int framebufferHeight);
    void drawString(std::string str, int x, int y);
    void processWindowEvents();
    void closeWindow();
};

#endif // X11WINDOW_H
