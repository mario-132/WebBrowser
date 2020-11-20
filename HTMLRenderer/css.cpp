#include "css.h"
#include <iostream>

CSS::CSS()
{

}

css_select_handler CSS::select_handler;

void CSS::init(GumboNode *root, std::string css)
{
    select_handler = {
        CSS_SELECT_HANDLER_VERSION_1,

        node_name,
        node_classes,
        node_id,
        named_ancestor_node,
        named_parent_node,
        named_sibling_node,
        named_generic_sibling_node,
        parent_node,
        sibling_node,
        node_has_name,
        node_has_class,
        node_has_id,
        node_has_attribute,
        node_has_attribute_equal,
        node_has_attribute_dashmatch,
        node_has_attribute_includes,
        node_has_attribute_prefix,
        node_has_attribute_suffix,
        node_has_attribute_substring,
        node_is_root,
        node_count_siblings,
        node_is_empty,
        node_is_link,
        node_is_visited,
        node_is_hover,
        node_is_active,
        node_is_focus,
        node_is_enabled,
        node_is_disabled,
        node_is_checked,
        node_is_target,
        node_is_lang,
        node_presentational_hint,
        ua_default_for_property,
        compute_font_size,
        set_libcss_node_data,
        get_libcss_node_data
    };

    css_error code;
    css_stylesheet *sheet;
    size_t size;

    uint32_t count;

    media = {
        .type = CSS_MEDIA_SCREEN,
    };

    params.params_version = CSS_STYLESHEET_PARAMS_VERSION_1;
    params.level = CSS_LEVEL_21;
    params.charset = "UTF-8";
    params.url = "foo";
    params.title = "bar";
    params.allow_quirks = false;
    params.inline_style = false;
    params.resolve = resolve_url;
    params.resolve_pw = NULL;
    params.import = NULL;
    params.import_pw = NULL;
    params.color = NULL;
    params.color_pw = NULL;
    params.font = NULL;
    params.font_pw = NULL;

    /* create a stylesheet */
    code = css_stylesheet_create(&params, &sheet);
    if (code != CSS_OK)
        die("css_stylesheet_create", code);
    code = css_stylesheet_size(sheet, &size);
    if (code != CSS_OK)
        die("css_stylesheet_size", code);
    printf("created stylesheet, size %zu\n", size);

    /* parse some CSS source */
    code = css_stylesheet_append_data(sheet, (const uint8_t *) css.c_str(),
            css.size());
    if (code != CSS_OK && code != CSS_NEEDDATA)
        die("css_stylesheet_append_data", code);
    code = css_stylesheet_data_done(sheet);
    if (code != CSS_OK)
        die("css_stylesheet_data_done", code);
    code = css_stylesheet_size(sheet, &size);
    if (code != CSS_OK)
        die("css_stylesheet_size", code);
    printf("appended data, size now %zu\n", size);


    /* prepare a selection context containing the stylesheet */
    code = css_select_ctx_create(&select_ctx);
    if (code != CSS_OK)
        die("css_select_ctx_create", code);
    code = css_select_ctx_append_sheet(select_ctx, sheet, CSS_ORIGIN_AUTHOR,
            NULL);
    if (code != CSS_OK)
        die("css_select_ctx_append_sheet", code);
    code = css_select_ctx_count_sheets(select_ctx, &count);
    if (code != CSS_OK)
        die("css_select_ctx_count_sheets", code);
    printf("created selection context with %i sheets\n", count);
}

