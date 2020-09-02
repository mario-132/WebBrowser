#include "cssparse.h"
#include <iostream>

namespace css
{
    std::vector<CSSBasicSelector> basicParseFromString(std::string css)
    {
        std::string selector;
        std::string attribute;
        std::string value;
        std::vector<CSSBasicSelector> selectors;
        int cssLength = css.length();
        int curChr = 0;
        int curSelectorIndex = 0;
        bool isFinal = false;
        while (!isFinal)
        {
            selector = "";

            if (getSelector(css, cssLength, isFinal, curChr, selector))
            {
                break;
            }
            CSSBasicSelector sel;
            sel.selectorCombo = selector;
            selectors.push_back(sel);
            curSelectorIndex = selectors.size()-1;
            bool isFinalAttribute = false;
            //std::cout << "<" << selector << ">" << std::endl;
            while(!isFinalAttribute)
            {
                attribute = "";
                value = "";

                bool attribContinue = false;
                if (getAttribute(css, cssLength, isFinal, isFinalAttribute, curChr, attribute, attribContinue))
                {
                    break;
                }
                if (attribContinue)
                {
                    continue;
                }

                if (getValue(css, cssLength, isFinal, isFinalAttribute, curChr, value))
                {
                    break;
                }
                selectors[curSelectorIndex].items.push_back({attribute, value});
                //std::cout << "  " << attribute << ":" << value << ";" << std::endl;
            }
        }
        return selectors;
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
        }*/
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

    bool getSelector(std::string css, int cssLength, bool &isFinal, int &curChr, std::string &selector)// Returns wether to break
    {
        while (!isValidSelectorNameStart(css[curChr]) && curChr < cssLength)
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

    bool isValidValueStart(char c)
    {
        return isChr(c) || isNum(c) || c == '-' || c == '.' || c == '.' || c == '#' || c == '"' || c == '\'';
    }

    bool isValidAttributeName(char c)
    {
        return isChr(c) || isNum(c) || c == '-';
    }

    bool isValidSelectorNameStart(char c)
    {
        return isChr(c) || isNum(c) || c == '@' || c == '*' || c == '.' || c == ':' || c == '-' || c == '#' || c == '[' || c == ']' || c == '<' || c == '>';
    }

    bool isNum(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool isChr(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    void parseFromString(std::string css)
    {
        std::vector<CSSBasicSelector> out = basicParseFromString(css);

        for (int i = 0; i < out.size(); i++)
        {
            std::cout << "<" << out[i].selectorCombo << ">" << std::endl;
            for (int j = 0; j < out[i].items.size(); j++)
            {
                std::cout << "  " << out[i].items[j].name << ":" << out[i].items[j].value << ";" << std::endl;
            }
        }
        parseFromBasic(out);
    }

    bool isSelectorEnding(char c)
    {
        return (!isChr(c) && !isNum(c) && c != ':');
    }

    void parseFromBasic(std::vector<CSSBasicSelector> css)
    {
        for (int i = 0; i < css.size(); i++)
        {
            int numChr = 0;
            int cssSize = css[i].selectorCombo.size();
            std::string selector;
            std::vector<CSSAdditionalSelector> additionalOperands;

            while (!isValidSelectorNameStart(css[i].selectorCombo[numChr]) && numChr < cssSize)
            {
                numChr++;
            }
            std::cout << "Original: " << css[i].selectorCombo << ":";
            bool isFirstSelector = true;
            while(1)
            {
                selector = "";
                CSSAdditionalSelector addSel;
                if (isFirstSelector)
                {
                    addSel.selectorOp = CSS_NONE;
                }

                if (numChr < cssSize)
                {
                    if (css[i].selectorCombo[numChr] != ' ')
                    {
                        selector+= css[i].selectorCombo[numChr];
                    }
                    numChr++;
                }
                while (!isSelectorEnding(css[i].selectorCombo[numChr]) && numChr < cssSize)
                {
                    selector+= css[i].selectorCombo[numChr];
                    numChr++;
                }
                if (css[i].selectorCombo[numChr] == '.')
                {
                    addSel.matchingClasses.push_back(".");
                    numChr++;
                    while (!isSelectorEnding(css[i].selectorCombo[numChr]) && numChr < cssSize)
                    {
                        addSel.matchingClasses.back()+= css[i].selectorCombo[numChr];
                        numChr++;
                    }
                }
                if (css[i].selectorCombo[numChr] == '#')
                {
                    addSel.matchingIDs.push_back("#");
                    numChr++;
                    while (!isSelectorEnding(css[i].selectorCombo[numChr]) && numChr < cssSize)
                    {
                        addSel.matchingIDs.back()+= css[i].selectorCombo[numChr];
                        numChr++;
                    }
                }

                if (selector != "")
                {
                    std::cout << " |" << selector << "| ";
                }
                if (numChr >= cssSize)
                {
                    break;
                }

                addSel.name = selector;

                additionalOperands.push_back(addSel);
                isFirstSelector = false;
            }
            std::cout << std::endl;
            for (int j = 0; j < additionalOperands.size(); j++)
            {
                std::cout << "New Selector: " << additionalOperands[j].name << std::endl;
                for (int k = 0; k < additionalOperands[j].matchingClasses.size(); k++)
                {
                    std::cout << "Matching class: " << additionalOperands[j].matchingClasses[k] << std::endl;
                }
                for (int k = 0; k < additionalOperands[j].matchingIDs.size(); k++)
                {
                    std::cout << "Matching ID: " << additionalOperands[j].matchingIDs[k] << std::endl;
                }
            }
        }
    }

}
