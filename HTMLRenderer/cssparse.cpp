#include "cssparse.h"
#include <iostream>
#include <cstring>

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
        while (css[curChr] != ':' && css[curChr] != ';' && css[curChr] != '}' && curChr < cssLength)
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
        for (unsigned int i = 0; i < str.length(); i++)
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
        return isChr(c) || isNum(c) || c == '@' || c == '*' || c == '.' || c == ':' || c == '-' || c == '#' || c == '[' || c == ']' || c == '_' || c == '-';
    }

    bool isNum(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool isChr(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    std::vector<CSSSelectorBlock> parseFromString(std::string css)
    {
        std::vector<CSSBasicSelector> out = basicParseFromString(css);

        for (unsigned int i = 0; i < out.size(); i++)
        {
            //std::cout << "<" << out[i].selectorCombo << ">" << std::endl;
            for (unsigned int j = 0; j < out[i].items.size(); j++)
            {
                //std::cout << "  " << out[i].items[j].name << ":" << out[i].items[j].value << ";" << std::endl;
            }
        }
        return parseFromBasic(out);
    }

    bool isSelectorEnding(char c)
    {
        return (!isChr(c) && !isNum(c) && c != ':' && c != '_' && c != '-');
    }

    bool parseSingleSelector(std::string &selector, unsigned int &numChr, unsigned int &cssSize, std::vector<CSSBasicSelector> &css, unsigned int &i, bool &isFirstSelector, std::vector<CSSAdditionalSelector> &additionalOperands, CSSSelectorBlock &cssSelBlock)
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
            if (numChr < cssSize)// In here aselector character is added to the appropriate string.
            {
                // an element is added to matchingIDs or mathingClasses beforehand somewhere at the end of this function.
                if (writingToMatchingID)
                {
                    addSel.matchingIDs.back() += css[i].selectorCombo[numChr];
                }
                else if (writingToMatchingClass)
                {
                    addSel.matchingClasses.back() += css[i].selectorCombo[numChr];
                }
                else
                {
                    selector += css[i].selectorCombo[numChr];
                }
                numChr++;
            }
            bool continueAndIgnoreSq = false;// Used to just add the input to the selector string incase of [] since it is ignored right now.
                                           // (And made part of the selector name string to avoid conflicts with actual supported selectors
            bool continueAndIgnoreRo = false;

            while (((!isSelectorEnding(css[i].selectorCombo[numChr]) || css[i].selectorCombo[numChr] == '[') || css[i].selectorCombo[numChr] == '(' || continueAndIgnoreSq || continueAndIgnoreRo) && numChr < cssSize)
            {
                if (css[i].selectorCombo[numChr] == '[' && !continueAndIgnoreRo)// If there is a square open bracket, just write the contents paying no mind to what's in it till a end bracket is found.
                {
                    continueAndIgnoreSq = true;
                }
                if (css[i].selectorCombo[numChr] == ']' && !continueAndIgnoreRo)
                {
                    continueAndIgnoreSq = false;
                }

                if (css[i].selectorCombo[numChr] == '(' && !continueAndIgnoreSq)// If there is an round open bracket, just write the contents paying no mind to what's in it till a end bracket is found.
                {
                    continueAndIgnoreRo = true;
                }
                if (css[i].selectorCombo[numChr] == ')' && !continueAndIgnoreSq)
                {
                    continueAndIgnoreRo = false;
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
            // If the current char is a . or a # it means there's an id or class following and we have to read that as well.
            if (css[i].selectorCombo[numChr] == '.')///Todo: Fixme: @mario-132 Bug: doesn't work with more than 1 additional class or ID (this is the combining part e.g.: ".classa.classb.classc {}"
            {
                writingToMatchingClass = true;
                writingToMatchingID = false;
                addSel.matchingClasses.push_back("");// For writing the class into
                //numChr++;
            }
            else if (css[i].selectorCombo[numChr] == '#')
            {
                writingToMatchingClass = false;
                writingToMatchingID = true;
                addSel.matchingIDs.push_back("");// For writing the ID into.
                //numChr++;
            }
            else // No class or id specifier is following this selector so we end it here
            {
                break;
            }
        }
        while((css[i].selectorCombo[numChr] == ' ' || css[i].selectorCombo[numChr] == '\t' || css[i].selectorCombo[numChr] == '\n') && numChr < cssSize)
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
            // If it is none of the above operators and a new selectorStarter character is detected, it means it is a space and therefore an inside of operator.
            if (isValidSelectorNameStart(css[i].selectorCombo[numChr]))
            {
                addSel.selectorOp = CSS_INSIDE_OF;
            }
        }

        //if (selector != "")
        //{
        //    std::cout << " |" << selector << "| ";
        //}

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

    std::vector<CSSSelectorBlock> parseFromBasic(std::vector<CSSBasicSelector> css)
    {
        std::vector<CSSSelectorBlock> CSSSelectorBlocks;
        for (unsigned int i = 0; i < css.size(); i++)
        {
            unsigned int numChr = 0;// To remember at which character we're at.
            unsigned int cssSize = css[i].selectorCombo.size();
            std::string selector;// Temporarily stores the selector name as it is parsed.
            std::vector<CSSAdditionalSelector> additionalOperands;// Stores parsed selectors in reverse order, has to be reversed later.

            CSSSelectorBlock cssSelBlock;// Stores all selectors for a css block

            //std::cout << "Original: " << css[i].selectorCombo << ":";
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
            //std::cout << std::endl;
            for (unsigned int j = 0; j < css[i].items.size(); j++)
            {
                CSSAttribute attr = parseSingleAttribute(css[i].items[j].name);
                CSSValue value = parseSingleValue(css[i].items[j].value);

                CSSItem item;
                item.attribute = attr;
                item.value = value;
                cssSelBlock.items.push_back(item);
            }

            //std::cout << std::endl;
            for (unsigned int a = 0; a < cssSelBlock.selectors.size(); a++)
            {
                /*std::cout << "new css selectorblock item;" << std::endl;
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
                std::cout << std::endl;*/
            }
            CSSSelectorBlocks.push_back(cssSelBlock);
        }
        return CSSSelectorBlocks;
    }

    CSSAttribute parseSingleAttribute(std::string attrib)
    {
        CSSAttribute attribute;
        //std::cout << "<" << attrib << ">";
        for (int i = attrib.size()-1; i >= 0; i--)
        {
            if (attrib[i] == ' ' || attrib[i] == '\t')// Remove tabs and spaces at the end
            {
                attrib.erase(i, 1);
            }
            else
            {
                break;
            }
        }
        //std::cout << " <" << attrib << ">" << std::endl;
        attribute.attributeAsString = attrib;
        return attribute;
    }

    CSSValue parseSingleValue(std::string value)
    {
        CSSValue val;
        val.type = CSS_TYPE_UNKNOWN;
        //std::cout << "<>" << value << "<>";
        for (int i = value.size()-1; i >= 0; i--)
        {
            if (value[i] == ' ' || value[i] == '\t')// Remove tabs and spaces at the end
            {
                value.erase(i, 1);
            }
            else
            {
                break;
            }
        }
        //std::cout << " <>" << value << "<>" << std::endl;
        val.valueAsString = value;

        bool isFunction = false;
        {
            std::string parsedValue;
            int i = 0;
            for (unsigned i = 0; i < value.size(); i++)
            {
                if (value[i] != '(')
                {
                    parsedValue += value[i];
                }
                else
                {
                    isFunction = true;// It has opening bracket so it's a function.
                    break;
                }
            }

            if (isFunction)
            {
                //std::cout << "css function unhandled!" << std::endl;
                if (parsedValue == "rgb")// rgb function
                {

                }
            }
        }

        std::string number;
        std::string unit;
        unsigned int chrI = 0;

        if (!isFunction)
        {
            while((isNum(value[chrI]) || value[chrI] == '.') && chrI < value.size())
            {
                number += value[chrI];
                chrI++;
            }
            /*while(value[chrI] == ' ' && chrI < value.size())
            {
                chrI++;
            }*/
            if (chrI < value.size())
            {
                if (value[chrI] == '#')
                {
                    chrI++;
                    unsigned long c = strtol(value.c_str() + chrI, NULL, 16);
                    //std::cout << value.c_str() + chrI << ": " << c << std::endl;
                    if (strlen(value.c_str() + chrI) == 6)
                    {
                        val.color.r = (c >> 16) & 0xFF;
                        val.color.g = (c >> 8) & 0xFF;
                        val.color.b = (c) & 0xFF;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (strlen(value.c_str() + chrI) == 3)/// Todo: Fix this, you have to do more than the pwoer of 2 lol @mario-132
                    {
                        val.color.r = ((c >> 4) & 0xF0) + ((c >> 8) & 0xF);
                        val.color.g = ((c) & 0xF0) + ((c >> 4) & 0xF);
                        val.color.b = ((c << 4) & 0xF0) + ((c) & 0xF);
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else
                    {
                        std::cerr << "Unrecognised color! Color: " << value.c_str() + chrI << std::endl;
                    }
                }
                else
                {
                    while(value[chrI] != ' ' && chrI < value.size())
                    {
                        unit += value[chrI];
                        chrI++;
                    }

                    if (unit == "black")
                    {
                        val.color.r = 0;
                        val.color.g = 0;
                        val.color.b = 0;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "blue")
                    {
                        val.color.r = 0;
                        val.color.g = 0;
                        val.color.b = 255;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "green")
                    {
                        val.color.r = 0;
                        val.color.g = 0x80;
                        val.color.b = 0;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "red")
                    {
                        val.color.r = 255;
                        val.color.g = 0;
                        val.color.b = 0;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "aqua" || unit == "cyan")
                    {
                        val.color.r = 0;
                        val.color.g = 255;
                        val.color.b = 255;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "lime")
                    {
                        val.color.r = 0;
                        val.color.g = 255;
                        val.color.b = 0;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "yellow")
                    {
                        val.color.r = 255;
                        val.color.g = 255;
                        val.color.b = 0;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "transparent")
                    {
                        val.color.r = 255;
                        val.color.g = 255;
                        val.color.b = 255;
                        val.color.a = 0;
                        val.type = CSS_TYPE_COLOR;
                    }
                    else if (unit == "white")
                    {
                        val.color.r = 255;
                        val.color.g = 255;
                        val.color.b = 255;
                        val.color.a = 255;
                        val.type = CSS_TYPE_COLOR;
                    }


                    if (unit == "px")
                    {
                        //std::cout << "Value: " << stof(number) << " unit: px" << std::endl;
                        val.type = CSS_TYPE_PX;
                        val.numberValue = stof(number);
                    }
                    else if (unit == "em")
                    {
                        //std::cout << "Value: " << stof(number) << " unit: em" << std::endl;
                        val.type = CSS_TYPE_EM;
                        val.numberValue = stof(number);
                    }
                    else if (unit == "%")
                    {
                        //std::cout << "Value: " << stof(number) << " unit: %" << std::endl;
                        val.type = CSS_TYPE_PERCENT;
                        val.numberValue = stof(number);
                    }
                }
            }
            else
            {
                if (number.size() > 0)
                {
                    val.type = CSS_TYPE_NUMBER;
                    val.numberValue = stof(number);
                }
            }
        }

        return val;
    }

    std::string commentRemover(std::string str)
    {
        std::string newStr;
        char prevC = ' ';
        bool isCommenting = false;
        for (unsigned int i = 0; i < str.size(); i++)
        {
            if (str[i] == '/')
            {
                if (i + 1 < str.size())
                {
                    if (str[i+1] == '*')
                    {
                        isCommenting = true;
                    }
                }
            }
            if (str[i] == '*')
            {
                if (i + 1 < str.size())
                {
                    if (str[i+1] == '/')
                    {
                        isCommenting = false;
                        if (i+2 < str.size())
                        {
                            i+=2;
                        }
                    }
                }
            }

            if (isCommenting == false)
            {
                newStr.push_back(str[i]);
            }
            prevC = str[i];
        }
        return newStr;
    }

    std::vector<CSSItem> parseInlineFromString(std::string inlineCSS)
    {
        std::vector<CSSBasicItem> items;
        inlineCSS = commentRemover(inlineCSS);// First remove the comments if there are any

        int curChr = 0;
        int cssLength = inlineCSS.size();

        std::string attribute;
        std::string value;
        bool isFinal = false;// Tells when the selectors have ended, unused since there is are none.
        bool isFinalAttribute = false;// When the last attribute has been found.

        while((inlineCSS[curChr] == ' ' || inlineCSS[curChr] == '\n' || inlineCSS[curChr] == '\t') && curChr < cssLength)
        {
            cssLength++;
        }

        while(curChr < cssLength && !isFinalAttribute)
        {
            attribute = "";
            value = "";

            bool attribContinue = false;
            if (getAttribute(inlineCSS, cssLength, isFinal, isFinalAttribute, curChr, attribute, attribContinue))
            {
                break;
            }
            if (attribContinue)
            {
                continue;
            }

            if (getValue(inlineCSS, cssLength, isFinal, isFinalAttribute, curChr, value))
            {
                break;
            }
            items.push_back({attribute, value});
            //std::cout << "  " << attribute << ":" << value << ";" << std::endl;
        }

        std::vector<CSSItem> parsedItems;
        for (unsigned int i = 0; i < items.size(); i++)
        {
            CSSAttribute attr = parseSingleAttribute(items[i].name);
            CSSValue value = parseSingleValue(items[i].value);

            CSSItem item;
            item.attribute = attr;
            item.value = value;
            parsedItems.push_back(item);
        }

        return parsedItems;
    }
}
