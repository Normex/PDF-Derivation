////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_classmap_h
#define tagged_pdf_classmap_h

#include "Pdfix.h"

#include <string>
#include <unordered_map>
#include <array>

using std::string;
using std::unordered_map;
using std::array;
using namespace PDFixSDK;

using cm_attribute_map = unordered_map<string, unordered_map<string, string>>;

class CClassMap final {
public:
  explicit CClassMap(PdfDoc* doc);
  ~CClassMap() { }

  string get_css_style_content();
  unordered_map<string, string> get_html_attributes(const string& key) const;

private:
  cm_attribute_map m_html_attrs;
  cm_attribute_map m_css_attrs;

  void process_table_owner(PdsDictionary* attr_dict, const string& key);
  void process_layout_owner(PdsDictionary* attr_dict, const string& key);
  void process_html_or_css_owner(PdsDictionary* attr_dict, bool html_owner, const string& key);

  double userSpace2DeviceSpace(double s);
  array<double, 4> userSpace2DeviceSpace(const array<double, 4> input);
};

#endif // tagged_pdf_classmap_h
