#include "debugger.h"
#include <iostream>
#include <thread>
#include <X11/Xlib.h>

GtkBuilder *builder;
GtkWidget *window;
bool windowOpen = false;
bool buttonWasClicked = false;

void Debugger::start()
{
    windowOpen = true;
    //std::thread *thread = new std::thread(ui_main);
    ui_main();
    //std::thread *thread = new std::thread(Debugger::loop);
}

int Debugger::ui_main()
{
    //XInitThreads();
    gtk_init(0,0);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "/home/tim/Documents/GitProjects/WebBrowser/debugger.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);

    // get pointers to the two labels
    //urlBox = GTK_WIDGET(gtk_builder_get_object(builder, "urlBox"));
    //textBox = GTK_WIDGET(gtk_builder_get_object(builder, "textBox"));
    //spinner = GTK_WIDGET(gtk_builder_get_object(builder, "spinner"));
    //g_lbl_count = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_count"));

    gtk_spinner_stop(GTK_SPINNER(gtk_builder_get_object(builder, "loadingSpinner")));

    //g_object_unref(builder);

    //g_timeout_add(16, update, 0);

    gtk_widget_show(window);
    //gtk_main();

    return 0;
}

extern "C"
{
    // called when button is clicked
    void on_btn_go_clicked()
    {
        buttonWasClicked = true;
    }

    // called when window is closed
    void on_gtk_window_destroy()
    {
        gtk_main_quit();
    }
    void on_gtk_menu_exit()
    {
        gtk_main_quit();
    }
}

void Debugger::setEntryText(std::string name, std::string contents)
{
    gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder, name.c_str())), contents.c_str());
}

std::string Debugger::getEntryText(std::string name)
{
    return gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, name.c_str())));
}

void Debugger::setTextBoxText(std::string name, std::string text)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, name.c_str())));
    gtk_text_buffer_set_text(buffer, text.c_str(), text.size());
}

std::string Debugger::getTextBoxText(std::string name)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, name.c_str())));
    GtkTextIter endItr;
    GtkTextIter beginItr;
    gtk_text_buffer_get_end_iter(buffer, &endItr);
    gtk_text_buffer_get_iter_at_offset(buffer, &beginItr, 0);
    return gtk_text_buffer_get_text(buffer, &beginItr, &endItr, true);
}

void Debugger::setSpinnerEnabled(std::string name, bool enabled)
{
    if (enabled)
            gtk_spinner_start(GTK_SPINNER(gtk_builder_get_object(builder, name.c_str())));
        else
            gtk_spinner_stop(GTK_SPINNER(gtk_builder_get_object(builder, name.c_str())));
}

bool Debugger::getWasGoButtonPressed()
{
    if (buttonWasClicked)
    {
        buttonWasClicked = false;
        return true;
    }
    return false;
}

void Debugger::loop()
{
    while (gtk_events_pending())
        gtk_main_iteration();
}

bool Debugger::windowIsOpen()
{
    return windowOpen;
}

bool Debugger::getCheckboxEnabled(std::string name)
{
    return gtk_toggle_button_get_active(&GTK_CHECK_BUTTON(gtk_builder_get_object(builder, name.c_str()))->toggle_button);
}

float Debugger::getAdjustmentGetValue(std::string name)
{
    return (float)gtk_adjustment_get_value(GTK_ADJUSTMENT(gtk_builder_get_object(builder, name.c_str())));
}
