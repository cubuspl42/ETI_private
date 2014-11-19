#include "xml.h"
#include <ctype.h>

static const char *valid_declaration = "<?xml version=\"1.0\"?>";

static void expect_char(std::istream &is, char c)
{
    if(is.peek() != c) {
        is.setstate(std::ios::failbit);
        fprintf(stderr, "%i, Nie znaleziono znaku %c, lecz %c (%d)\n", int(is.tellg()), c, char(is.peek()), is.peek());
    } else is.get();
}

static void parse_identifier(std::istream &is, std::string &str)
{
    while(isalnum(is.peek()) || is.peek() == '-') {
        str += is.get();
    }
}

void skip_whitespace(std::istream &is)
{
    while(isspace(is.peek()))
        is.get();
}

void expect_valid_declaration(std::istream &is)
{
    std::string decl;
    getline(is, decl);
    if(decl != valid_declaration) {
        fprintf(stderr, "Deklaracja różna od <?xml version=\"1.0\"?>\n");
        is.setstate(std::ios::failbit);
    }
}

void parse_start_tag(std::istream &is, const std::string &name, AttrMap &attrs)
{
    fprintf(stderr, "%d: Otwieranie tagu %s\n", (int)is.tellg(), name.c_str());
    auto start = is.tellg();
    skip_whitespace(is);
    expect_char(is, '<'), skip_whitespace(is);
    std::string tag_name;
    parse_identifier(is, tag_name), skip_whitespace(is);
    if(tag_name != name)
        is.setstate(std::ios::failbit);
    unsigned i = 0;
    while(is && is.peek() != '>') {
        if(++i > max_attributes) {
            is.setstate(std::ios::failbit);
            break;
        }
        std::string attr_name, attr_value;
        parse_identifier(is, attr_name), skip_whitespace(is);
        if(!attr_name.length())
            break;
        expect_char(is, '='), skip_whitespace(is);
        expect_char(is, '"'), skip_whitespace(is);
        parse_identifier(is, attr_value), skip_whitespace(is);
        expect_char(is, '"'), skip_whitespace(is);
        attrs[attr_name] = attr_value;
    }
    expect_char(is, '>');
    if(!is) {
        is.clear();
        fprintf(stderr, "%d: Przestawianie wskaźnika\n", int(is.tellg()));
        is.seekg(start);
        is.setstate(std::ios::failbit);
    }
}

bool try_parse_start_tag(std::istream &is, const std::string &name, AttrMap &attrs) {
    if(!is) {
        fprintf(stderr, "Strumień zastano w złym stanie (%s)\n", name.c_str());
        return false;
    }
    parse_start_tag(is, name, attrs);
    if(!is) {
        is.clear();
        attrs.clear();
        fprintf(stderr, "Naprawianie strumienia (%s) -> %d\n", name.c_str(), int(is.tellg()));
        return false;
    }
    return true;
}

void parse_end_tag(std::istream &is, const std::string &name)
{
    fprintf(stderr, "%d: Zamykanie tagu %s\n", (int)is.tellg(), name.c_str());
    auto start = is.tellg();
    skip_whitespace(is);
    expect_char(is, '<'), skip_whitespace(is);
    expect_char(is, '/'), skip_whitespace(is);
    std::string tag_name;
    parse_identifier(is, tag_name);
    if(tag_name != name)
        is.setstate(std::ios::failbit);
    skip_whitespace(is);
    expect_char(is, '>');
    if(!is)
        is.seekg(start);
}

void write_declaration(std::ostream &os) {
    os << valid_declaration << "\n";
}

void write_indentation(std::ostream &os,unsigned indent_level) {
    for(unsigned i = 0; i < indent_level; ++i)
        os << "    ";
}
