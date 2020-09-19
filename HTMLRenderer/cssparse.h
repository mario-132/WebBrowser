#ifndef CSSPARSE_H
#define CSSPARSE_H
#include <string>
#include <map>
#include <vector>

namespace css
{
    /// Contains a CSS item in string form and unparsed for later parsing.
    struct CSSBasicItem
    {
        std::string name;
        std::string value;
    };

    /// Contains one or more CSS selectors in string form and unpares, must be parser later.
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

    /// CSSAdditionalSelector contains more specific selectors like the "div >" part in "div > p {}". For more
    /// info on that read the CSSSelector comment.
    /// matchingIDs and matchingClasses contain other classes or IDs this selector has to match,
    /// if the base selector(in name) already is an ID, matching IDs would be useless as html elements
    /// can only have one id.
    struct CSSAdditionalSelector
    {
        std::string name;
        CSSAdditionalSelectorOperand selectorOp;
        std::vector<std::string> matchingIDs;
        std::vector<std::string> matchingClasses;
    };

    struct CSSColor
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    struct CSSAttribute
    {
        std::string attributeAsString;
    };

    enum CSSValueType
    {
        CSS_TYPE_UNKNOWN,
        CSS_TYPE_PX,
        CSS_TYPE_EM,
        CSS_TYPE_PERCENT,
        CSS_TYPE_NUMBER,
        CSS_TYPE_COLOR
    };

    struct CSSValue
    {
        std::string valueAsString;

        CSSValueType type;
        float numberValue;// Only use when type is known.

        CSSColor color;
    };

    /// Contains the attribute and value. The value is in multiple forms to allow for easy access.
    struct CSSItem
    {
        CSSAttribute attribute;
        CSSValue value;
    };

    /// CSSSelector contains the base selector(the most right one in the css stylesheet) and it's associated
    /// items and additionals.
    /// The additionals are more specific selectors, it has things like "div > p {}". P would be in the selector
    /// string and the div and > would be in additionals as a combination.(> would be CSS_DIRECTLY_CHILD_OF
    /// in the CSSAdditionalSelector.selectorOP.
    struct CSSSelector
    {
        std::string selector;
        std::vector<CSSAdditionalSelector> additionals;
    };

    struct CSSSelectorBlock
    {
        std::vector<CSSSelector> selectors;
        std::vector<CSSItem> items;
    };

    bool isChr(char c);///< Checks if c is a a-z or A-Z ascii character

    bool isNum(char c);///< Checks if c is a 0-9 number

    bool isValidSelectorNameStart(char c);

    bool isValidAttributeName(char c);

    bool isValidValueStart(char c);

    std::string removeExcessSpaces(std::string str);

    bool getSelector(std::string css, int cssLength, bool &isFinal, int &curChr, std::string &selector);

    bool getAttribute(std::string css, int cssLength, bool &isFinal, bool &isFinalAttribute, int &curChr, std::string &attribute, bool &attribContinue);

    bool getValue(std::string css, int cssLength, bool &isFinal, bool &isFinalAttribute, int &curChr, std::string &value);

    std::vector<CSSBasicSelector> basicParseFromString(std::string css);/// Takes a stylesheet and spits it up, it does not fully parse.

    std::vector<CSSSelectorBlock> parseFromBasic(std::vector<CSSBasicSelector> css);/// Parses the basicParse contents.

    std::vector<CSSSelectorBlock> parseFromString(std::string css);/// Takes a stylesheet as a string and parses it.

    std::vector<CSSItem> parseInlineFromString(std::string inlineCSS);

    std::string commentRemover(std::string str);

    CSSAttribute parseSingleAttribute(std::string attrib);
    CSSValue parseSingleValue(std::string value);
}

#endif // CSSPARSE_H
