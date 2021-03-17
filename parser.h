#ifndef __PARSER_H
#define __PARSER_H

#include <libxml++/libxml++.h>
#include <map>
#include <functional>
#include <optional>

struct Element {
  std::string type;
  std::map<std::string, std::string> attrs;
  std::vector<std::pair<std::string, std::string>> tags;
  std::vector<std::map<std::string, std::string>> members;

  void save();

  void addTag(std::string key, std::string value) {
    tags.push_back(std::make_pair(key, value));
  }

  void addMember(std::map<std::string, std::string> member) {
    members.push_back(member);
  }
};

class OsmChangeFile : public xmlpp::SaxParser
{
public:
  OsmChangeFile();

  std::string action;

  Element element;

  std::vector<std::string> elementTypes{"node", "way", "relation"};
  std::vector<std::string> actions{"create", "modify", "delete"};
  std::map<std::string, Element> refNodes;

protected:
  //overrides:
  void setElement(std::string value);


  void on_start_element(const Glib::ustring& name,
                        const AttributeList& properties);
  void on_end_element(const Glib::ustring& name);

private:
  std::string find(const AttributeList& attributes, Glib::ustring key); 
  std::map<std::string, std::string> parseAttrs(const AttributeList& attributes);
};


#endif