#ifndef xml_h
#define xml_h

#include "map.h"
#include <iostream>
#include <string>

const unsigned max_attributes = 2;
typedef ktl::map<std::string, std::string, max_attributes> AttrMap;
void skip_whitespace(std::istream &is);
void expect_valid_declaration(std::istream &is);
void parse_start_tag(std::istream &is, const std::string &name, AttrMap &attrs);
bool try_parse_start_tag(std::istream &is, const std::string &name, AttrMap &attrs);
void parse_end_tag(std::istream &is, const std::string &name);

void write_declaration(std::ostream &os);
void write_indentation(std::ostream &os,unsigned indent_level);

#endif
