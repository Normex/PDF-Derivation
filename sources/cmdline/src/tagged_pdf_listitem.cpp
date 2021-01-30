////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_listitem.h"
#include "tagged_pdf_list.h"
#include <memory>
#include <sstream>

using std::unique_ptr;
using std::stringstream;

// ctor
CTaggedPdfListItem::CTaggedPdfListItem(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  // determine type of List
  PdsObject* parent_obj = m_elem->GetParentObject();
  PdsStructTree* struct_tree = m_doc->GetStructTree();
  PdsStructElement* parent_elem = struct_tree->AcquireStructElement(parent_obj);

  if (!parent_elem)
    throw std::runtime_error("Parent element not acquired");

  unique_ptr<CTaggedPdfList> list(new CTaggedPdfList(m_doc, parent_elem, m_config));
  if (list->get_list_type() == CTaggedPdfList::ListType::kDescriptionList)
    m_html_tag = "div";
  else
    m_html_tag = "li";
}

// dotr
CTaggedPdfListItem::~CTaggedPdfListItem() {
}

