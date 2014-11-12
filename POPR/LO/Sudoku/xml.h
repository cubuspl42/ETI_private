#ifndef xml_h
#define xml_h

#include "map.h"
#include <istream>
#include <string>

const unsigned max_attributes = 2;
typedef map<std::string, std::string, max_attributes> AttrMap;
void skip_whitespace(std::istream &is);
void expect_valid_declaration(std::istream &is);
void parse_start_tag(std::istream &is, const std::string &name, map<std::string, std::string, max_attributes> &attrs);
bool try_parse_start_tag(std::istream &is, const std::string &name, map<std::string, std::string, max_attributes> &attrs);
void parse_end_tag(std::istream &is, const std::string &name);

#endif
