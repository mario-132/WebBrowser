#ifndef CSS_H
#define CSS_H
#include <libcss/libcss.h>
#include <gumbo.h>
#include <string>

#define UNUSED(x) ((x) = (x))

/* Function declarations. */
css_error resolve_url(void *pw,
        const char *base, lwc_string *rel, lwc_string **abs);
void die(const char *text, css_error code);

css_error node_name(void *pw, void *node,
        css_qname *qname);
css_error node_classes(void *pw, void *node,
        lwc_string ***classes, uint32_t *n_classes);
css_error node_id(void *pw, void *node,
        lwc_string **id);
css_error named_ancestor_node(void *pw, void *node,
        const css_qname *qname,
        void **ancestor);
css_error named_parent_node(void *pw, void *node,
        const css_qname *qname,
        void **parent);
css_error named_sibling_node(void *pw, void *node,
        const css_qname *qname,
        void **sibling);
css_error named_generic_sibling_node(void *pw, void *node,
        const css_qname *qname,
        void **sibling);
css_error parent_node(void *pw, void *node, void **parent);
css_error sibling_node(void *pw, void *node, void **sibling);
css_error node_has_name(void *pw, void *node,
        const css_qname *qname,
        bool *match);
css_error node_has_class(void *pw, void *node,
        lwc_string *name,
        bool *match);
css_error node_has_id(void *pw, void *node,
        lwc_string *name,
        bool *match);
css_error node_has_attribute(void *pw, void *node,
        const css_qname *qname,
        bool *match);
css_error node_has_attribute_equal(void *pw, void *node,
        const css_qname *qname,
        lwc_string *value,
        bool *match);
css_error node_has_attribute_dashmatch(void *pw, void *node,
        const css_qname *qname,
        lwc_string *value,
        bool *match);
css_error node_has_attribute_includes(void *pw, void *node,
        const css_qname *qname,
        lwc_string *value,
        bool *match);
css_error node_has_attribute_prefix(void *pw, void *node,
        const css_qname *qname,
        lwc_string *value,
        bool *match);
css_error node_has_attribute_suffix(void *pw, void *node,
        const css_qname *qname,
        lwc_string *value,
        bool *match);
css_error node_has_attribute_substring(void *pw, void *node,
        const css_qname *qname,
        lwc_string *value,
        bool *match);
css_error node_is_root(void *pw, void *node, bool *match);
css_error node_count_siblings(void *pw, void *node,
        bool same_name, bool after, int32_t *count);
css_error node_is_empty(void *pw, void *node, bool *match);
css_error node_is_link(void *pw, void *node, bool *match);
css_error node_is_visited(void *pw, void *node, bool *match);
css_error node_is_hover(void *pw, void *node, bool *match);
css_error node_is_active(void *pw, void *node, bool *match);
css_error node_is_focus(void *pw, void *node, bool *match);
css_error node_is_enabled(void *pw, void *node, bool *match);
css_error node_is_disabled(void *pw, void *node, bool *match);
css_error node_is_checked(void *pw, void *node, bool *match);
css_error node_is_target(void *pw, void *node, bool *match);
css_error node_is_lang(void *pw, void *node,
        lwc_string *lang, bool *match);
css_error node_presentational_hint(void *pw, void *node,
        uint32_t *nhints, css_hint **hints);
css_error ua_default_for_property(void *pw, uint32_t property,
        css_hint *hint);
css_error compute_font_size(void *pw, const css_hint *parent,
        css_hint *size);
css_error set_libcss_node_data(void *pw, void *n,
        void *libcss_node_data);
css_error get_libcss_node_data(void *pw, void *n,
        void **libcss_node_data);

class CSS
{
public:
    CSS();

    void init(GumboNode* root, std::string css);
    void printNode(GumboNode* node);
    void printunit(css_unit *unit);
    static std::string gumboTagToString(GumboTag tag);

    static css_select_handler select_handler;
    static bool iequals(const std::string& a, const std::string& b);
private:
    css_media media;
    css_select_ctx *select_ctx;
    css_stylesheet_params params;
};

#endif // CSS_H
