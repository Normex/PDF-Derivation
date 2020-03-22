////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_attr_dict_h
#define tagged_pdf_attr_dict_h

#include "Pdfix.h"

#include <string>
#include <unordered_map>
#include <array>

using std::string;
using std::unordered_map;
using std::array;
using namespace PDFixSDK;


using attribute_map = unordered_map<string, string>;

class CAttributeDictProcessor final
{
private:
  CAttributeDictProcessor();

public:
  ~CAttributeDictProcessor();

  static void process_table_owner(PdsDictionary* attr_dict, attribute_map& out_html_attrs, attribute_map& out_css_attrs, std::wstring type);
  static void process_layout_owner(PdsDictionary* attr_dict, attribute_map& out_html_attrs, attribute_map& out_css_attrs);
  static void process_html_or_css_owner(PdsDictionary* attr_dict, bool html_owner, attribute_map& out_html_attrs, attribute_map& out_css_attrs);

  static double userSpace2DeviceSpace(double s);
  static array<double, 4> userSpace2DeviceSpace(const array<double, 4> input);
};

#endif // tagged_pdf_attr_dict_h
