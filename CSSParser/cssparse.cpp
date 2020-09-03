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
        while (!isFinal)// Keep looping till all selectors and their contents have been found
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
            bool isFinalAttribute = false;// End the while if the final attribute has been found(meaning } has been found).
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
        return selectors;// Return the results for further parsing by another function
    }

    bool getValue(std::string css, int cssLength, bool &isFinal, bool &isFinalAttribute, int &curChr, std::string &value)// Returns wether to break
    {
        while (!isValidValueStart(css[curChr]) && curChr < cssLength)// Read and discard till we reach the value's start.(skipping spaces and stuff)
        {
            curChr++;
        }
        while (css[curChr] != ';' && curChr < cssLength)// From the start of the value, read everything till the ;.
        {
            if (css[curChr] == '}')// End of selector block, break.
            {
                isFinalAttribute = true;
                break;
            }
            if (css[curChr] == '\n')// Replace newline with space for convenience later.
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
        while (!isValidAttributeName(css[curChr]) && curChr < cssLength)// Read and discard all character till value start has been found.
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
        while (!isValidSelectorNameStart(css[curChr]) && curChr < cssLength)// Read and discard till selector name start.
        {
            if (css[curChr] == '{')
            {
                break;
            }
            curChr++;
        }
        while (css[curChr] != '{' && curChr < cssLength)// Then read everything till the open bracket for later parsing.
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

    std::string removeExcessSpaces(std::string str)// Simply removes all unnecesery spaces.
    {
        // Removes all spaces at the beginning of the string, replaces a row of
        //  2 spaces or more with a single space, leaves max one space at the end.
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
        return isChr(c) || isNum(c) || c == '@' || c == '*' || c == '.' || c == ':' || c == '-' || c == '#' || c == '[' || c == ']';
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

    bool parseSingleSelector(std::string &selector, int &numChr, int &cssSize, std::vector<CSSBasicSelector> &css, int &i, bool &isFirstSelector, std::vector<CSSAdditionalSelector> &additionalOperands, CSSSelectorBlock &cssSelBlock)
    {
        selector = "";
        CSSAdditionalSelector addSel;
        addSel.selectorOp = CSS_NONE;

        while (!isValidSelectorNameStart(css[i].selectorCombo[numChr]) && numChr < cssSize)// Get rid of useless tabs/spaces at the beginning
        {
            numChr++;
        }

        // These are for identifying weather the while loop below should be writing to the selector string or a matchin* element/string.
        bool writingToMatchingClass = false;
        bool writingToMatchingID = false;
        while(numChr < cssSize)
        {
            if (numChr < cssSize)
            {
                if (writingToMatchingID)
                {
                    addSel.matchingIDs.back()+= css[i].selectorCombo[numChr];
                }
                else if (writingToMatchingClass)
                {
                    addSel.matchingClasses.back()+= css[i].selectorCombo[numChr];
                }
                else
                {
                    selector+= css[i].selectorCombo[numChr];
                }
                numChr++;
            }
            bool continueAndIgnore = false;// Used to just add the input to the selector string incase of [] since it is ignored right now.
                                           // (And made part of the selector name string to avoid conflicts with actual supported selectors
            while (((!isSelectorEnding(css[i].selectorCombo[numChr]) || css[i].selectorCombo[numChr] == '[') || continueAndIgnore) && numChr < cssSize)
            {
                if (css[i].selectorCombo[numChr] == '[')
                {
                    continueAndIgnore = true;
                }
                if (css[i].selectorCombo[numChr] == ']')
                {
                    continueAndIgnore = false;
                }
                if (writingToMatchingID)
                {
                    addSel.matchingIDs.back()+= css[i].selectorCombo[numChr];
                }
                else if (writingToMatchingClass)
                {
                    addSel.matchingClasses.back()+= css[i].selectorCombo[numChr];
                }
                else
                {
                    selector+= css[i].selectorCombo[numChr];
                }

                numChr++;
            }
            if (css[i].selectorCombo[numChr] == '.')///Todo: Fixme: @mario-132 Bug: doesn't work with more than 1 additional class or ID (this is the combining part e.g.: ".classa.classb.classc {}"
            {
                writingToMatchingClass = true;
                writingToMatchingID = false;
                addSel.matchingClasses.push_back(".");
                numChr++;
            }
            else if (css[i].selectorCombo[numChr] == '#')
            {
                writingToMatchingClass = false;
                writingToMatchingID = true;
                addSel.matchingIDs.push_back("#");
                numChr++;
            }
            else
            {
                break;
            }
        }
        while(css[i].selectorCombo[numChr] == ' ' && numChr < cssSize)
        {
            numChr++;
        }
        bool pushbackSelector = false;
        if (css[i].selectorCombo[numChr] == '>')
        {
            addSel.selectorOp = CSS_DIRECT_CHILD_OF;
            numChr++;
        }
        else if (css[i].selectorCombo[numChr] == '+')
        {
            addSel.selectorOp = CSS_DIRECTLY_ADJECENT_TO;
            numChr++;
        }
        else if (css[i].selectorCombo[numChr] == '~')
        {
            addSel.selectorOp = CSS_ADJECENT_TO;
            numChr++;
        }
        else if (css[i].selectorCombo[numChr] == ',')/// FIXME: TODO: @mario-132 Currently, ',' is ignored and is added as a additionalselector with CSS_NONE.
        {

            pushbackSelector = true;
            numChr++;
        }
        else if (numChr < cssSize)
        {
            if (isValidSelectorNameStart(css[i].selectorCombo[numChr]))
            {
                addSel.selectorOp = CSS_INSIDE_OF;
            }
        }

        if (selector != "")
        {
            std::cout << " |" << selector << "| ";
        }

        addSel.name = selector;
        additionalOperands.push_back(addSel);
        if (pushbackSelector)
        {
            cssSelBlock.selectors.push_back(CSSSelector());
            cssSelBlock.selectors.back().selector = "INVALID";
            cssSelBlock.selectors.back().additionals = additionalOperands;
            additionalOperands.clear();
        }
        isFirstSelector = false;

        if (numChr >= cssSize)
        {
            return true;
        }
        return false;
    }

    void parseFromBasic(std::vector<CSSBasicSelector> css)
    {
        for (int i = 0; i < css.size(); i++)
        {
            int numChr = 0;// To remember at which character we're at.
            int cssSize = css[i].selectorCombo.size();
            std::string selector;// Temporarily stores the selector name as it is parsed.
            std::vector<CSSAdditionalSelector> additionalOperands;// Stores parsed selectors in reverse order, has to be reversed later.

            CSSSelectorBlock cssSelBlock;

            std::cout << "Original: " << css[i].selectorCombo << ":";
            bool isFirstSelector = true;
            while(numChr < cssSize)
            {
                if (parseSingleSelector(selector, numChr, cssSize, css, i, isFirstSelector, additionalOperands, cssSelBlock))
                {
                    break;
                }
            }
            cssSelBlock.selectors.push_back(CSSSelector());
            cssSelBlock.selectors.back().selector = "INVALID";
            cssSelBlock.selectors.back().additionals = additionalOperands;

            std::cout << std::endl;
            for (int a = 0; a < cssSelBlock.selectors.size(); a++)
            {
                std::cout << "new css selectorblock item;" << std::endl;
                for (int j = 0; j < cssSelBlock.selectors[a].additionals.size(); j++)
                {
                    std::cout << "New Selector: " << cssSelBlock.selectors[a].additionals[j].name;

                    if (cssSelBlock.selectors[a].additionals[j].selectorOp == CSS_NONE)
                    {
                        std::cout << " selectorOP: CSS_NONE" << std::endl;
                    }
                    if (cssSelBlock.selectors[a].additionals[j].selectorOp == CSS_DIRECT_CHILD_OF)
                    {
                        std::cout << " selectorOP: CSS_DIRECT_CHILD_OF" << std::endl;
                    }
                    if (cssSelBlock.selectors[a].additionals[j].selectorOp == CSS_DIRECTLY_ADJECENT_TO)
                    {
                        std::cout << " selectorOP: CSS_DIRECTLY_ADJECENT_TO" << std::endl;
                    }
                    if (cssSelBlock.selectors[a].additionals[j].selectorOp == CSS_ADJECENT_TO)
                    {
                        std::cout << " selectorOP: CSS_ADJECENT_TO" << std::endl;
                    }
                    if (cssSelBlock.selectors[a].additionals[j].selectorOp == CSS_INSIDE_OF)
                    {
                        std::cout << " selectorOP: CSS_INSIDE_OF" << std::endl;
                    }

                    for (int k = 0; k < cssSelBlock.selectors[a].additionals[j].matchingClasses.size(); k++)
                    {
                        std::cout << "Matching class: " << cssSelBlock.selectors[a].additionals[j].matchingClasses[k] << std::endl;
                    }
                    for (int k = 0; k < cssSelBlock.selectors[a].additionals[j].matchingIDs.size(); k++)
                    {
                        std::cout << "Matching ID: " << cssSelBlock.selectors[a].additionals[j].matchingIDs[k] << std::endl;
                    }
                }
                std::cout << std::endl;
            }
        }
    }

}
