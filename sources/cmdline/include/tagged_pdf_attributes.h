////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_attributes_h
#define tagged_pdf_attributes_h

#include "Pdfix.h"

#include "tagged_pdf_attr_dict.h"
#include "tagged_pdf_classmap.h"

#include <string>
#include <array>
#include <map>
#include <vector>
#include <unordered_map>

using std::string;
using std::wstring;
using std::array;
using std::unordered_map;
using std::vector;

class CTaggedPdfAttributes
{
public:
  CTaggedPdfAttributes(PdfDoc* doc, PdsStructElement* elem, CClassMap* classmap);
  ~CTaggedPdfAttributes();

  string attributes() const;

private:
  attribute_map m_html_attrs;
  attribute_map m_css_attrs;
  vector<string> m_css_classes;
  PdfDoc* m_doc;
  PdsStructElement* m_elem;
  CClassMap* m_classmap;

  void process_attributes();

  void process_table_owner(PdsDictionary* attr_dict, wstring type);
  void process_layout_owner(PdsDictionary* attr_dict);
  void process_html_or_css_owner(PdsDictionary* attr_dict, bool html_owner);
};

#endif // !tagged_pdf_attributes_h
