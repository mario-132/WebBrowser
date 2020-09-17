/*#include "x11window.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

X11Window::X11Window()
{

}

void X11Window::createWindow(std::string title, int width, int height)
{
    Display *d;
    Window w;
    XEvent e;
    const char *msg = "Hello, World!";
    int s;

    d = XOpenDisplay(NULL);
    if (d == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    s = DefaultScreen(d);
    w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1,
                           BlackPixel(d, s), WhitePixel(d, s));
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    XMapWindow(d, w);

    XStoreName( d, w, title.c_str() );
    XSetIconName( d, w, title.c_str() );

    while (1) {
        XNextEvent(d, &e);
        if (e.type == Expose) {
            XFillRectangle(d, w, DefaultGC(d, s), 20, 20, 10, 10);
            XDrawString(d, w, DefaultGC(d, s), 10, 50, msg, strlen(msg));
        }
        if (e.type == KeyPress)
            break;
    }

    XCloseDisplay(d);
}*/

#include "x11window.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

X11Window::X11Window()
{

}

void X11Window::createWindow(std::string title, int width, int height, int framebufferWidth, int framebufferHeight)
{
    scrollPos = 0;
    this->width = width;
    this->height = height;
    int s = 0;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    s = DefaultScreen(display);
    visual = DefaultVisual(display, s);
    if(visual->c_class!=TrueColor) {
        fprintf(stderr, "Cannot handle non true color visual ...\n");
    }

    window = XCreateSimpleWindow(display,
        RootWindow(display, s), 0, 0, width, height, 1, BlackPixel(display, s), WhitePixel(display, s));
    XMapWindow(display, window);
    XSelectInput(display, window, ExposureMask|ButtonPressMask|KeyPressMask|PropertyChangeMask);
    XStoreName( display, window, title.c_str() );
    XSetIconName( display, window, title.c_str() );

    displayBuffer = (char*)malloc(framebufferWidth*framebufferHeight*4);
    for (int i = 0; i < framebufferWidth*framebufferHeight*4; i++)
    {
        displayBuffer[i] = i;
    }

    ximage = XCreateImage(display, visual, 24, ZPixmap, 0,
        displayBuffer, framebufferWidth, framebufferHeight, 32, 0);
}

void X11Window::processWindowEvents()
{
    XWindowAttributes attribs;
    XGetWindowAttributes(display, window, &attribs);
    width = attribs.width;
    height = attribs.height;
    XPutImage(display, window, DefaultGC(display, 0),
        ximage, 0, 0, 0, 0, width, height);
    while(XPending(display))
    {
        XNextEvent(display, &e);
        if (e.type == ButtonPress){
            switch (e.xbutton.button){
                case Button4:
                    scrollPos+=1;
                    break;
                case Button5:
                    scrollPos-=1;
                    break;
            }
        }
    }
    //if (e.type == KeyPress)
}

void X11Window::setTitle(std::string title)
{
    XStoreName(display, window, title.c_str());
}

void X11Window::closeWindow()
{
    XCloseDisplay(display);
}
