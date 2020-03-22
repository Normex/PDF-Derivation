////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_attributes.h"
#include "tagged_utils.h"
#include "tagged_pdf_utils.h"

#include <string>
#include <sstream>

using std::wstring;
using std::stringstream;

// ctor
CTaggedPdfAttributes::CTaggedPdfAttributes(PdfDoc* doc, PdsStructElement* elem, CClassMap* classmap)
  : m_doc(doc),
  m_elem(elem),
  m_classmap(classmap) {
  process_attributes(); // SE elements (/A entry and collect classes from /C)
}

// dtor
CTaggedPdfAttributes::~CTaggedPdfAttributes() {

}

// methods
string CTaggedPdfAttributes::attributes() const {
  stringstream attrs;

  //html attributes
  for (const auto& html_attr : m_html_attrs) {
    attrs << html_attr.first << "=\"" << html_attr.second << "\" ";
  }

  // inline css attributes
  stringstream css_attrs;
  for (const auto& css_attr : m_css_attrs) {
    css_attrs << css_attr.first << ": " << css_attr.second << "; ";
  }

  string css_inline = css_attrs.str();
  if (!css_inline.empty()) {
    attrs << "style=\"" << css_inline << "\" ";
  }

  if (!m_css_classes.empty()) {
    attrs << "class=\"";

    size_t classes_count = m_css_classes.size();
    for (int i = 0; i < classes_count; ++i) {
      attrs << m_css_classes[i];

      if (i < (classes_count - 1))
        attrs << " ";
    }

    attrs << "\" ";
  }

  return attrs.str();
}

void CTaggedPdfAttributes::process_attributes() {
  // attributes from /C (collect classes)
  PdsDictionary* elem_dict = static_cast<PdsDictionary*> (m_elem->GetElementObject());
  if (elem_dict->Known(L"C")) {
    PdsObject* c_obj = elem_dict->Get(L"C");

    if (c_obj->GetObjectType() == kPdsArray) {
      PdsArray* c_array = static_cast<PdsArray*> (c_obj);
      for (int i = 0; i < c_array->GetNumObjects(); ++i) {
        wstring class_name = CTaggedPdfUtils::PdfArrayGetTextAtIndex(c_array, i);

        if (!class_name.empty()) {
          m_css_classes.push_back(w2utf8(class_name));

          // load HTML attributes, if given attribute exists in /A then
          // it will replaced by /A entry (because of map)
          auto html_attrs = m_classmap->get_html_attributes(w2utf8(class_name));
          if (!html_attrs.empty())
            m_html_attrs.insert(html_attrs.begin(), html_attrs.end());
        }
      }
    }
    else if (c_obj->GetObjectType() == kPdsName) {
      wstring class_name = CTaggedPdfUtils::PdfNameGetText(static_cast<PdsName*> (c_obj));
      m_css_classes.push_back(w2utf8(class_name));

      // load HTML attributes, if given attribute exists in /A then
      // it will replaced by /A entry (because of map)
      auto html_attrs = m_classmap->get_html_attributes(w2utf8(class_name));
      if (!html_attrs.empty())
        m_html_attrs.insert(html_attrs.begin(), html_attrs.end());
    }
    else {
      // todo: log or throw exception
    }
  }

  wstring type = CTaggedPdfUtils::StructElementGetType(m_elem);

  // attributes form /A
  for (int i = 0; i < m_elem->GetNumAttrObjects(); ++i) {
    PdsObject * attr_obj = m_elem->GetAttrObject(i);

    if (attr_obj->GetObjectType() != kPdsDictionary)
      continue;

    PdsDictionary* attr_dict = static_cast<PdsDictionary*> (attr_obj);

    string value;
    value.resize(attr_dict->GetString(L"O", nullptr, 0));
    attr_dict->GetString(L"O", const_cast<char*> (value.c_str()), static_cast<int> (value.size()));

    if (value == "Layout")
      process_layout_owner(attr_dict);
    else if (value == "List")
      ; // Done in List processing
    else if (value == "Table")
      process_table_owner(attr_dict, type);
    else if (value.find("HTML") == 0)
      process_html_or_css_owner(attr_dict, true);
    else if (value.find("CSS") == 0)
      process_html_or_css_owner(attr_dict, false);
    else if (value.find("ARIA") == 0)
      process_html_or_css_owner(attr_dict, true); // process as HTML attributes
    else
      continue;
  }
}

void CTaggedPdfAttributes::process_table_owner(PdsDictionary* attr_dict, wstring type) {
  CAttributeDictProcessor::process_table_owner(attr_dict, m_html_attrs, m_css_attrs, type);
}

void CTaggedPdfAttributes::process_layout_owner(PdsDictionary* attr_dict) {
  CAttributeDictProcessor::process_layout_owner(attr_dict, m_html_attrs, m_css_attrs);
}

void CTaggedPdfAttributes::process_html_or_css_owner(PdsDictionary* attr_dict, bool html_owner) {
  CAttributeDictProcessor::process_html_or_css_owner(attr_dict, html_owner, m_html_attrs, m_css_attrs);
}
