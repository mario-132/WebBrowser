#ifndef CSSPARSE_H
#define CSSPARSE_H
#include <string>
#include <map>
#include <vector>

namespace css
{
    struct CSSBasicItem
    {
        std::string name;
        std::string value;
    };

    struct CSSBasicSelector
    {
        std::string selectorCombo;
        std::vector<CSSBasicItem> items;
    };

    enum CSSAdditionalSelectorOperand
    {
        CSS_DIRECT_CHILD_OF,
        CSS_DIRECTLY_ADJECENT_TO,
        CSS_ADJECENT_TO,
        CSS_INSIDE_OF,
        CSS_NONE,
        CSS_ID_OF
    };

    struct CSSAdditionalSelector
    {
        std::string name;
        CSSAdditionalSelectorOperand selectorOp;
    };

    struct CSSItem
    {
        std::string attribute;
        std::string valueAsString;
    };

    struct CSSSelector
    {
        std::string selector;
        std::vector<CSSAdditionalSelector> additionals;
        std::vector<CSSItem> items;
    };

    bool isChr(char c);

    bool isNum(char c);

    bool isValidSelectorName(char c);

    bool isValidAttributeName(char c);

    bool isValidValueStart(char c);

    std::string removeExcessSpaces(std::string str);

    bool getSelector(std::string css, int cssLength, bool &isFinal, int &curChr, std::string &selector);

    bool getAttribute(std::string css, int cssLength, bool &isFinal, bool &isFinalAttribute, int &curChr, std::string &attribute, bool &attribContinue);

    bool getValue(std::string css, int cssLength, bool &isFinal, bool &isFinalAttribute, int &curChr, std::string &value);

    std::vector<CSSBasicSelector> basicParseFromString(std::string css);

    void parseFromBasic(std::vector<CSSBasicSelector> css);

    void parseFromString(std::string css);
}

#endif // CSSPARSE_H
