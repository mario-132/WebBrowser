#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "cssparse.h"

std::string fileLoad(std::string path)
{
    std::ifstream htmlFile(path, std::ios::binary | std::ios::ate);
    std::streamsize size = htmlFile.tellg();
    if (size == 0)
    {
        std::cerr << "Failed to open file" << std::endl;
    }
    htmlFile.seekg(0, std::ios::beg);

    char *fileString = (char*)malloc(size);
    htmlFile.read(fileString, size);

    return std::string(fileString);
}

/*struct item
{
    std::string name;
    std::string value;
    bool isFloat;
    bool isInt;
    bool unitPX;
    bool unitUnitless;
};

bool isChr(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNum(char c)
{
    return c >= '0' && c <= '9';
}

std::map<std::string, std::vector<item>> parseCSS(std::string css)
{
    std::string selector;
    std::map<std::string, std::vector<item>> selectors;

    int i = 0;
    int size = css.size();
    while(1)
    {
        if (i >= size)
        {
            return selectors;
        }
        int curItem = 0;
        selector = "";

        while (!isChr(css[i]) && css[i] != '@' && css[i]!='-' && css[i]!='.' && i < size)
        {
            i++;
        }
        while (isChr(css[i]) || isNum(css[i]) || css[i] == '-' || css[i] == '@' || css[i] == '.')
        {
            selector+= css[i];
            i++;
        }
        while (css[i]!='{')
        {
            i++;
        }
        while (!isChr(css[i]) && css[i] != '-')
        {
            i++;
        }
        if (i >= size)
        {
            return selectors;
        }
        bool parsing = true;
        while (parsing)
        {

            selectors[selector].push_back({"", ""});
            while (isChr(css[i]) || isNum(css[i]) || css[i] == '-')
            {
                selectors[selector][curItem].name += css[i];
                i++;
            }
            if (i >= size)
            {
                return selectors;
            }
            while(css[i] != ':')
            {
                if (css[i] == ';')
                {
                    goto novalue;
                }
                i++;
            }
            while(!isChr(css[i]) && !isNum(css[i]) && css[i] != '-')
            {
                if (css[i] == ';')
                {
                    goto novalue;
                }
                i++;
            }
            if (i >= size)
            {
                return selectors;
            }
            while (isChr(css[i]) || isNum(css[i]) || css[i] == '.' || css[i] == '-')
            {
                selectors[selector][curItem].value += css[i];
                i++;
            }
            {
                selectors[selector][curItem].isInt = false;
                selectors[selector][curItem].unitPX = false;
                selectors[selector][curItem].isFloat = false;
                selectors[selector][curItem].unitUnitless = false;
                if (selectors[selector][curItem].value.find("px") != std::string::npos)
                {
                    selectors[selector][curItem].unitPX = true;
                }
                else
                {
                    selectors[selector][curItem].unitUnitless = true;
                }
                if (selectors[selector][curItem].value.find(".") != std::string::npos)
                {
                    selectors[selector][curItem].isFloat = true;
                }
                else
                {
                    selectors[selector][curItem].isInt = true;
                }
            }
            novalue:
            if (i >= size)
            {
                return selectors;
            }
            while (!isChr(css[i]))
            {
                if (css[i] == '}')
                {
                    parsing = false;
                    break;
                }
                i++;
            }
            if (i >= size)
            {
                return selectors;
            }

            curItem++;
        }
        std::cout << selector << ":" << std::endl;
        for (int i = 0; i < selectors[selector].size(); i++)
        {
            std::cout << "  " << selectors[selector][i].name << ":" << selectors[selector][i].value << " | isInt: " << selectors[selector][i].isInt << "/isFloat: " << selectors[selector][i].isFloat << "/UnitPX: " << selectors[selector][i].unitPX << "/UnitUnitless: " << selectors[selector][i].unitUnitless << std::endl;
        }
    }
    return selectors;
}

bool isChr2(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNum2(char c)
{
    return c >= '0' && c <= '9';
}

bool isValidSelectorName(char c)
{
    return isChr2(c) || isNum2(c) || c == '@' || c == '*' || c == '.' || c == ':' || c == '-' || c == '#' || c == '[' || c == ']' || c == '<' || c == '>';
}

bool isValidAttributeName(char c)
{
    return isChr2(c) || isNum2(c) || c == '-';
}

bool isValidValueStart(char c)
{
    return isChr2(c) || isNum2(c) || c == '-' || c == '.' || c == '.' || c == '#' || c == '"' || c == '\'';
}

struct CSSItem
{
    std::string name;
    std::string value;
};

std::string removeExcessSpaces(std::string str)
{
    std::string newStr;
    char lastC = 0;
    bool removingBeginSpaces = true;
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] != ' ')
        {
            removingBeginSpaces = false;
        }
        if (!removingBeginSpaces)
        {
            if (!(lastC == str[i] && str[i] == ' '))
            {
                newStr+=str[i];
            }
        }

        lastC = str[i];
    }
    return newStr;
}

bool getSelector(std::string css, int cssLength, bool &isFinal, int &curChr, std::string &selector)// Returns wether to break
{
    while (!isValidSelectorName(css[curChr]) && curChr < cssLength)
    {
        if (css[curChr] == '{')
        {
            break;
        }
        curChr++;
    }
    while (css[curChr] != '{' && curChr < cssLength)
    {
        selector+=css[curChr];
        curChr++;
    }
    while (css[curChr] != '{' && curChr < cssLength)
    {
        curChr++;
    }
    if (curChr >= cssLength)
    {
        isFinal = true;
        return true;
    }
    return false;
}

bool getAttribute(std::string css, int cssLength, bool &isFinal, bool &isFinalAttribute, int &curChr, std::string &attribute, bool &attribContinue)// Returns wether to break
{
    while (!isValidAttributeName(css[curChr]) && curChr < cssLength)
    {
        if (css[curChr] == '}')
        {
            isFinalAttribute = true;
            break;
        }
        curChr++;
    }
    if (isFinalAttribute)
    {
        return true;
    }
    /*while (isValidAttributeName(css[curChr]) && curChr < cssLength)
    {
        attribute+=css[curChr];
        curChr++;
    while (css[curChr] != ':' && css[curChr] != ';' && curChr < cssLength)
    {
        if (css[curChr] != '\n')
        {
            attribute+=css[curChr];
        }
        curChr++;
    }
    if (css[curChr] == ';')
    {
        attribContinue = true;
        return false;
    }
    while (css[curChr] != ':' && curChr < cssLength)
    {
        curChr++;
    }
    return false;
}

bool getValue(std::string css, int cssLength, bool &isFinal, bool &isFinalAttribute, int &curChr, std::string &value)// Returns wether to break
{
    while (!isValidValueStart(css[curChr]) && curChr < cssLength)
    {
        curChr++;
    }
    while (css[curChr] != ';' && curChr < cssLength)
    {
        if (css[curChr] == '}')
        {
            isFinalAttribute = true;
            break;
        }
        if (css[curChr] == '\n')
        {
            value+=" ";
        }
        else
        {
            value+=css[curChr];
        }
        curChr++;
    }
    if (curChr >= cssLength)
    {
        isFinal = true;
        isFinalAttribute = true;
    }
    return false;
}

void parseCSS_V2(std::string css)
{

}*/

int main()
{
    std::string css = fileLoad("/home/tim/HTMLRenderer/HTML/stress.css");
    //std::string css = fileLoad("/home/tim/HTMLRenderer/HTML/Wikipedia - Wikipedia_files/load.css");
    //std::string css = fileLoad("/home/tim/HTMLRenderer/HTML/es64f4.html");// Garbage
    //std::string css = fileLoad("/home/tim/HTMLRenderer/HTML/Home - MangaDex_files/all.css");
    //std::string css = fileLoad("/home/tim/HTMLRenderer/HTML/Home - MangaDex_files/theme.css");

    css::parseFromString(css);

    return 0;
}