void CSS::printNode(GumboNode *node)
{
    std::cout << gumboTagToString(node->v.element.tag) << std::endl;

    css_error code;

    css_select_results *style;
    lwc_string *url;
    uint8_t color_type;
    css_color color_shade;

    code = css_select_style(select_ctx, node,
            &media, NULL,
            &select_handler, 0,
            &style);
    if (code != CSS_OK)
        die("css_select_style", code);

    color_type = css_computed_color(
            style->styles[CSS_PSEUDO_ELEMENT_NONE],
            &color_shade);

    lwc_intern_string("a", 1, &url);
    if (css_computed_background_image(style->styles[CSS_PSEUDO_ELEMENT_NONE], &url) && url!=0)
    {
        printf("bg: ");
        printf(lwc_string_data(url));
    }

    css_fixed flength;
    css_unit funit;
    int fret = css_computed_font_size(style->styles[CSS_PSEUDO_ELEMENT_NONE], &flength, &funit);
    if (fret != 0x0)
    {
       printf(" size: %f", FIXTOFLT(flength));
       printunit(&funit);
    }
    else if (fret == CSS_FONT_SIZE_INHERIT)
    {
        printf(" size: inherit");
    }

    if (color_type == CSS_COLOR_INHERIT)
        printf("\n color: 'inherit'\n");
    else
        printf("\n color: %x\n", color_shade);

    uint8_t display = css_computed_display(style->styles[CSS_PSEUDO_ELEMENT_NONE], false);
    std::cout << " display: ";
    switch (display)
    {
    case CSS_DISPLAY_NONE:
    std::cout << "none";
        break;

    case CSS_DISPLAY_BLOCK:
    std::cout << "block";
        break;

    case CSS_DISPLAY_FLEX:
    std::cout << "flex";
        break;

    case CSS_DISPLAY_INLINE:
    std::cout << "inline";
        break;

    case CSS_DISPLAY_INLINE_BLOCK:
    std::cout << "inline-block";
        break;

    default:
        std::cout << "unknown" << std::endl;
        break;
    }
    std::cout << "\n";

    code = css_select_results_destroy(style);
    if (code != CSS_OK)
        die("css_computed_style_destroy", code);
}

void CSS::printunit(css_unit *unit)
{
    switch (*unit)
    {
    case CSS_UNIT_PX:
        printf("px");
        break;

    case CSS_UNIT_EX:
        printf("ex");
        break;

    case CSS_UNIT_EM:
        printf("em");
        break;

    case CSS_UNIT_PT:
        printf("pt");
        break;

    default:
        std::cout << "/?" << *unit << "/";
        break;
    }
}

std::string CSS::gumboTagToString(GumboTag tag)
{
    std::string name;

    if (tag == GUMBO_TAG_P)
    {
        name = "p";
    }
    else if (tag == GUMBO_TAG_IMG)
    {
        name = "img";
    }
    else if (tag == GUMBO_TAG_H1)
    {
        name = "h1";
    }
    else if (tag == GUMBO_TAG_H2)
    {
        name = "h2";
    }
    else if (tag == GUMBO_TAG_H3)
    {
        name = "h3";
    }
    else if (tag == GUMBO_TAG_H4)
    {
        name = "h4";
    }
    else if (tag == GUMBO_TAG_H5)
    {
        name = "h5";
    }
    else if (tag == GUMBO_TAG_H6)
    {
        name = "h6";
    }
    else if (tag == GUMBO_TAG_BODY)
    {
        name = "body";
    }
    else if (tag == GUMBO_TAG_HTML)
    {
        name = "html";
    }
    else if (tag == GUMBO_TAG_B)
    {
        name = "b";
    }
    else if (tag == GUMBO_TAG_BR)
    {
        name = "br";
    }
    else if (tag == GUMBO_TAG_DIV)
    {
        name = "div";
    }
    else if (tag == GUMBO_TAG_SPAN)
    {
        name = "span";
    }
    else if (tag == GUMBO_TAG_A)
    {
        name = "a";
    }
    else if (tag == GUMBO_TAG_HEAD)
    {
        name = "head";
    }
    else if (tag == GUMBO_TAG_STYLE)
    {
        name = "style";
    }
    else if (tag == GUMBO_TAG_CENTER)
    {
        name = "center";
    }
    else if (tag == GUMBO_TAG_BIG)
    {
        name = "big";
    }
    else if (tag == GUMBO_TAG_STRONG)
    {
        name = "strong";
    }
    else if (tag == GUMBO_TAG_SCRIPT)
    {
        name = "script";
    }
    else if (tag == GUMBO_TAG_FORM)
    {
        name = "form";
    }
    else if (tag == GUMBO_TAG_BUTTON)
    {
        name = "button";
    }
    else if (tag == GUMBO_TAG_SECTION)
    {
        name = "section";
    }
    else if (tag == GUMBO_TAG_LINK)
    {
        name = "link";
    }
    else if (tag == GUMBO_TAG_UL)
    {
        name = "ul";
    }
    else if (tag == GUMBO_TAG_LI)
    {
        name = "li";
    }
    else if (tag == GUMBO_TAG_OL)
    {
        name = "ol";
    }
    else if (tag == GUMBO_TAG_TABLE)
    {
        name = "table";
    }
    else if (tag == GUMBO_TAG_TBODY)
    {
        name = "tbody";
    }
    else if (tag == GUMBO_TAG_TR)
    {
        name = "tr";
    }
    else if (tag == GUMBO_TAG_TD)
    {
        name = "td";
    }
    else if (tag == GUMBO_TAG_NOSCRIPT)
    {
        name = "noscript";
    }
    else if (tag == GUMBO_TAG_NOFRAMES)
    {
        name = "noframes";
    }
    else if (tag == GUMBO_TAG_HEADER)
    {
        name = "header";
    }
    else if (tag == GUMBO_TAG_NAV)
    {
        name = "nav";
    }
    else if (tag == GUMBO_TAG_ARTICLE)
    {
        name = "article";
    }
    else if (tag == GUMBO_TAG_ASIDE)
    {
        name = "aside";
    }
    else if (tag == GUMBO_TAG_FOOTER)
    {
        name = "footer";
    }
    else if (tag == GUMBO_TAG_DETAILS)
    {
        name = "details";
    }
    else if (tag == GUMBO_TAG_SUMMARY)
    {
        name = "summary";
    }
    else if (tag == GUMBO_TAG_META)
    {
        name = "meta";
    }
    else if (tag == GUMBO_TAG_VIDEO)
    {
        name = "video";
    }
    else if (tag == GUMBO_TAG_AUDIO)
    {
        name = "audio";
    }
    else if (tag == GUMBO_TAG_HR)
    {
        name = "hr";
    }
    else if (tag == GUMBO_TAG_TEXTAREA)
    {
        name = "textarea";
    }
    else if (tag == GUMBO_TAG_PRE)
    {
        name = "pre";
    }
    else
    {
        name = "unknown";
    }

    return name;
}

