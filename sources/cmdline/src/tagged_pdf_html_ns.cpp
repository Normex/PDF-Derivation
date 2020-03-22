////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_html_ns.h"
#include "tagged_utils.h"
#include "tagged_pdf_utils.h"
#include <sstream>
#include <cassert>

// ctor
CTaggedPdfHTMLNS::CTaggedPdfHTMLNS(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  m_html_tag = w2utf8(m_elem_type);
  if (m_elem_type == L"body")
    m_html_tag = "div";
}

// dtor
CTaggedPdfHTMLNS::~CTaggedPdfHTMLNS() {
}

void CTaggedPdfHTMLNS::tag_begin() {
  if (m_html_tag == "a")
    m_writer->push_html("<" + m_html_tag + " href=" + CTaggedPdfUtils::StructElementGetAction(m_elem_dict) + " " + tag_attributes() + ">\n");
  else
    m_writer->push_html("<" + m_html_tag + " " + tag_attributes() + ">\n");
}

