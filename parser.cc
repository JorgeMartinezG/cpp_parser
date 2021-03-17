#include "parser.h"
#include <iostream>
#include <algorithm>

void Element::save() {
  std::cout << "Saving element: " << type << " => " << attrs["id"] << " tags:" << tags.size() << std::endl;
}

std::map<std::string, std::string>
OsmChangeFile::parseAttrs(const AttributeList& attributes) {
  // Create pointer to struct.
  std::map<std::string, std::string> attrs;
  for(auto iter = attributes.begin(); iter != attributes.end(); ++iter)
  {
    attrs.insert(std::pair<std::string, std::string>(iter->name, iter->value));
  }

  return attrs;
}


OsmChangeFile::OsmChangeFile() : xmlpp::SaxParser() {}

std::string OsmChangeFile::find(const AttributeList& attributes, Glib::ustring key) {
  AttributeList::const_iterator attribute = std::find_if(
      attributes.begin(), attributes.end(), xmlpp::SaxParser::AttributeHasName(key));
  std::string value = attribute->value;

  return value;
}

void OsmChangeFile::on_start_element(const Glib::ustring& name,
                                   const AttributeList& attributes)
{
  // Check action -. create, delete, modify.
  if (std::count(this->actions.begin(), this->actions.end(), name) > 0) {
    std::cout << "Started processing action: " << name << std::endl;
    this->action = name;
  }

  // Iniitalize OSM element.
  if (std::count(this->elementTypes.begin(), this->elementTypes.end(), name) > 0) {
    this->element = Element{
      .type = name,
      .attrs = parseAttrs(attributes),
    };
  }
  // Tag nodes.
  if (name == "tag") {
    this->element.addTag(find(attributes, "k"), find(attributes, "v"));
  }

  // Ref nodes.
  if (name == "nd") {
    std::string ref = find(attributes, "ref");
  }

  // For relations.
  if (name == "member") {
    auto member_attrs = parseAttrs(attributes);
    this->element.addMember(member_attrs);
  }


}

void OsmChangeFile::on_end_element(const Glib::ustring& name) {
  // Closing tag for node, name and relation.
  if (std::count(this->elementTypes.begin(), this->elementTypes.end(), name) > 0) {
    // Validate if there are tags.
    if (name == "node") {
      if (this->element.tags.size() > 0) {
        this->element.save();
     
      } else {
        this->refNodes[this->element.attrs["id"]] = this->element;
      }
    }

    if (name == "way") {
      this->element.save();
    }

    if (name == "relation") {
      this->element.save();
    }

  }

  // Check action.
  if (std::count(this->actions.begin(), this->actions.end(), name) > 0) {
    std::cout << "Finished processing action: " << name << std::endl;
  }

}