bool CSS::iequals(const std::string &a, const std::string &b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

css_error resolve_url(void *pw,
                      const char *base, lwc_string *rel, lwc_string **abs)
{
    UNUSED(pw);
    UNUSED(base);

    /* About as useless as possible */
    *abs = lwc_string_ref(rel);

    return CSS_OK;
}


void die(const char *text, css_error code)
{
    fprintf(stderr, "ERROR: %s: %i: %s\n",
            text, code, css_error_to_string(code));
    exit(EXIT_FAILURE);
}

/* Select handlers. Our "document tree" is actually just a single node, which is
 * a libwapcaplet string containing the element name. Therefore all the
 * functions below except those getting or testing the element name return empty
 * data or false attributes. */
css_error node_name(void *pw, void *n, css_qname *qname)
{
    GumboNode *node = (GumboNode*)n;

    UNUSED(pw);
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        std::cerr << "[node_name]Not an element!" << std::endl;
        qname->name = 0;
        return CSS_INVALID;
    }
    std::string name = CSS::gumboTagToString(node->v.element.tag);
    lwc_intern_string(name.c_str(), name.size(), &qname->name);
    std::cout << "Get node name: " << name << std::endl;

    return CSS_OK;
}

css_error node_classes(void *pw, void *n,
                       lwc_string ***classes, uint32_t *n_classes)
{
    UNUSED(pw);
    UNUSED(n);
    *classes = NULL;
    *n_classes = 0;
    return CSS_OK;
}

css_error node_id(void *pw, void *n, lwc_string **id)
{
    UNUSED(pw);
    UNUSED(n);
    *id = NULL;
    return CSS_OK;
}

css_error named_ancestor_node(void *pw, void *n,
                              const css_qname *qname,
                              void **ancestor)
{
    UNUSED(pw);
    GumboNode *node = (GumboNode*)n;
    GumboNode *p = node->parent;
    while (p != 0 && p->type == GUMBO_NODE_DOCUMENT)
    {
        if (CSS::iequals(CSS::gumboTagToString(p->v.element.tag), lwc_string_data(qname->name)))
        {
            *ancestor = p;
            break;
        }
        *ancestor = NULL;
        p = p->parent;
    }
    std::string ans;
    if (p != 0)
        ans = CSS::gumboTagToString(p->v.element.tag);
    else
        ans = "0";
    std::cout << "got named ancestor: " << ans << " expected: " << lwc_string_data(qname->name) << std::endl;
    return CSS_OK;
}

css_error named_parent_node(void *pw, void *n,
                            const css_qname *qname,
                            void **parent)
{
    UNUSED(pw);
    GumboNode *node = (GumboNode*)n;
    GumboNode *p = node->parent;
    *parent = NULL;
    if (p->type == GUMBO_NODE_DOCUMENT)
    {
        std::cout << "got named parent: 0 expected: " << lwc_string_data(qname->name) << std::endl;
        return CSS_OK;
    }
    if (CSS::iequals(CSS::gumboTagToString(p->v.element.tag), lwc_string_data(qname->name)))
    {
        *parent = p;
    }
    std::string ans;
    if (p != 0)
        ans = CSS::gumboTagToString(p->v.element.tag);
    else
        ans = "0";
    std::cout << "got named parent: " << ans << " expected: " << lwc_string_data(qname->name) << std::endl;
    return CSS_OK;
}

