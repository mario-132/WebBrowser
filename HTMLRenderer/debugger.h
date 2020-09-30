#ifndef DEBUGGER_H
#define DEBUGGER_H
#include <string>
#include <gtk/gtk.h>

namespace Debugger
{
    void start();

    int ui_main();

    void setEntryText(std::string name, std::string contents);
    std::string getEntryText(std::string name);

    void setTextBoxText(std::string name, std::string text);
    std::string getTextBoxText(std::string name);

    void setSpinnerEnabled(std::string name, bool enabled);

    bool getCheckboxEnabled(std::string name);

    float getAdjustmentGetValue(std::string name);

    bool getWasGoButtonPressed();

    bool windowIsOpen();

    void loop();
};

#endif // DEBUGGER_H
