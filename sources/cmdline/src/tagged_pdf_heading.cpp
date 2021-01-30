////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_heading.h"
#include "tagged_pdf_utils.h"
#include <sstream>
#include <regex>

// ctor
CTaggedPdfHeading::CTaggedPdfHeading(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  if (m_elem_type == L"H")
    m_html_tag = "h1";
  else {
    int heading_level = 0;
    std::wregex r(L"H(\\d{1,})");
    std::wsmatch match;

    if (std::regex_search(m_elem_type, match, r) && match.size() > 1)
      heading_level = std::stoi(match.str(1));
    else
      ;//todo: throw excpetion, heading is not in correct format

    if (heading_level > 0 && heading_level < 7)
      m_html_tag = "h" + std::to_string(heading_level);
    else if (heading_level >= 7)
      m_html_tag = "p";
  }

  //check parent. If TH - then convert to p
  PdsObject* parent_obj = m_elem->GetParentObject();
  if (parent_obj) {
    PdsStructTree* structure_tree = m_doc->GetStructTree();
    wstring type;
    if (structure_tree) {
      PdsStructElement* parent_elem = structure_tree->AcquireStructElement(parent_obj);
      type = CTaggedPdfUtils::StructElementGetType(parent_elem);
      if (type == L"TH")
        m_html_tag = "p";
    }
  }
}

// dtor
CTaggedPdfHeading::~CTaggedPdfHeading() {
}