css_error named_generic_sibling_node(void *pw, void *n,
                                     const css_qname *qname,
                                     void **sibling)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    *sibling = NULL;
    return CSS_OK;
}

css_error named_sibling_node(void *pw, void *n,
                             const css_qname *qname,
                             void **sibling)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    *sibling = NULL;
    return CSS_OK;
}

css_error parent_node(void *pw, void *n, void **parent)
{
    GumboNode *node = (GumboNode*)n;
    UNUSED(pw);
    if (node->parent->type == GUMBO_NODE_ELEMENT)
    {
        *parent = node->parent;
        std::cout << "got parent: " << CSS::gumboTagToString(((GumboNode*)*parent)->v.element.tag) << std::endl;
    }
    else
    {
        *parent = 0;
        std::cout << "got parent: " << "0(not an element)" << std::endl;
    }

    return CSS_OK;
}

css_error sibling_node(void *pw, void *n, void **sibling)
{
    UNUSED(pw);
    UNUSED(n);
    *sibling = NULL;
    return CSS_OK;
}

css_error node_has_name(void *pw, void *n,
                        const css_qname *qname,
                        bool *match)
{
    GumboNode *node = (GumboNode*)n;

    UNUSED(pw);
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        std::cerr << "[node_has_name]Not an element!" << std::endl;
        return CSS_INVALID;
    }
    std::string name = CSS::gumboTagToString(node->v.element.tag);
    lwc_string *str;
    lwc_intern_string(name.c_str(), name.size(), &str);

    lwc_string_caseless_isequal(str, qname->name, match);
    std::cout << "is equal: " << lwc_string_data(str) << "/" << lwc_string_data(qname->name) << ":" << &match << std::endl;

    return CSS_OK;
}

css_error node_has_class(void *pw, void *n,
                         lwc_string *name,
                         bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(name);
    *match = false;
    return CSS_OK;
}

css_error node_has_id(void *pw, void *n,
                      lwc_string *name,
                      bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(name);
    *match = false;
    return CSS_OK;
}

css_error node_has_attribute(void *pw, void *n,
                             const css_qname *qname,
                             bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    *match = false;
    return CSS_OK;
}

css_error node_has_attribute_equal(void *pw, void *n,
                                   const css_qname *qname,
                                   lwc_string *value,
                                   bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    UNUSED(value);
    *match = false;
    return CSS_OK;
}

css_error node_has_attribute_dashmatch(void *pw, void *n,
                                       const css_qname *qname,
                                       lwc_string *value,
                                       bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    UNUSED(value);
    *match = false;
    return CSS_OK;
}

css_error node_has_attribute_includes(void *pw, void *n,
                                      const css_qname *qname,
                                      lwc_string *value,
                                      bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    UNUSED(value);
    *match = false;
    return CSS_OK;
}

css_error node_has_attribute_prefix(void *pw, void *n,
                                    const css_qname *qname,
                                    lwc_string *value,
                                    bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    UNUSED(value);
    *match = false;
    return CSS_OK;
}

css_error node_has_attribute_suffix(void *pw, void *n,
                                    const css_qname *qname,
                                    lwc_string *value,
                                    bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    UNUSED(value);
    *match = false;
    return CSS_OK;
}

css_error node_has_attribute_substring(void *pw, void *n,
        const css_qname *qname,
        lwc_string *value,
        bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(qname);
    UNUSED(value);
    *match = false;
    return CSS_OK;
}

css_error node_is_first_child(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_root(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    GumboNode *node = (GumboNode*)n;
    *match = (node->parent->type == GUMBO_NODE_DOCUMENT);
    std::cout << "is root: " << *match << std::endl;
    return CSS_OK;
}

css_error node_count_siblings(void *pw, void *n,
        bool same_name, bool after, int32_t *count)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(same_name);
    UNUSED(after);
    *count = 1;
    return CSS_OK;
}

