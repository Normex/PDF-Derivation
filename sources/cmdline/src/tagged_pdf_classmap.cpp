////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_classmap.h"
#include "tagged_pdf_attr_dict.h"
#include "tagged_utils.h"
#include "tagged_pdf_utils.h"

#include <string>
#include <sstream>

using std::wstring;
using std::stringstream;

#undef GetObject

// ctor
CClassMap::CClassMap(PdfDoc* doc) {
  PdsStructTree* st = doc->GetStructTree();
  if (!st)
    throw std::runtime_error("Input pdf file is not tagged");

  PdsDictionary* st_obj = static_cast<PdsDictionary*> (st->GetObject());

  if (st_obj->Known(L"ClassMap")) {
    PdsDictionary* class_map = static_cast<PdsDictionary*> (st_obj->GetDictionary(L"ClassMap"));

    for (int i = 0; i < class_map->GetNumKeys(); ++i) {
      wstring key = CTaggedPdfUtils::PdfDictionaryGetKeyAtIndex(class_map, i);
      PdsObject* obj = class_map->Get(key.c_str());

      if (obj->GetObjectType() == kPdsDictionary) {
        PdsDictionary* attr_dict = static_cast<PdsDictionary*> (obj);

        string o;
        o.resize(attr_dict->GetString(L"O", nullptr, 0));
        attr_dict->GetString(L"O", const_cast<char*> (o.c_str()), static_cast<int> (o.size()));

        if (o == "Layout")
          process_layout_owner(attr_dict, w2utf8(key));
        else if (o == "List")
          ; // Done in List processing
        else if (o == "Table")
          process_table_owner(attr_dict, w2utf8(key));
        else if (o.find("HTML") == 0)
          process_html_or_css_owner(attr_dict, true, w2utf8(key));
        else if (o.find("CSS") == 0)
          process_html_or_css_owner(attr_dict, false, w2utf8(key));
        else if (o.find("ARIA") == 0)
          process_html_or_css_owner(attr_dict, true, w2utf8(key)); // process as HTML attributes
        else
          continue;
      }
      else if (obj->GetObjectType() == kPdsArray) {
        PdsArray* ar = static_cast<PdsArray*> (obj);

        for (int i = 0; i < ar->GetNumObjects(); ++i) {
          PdsDictionary* attr_dict = ar->GetDictionary(i);

          if (!attr_dict)
            continue;

          string o;
          o.resize(attr_dict->GetString(L"O", nullptr, 0));
          attr_dict->GetString(L"O", const_cast<char*> (o.c_str()), static_cast<int> (o.size()));

          if (o == "Layout")
            process_layout_owner(attr_dict, w2utf8(key));
          else if (o == "List")
            ; // Done in List processing
          else if (o == "Table")
            process_table_owner(attr_dict, w2utf8(key));
          else if (o.find("HTML") == 0)
            process_html_or_css_owner(attr_dict, true, w2utf8(key));
          else if (o.find("CSS") == 0)
            process_html_or_css_owner(attr_dict, false, w2utf8(key));
          else if (o.find("ARIA") == 0)
            process_html_or_css_owner(attr_dict, true, w2utf8(key)); // process as HTML attributes
          else
            continue;
        }
      }
      else
        continue;
    }
  }
}

// methods
string CClassMap::get_css_style_content() {
  stringstream css;

  for (const auto& css_item : m_css_attrs) {
    css << "." << css_item.first << " {\n";
    for (const auto& css_entry : css_item.second) {
      css << css_entry.first << ": " << css_entry.second << ";\n";
    }
    css << "}\n\n";
  }
  return css.str();
}

unordered_map<string, string> CClassMap::get_html_attributes(const string& key) const {
  const auto& it = m_html_attrs.find(key);
  if (it != m_html_attrs.end())
    return it->second;
  return unordered_map<string, string> { };
}

void CClassMap::process_table_owner(PdsDictionary* attr_dict, const string& key) {
  attribute_map html_attrs;
  attribute_map css_attrs;

  CAttributeDictProcessor::process_table_owner(attr_dict, html_attrs, css_attrs, NULL);
  if (!html_attrs.empty())
    m_html_attrs.insert({ key, html_attrs });
  m_css_attrs.insert({ key, css_attrs });
}

void CClassMap::process_layout_owner(PdsDictionary* attr_dict, const string& key) {
  attribute_map html_attrs;
  attribute_map css_attrs;

  CAttributeDictProcessor::process_layout_owner(attr_dict, html_attrs, css_attrs);
  if (!html_attrs.empty())
    m_html_attrs.insert({ key, html_attrs });
  m_css_attrs.insert({ key, css_attrs });
}

void CClassMap::process_html_or_css_owner(PdsDictionary* attr_dict, bool html_owner, const string& key) {
  attribute_map html_attrs;
  attribute_map css_attrs;

  CAttributeDictProcessor::process_html_or_css_owner(attr_dict, html_owner, html_attrs, css_attrs);
  if (!html_attrs.empty())
    m_html_attrs.insert({ key, html_attrs });
  m_css_attrs.insert({ key, css_attrs });
}

double CClassMap::userSpace2DeviceSpace(double s) {
  return s;
}

array<double, 4> CClassMap::userSpace2DeviceSpace(array<double, 4> input) {
  return input;
}
