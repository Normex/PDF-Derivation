////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_link.h"
#include "tagged_utils.h"
#include "tagged_pdf_utils.h"

#include <sstream>

using std::stringstream;

CTaggedPdfLink::CTaggedPdfLink(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  m_html_tag = "a";
}

CTaggedPdfLink::~CTaggedPdfLink() {
}

void CTaggedPdfLink::tag_begin() {
  m_writer->push_html("<" + m_html_tag + " href=" + CTaggedPdfUtils::StructElementGetAction(m_elem_dict) + " " + tag_attributes() + ">\n");
}