css_error node_is_empty(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_link(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_visited(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_hover(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_active(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_focus(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_enabled(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_disabled(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_checked(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}

css_error node_is_target(void *pw, void *n, bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    *match = false;
    return CSS_OK;
}


css_error node_is_lang(void *pw, void *n,
        lwc_string *lang,
        bool *match)
{
    UNUSED(pw);
    UNUSED(n);
    UNUSED(lang);
    *match = false;
    return CSS_OK;
}

css_error node_presentational_hint(void *pw, void *node,
        uint32_t *nhints, css_hint **hints)
{
    UNUSED(pw);
    UNUSED(node);
    *nhints = 0;
    *hints = NULL;
    return CSS_OK;
}

css_error ua_default_for_property(void *pw, uint32_t property, css_hint *hint)
{
    UNUSED(pw);

    if (property == CSS_PROP_COLOR) {
        hint->data.color = 0x00000000;
        hint->status = CSS_COLOR_COLOR;
    } else if (property == CSS_PROP_FONT_FAMILY) {
        hint->data.strings = NULL;
        hint->status = CSS_FONT_FAMILY_SANS_SERIF;
    } else if (property == CSS_PROP_QUOTES) {
        /* Not exactly useful :) */
        hint->data.strings = NULL;
        hint->status = CSS_QUOTES_NONE;
    } else if (property == CSS_PROP_VOICE_FAMILY) {
        /** \todo Fix this when we have voice-family done */
        hint->data.strings = NULL;
        hint->status = 0;
    } else {
        return CSS_INVALID;
    }

    return CSS_OK;
}

css_error compute_font_size(void *pw, const css_hint *parent, css_hint *size)
{
    css_hint_length sizes[] = {
        { FLTTOFIX(6.75), CSS_UNIT_PT },
        { FLTTOFIX(7.50), CSS_UNIT_PT },
        { FLTTOFIX(9.75), CSS_UNIT_PT },
        { FLTTOFIX(12.0), CSS_UNIT_PT },
        { FLTTOFIX(13.5), CSS_UNIT_PT },
        { FLTTOFIX(18.0), CSS_UNIT_PT },
        { FLTTOFIX(24.0), CSS_UNIT_PT }
    };
    const css_hint_length *parent_size;

    UNUSED(pw);

    /* Grab parent size, defaulting to medium if none */
    if (parent == NULL) {
        parent_size = &sizes[CSS_FONT_SIZE_MEDIUM - 1];
    } else {
        assert(parent->status == CSS_FONT_SIZE_DIMENSION);
        assert(parent->data.length.unit != CSS_UNIT_EM);
        assert(parent->data.length.unit != CSS_UNIT_EX);
        parent_size = &parent->data.length;
    }

    assert(size->status != CSS_FONT_SIZE_INHERIT);

    if (size->status < CSS_FONT_SIZE_LARGER) {
        /* Keyword -- simple */
        size->data.length = sizes[size->status - 1];
    } else if (size->status == CSS_FONT_SIZE_LARGER) {
        /** \todo Step within table, if appropriate */
        size->data.length.value =
                FMUL(parent_size->value, FLTTOFIX(1.2));
        size->data.length.unit = parent_size->unit;
    } else if (size->status == CSS_FONT_SIZE_SMALLER) {
        /** \todo Step within table, if appropriate */
        size->data.length.value =
                FMUL(parent_size->value, FLTTOFIX(1.2));
        size->data.length.unit = parent_size->unit;
    } else if (size->data.length.unit == CSS_UNIT_EM ||
            size->data.length.unit == CSS_UNIT_EX) {
        size->data.length.value =
            FMUL(size->data.length.value, parent_size->value);

        if (size->data.length.unit == CSS_UNIT_EX) {
            size->data.length.value = FMUL(size->data.length.value,
                    FLTTOFIX(0.6));
        }

        size->data.length.unit = parent_size->unit;
    } else if (size->data.length.unit == CSS_UNIT_PCT) {
        size->data.length.value = FDIV(FMUL(size->data.length.value,
                parent_size->value), FLTTOFIX(100));
        size->data.length.unit = parent_size->unit;
    }

    size->status = CSS_FONT_SIZE_DIMENSION;

    return CSS_OK;
}

css_error set_libcss_node_data(void *pw, void *n,
        void *libcss_node_data)
{
    UNUSED(pw);
    UNUSED(n);

    /* Since we're not storing it, ensure node data gets deleted */
    css_libcss_node_data_handler(&CSS::select_handler, CSS_NODE_DELETED,
            pw, n, NULL, libcss_node_data);

    return CSS_OK;
}

css_error get_libcss_node_data(void *pw, void *n,
        void **libcss_node_data)
{
    UNUSED(pw);
    UNUSED(n);
    *libcss_node_data = NULL;

    return CSS_OK;
}
