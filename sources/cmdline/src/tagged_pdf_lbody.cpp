////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_lbody.h"
#include "tagged_pdf_list.h"
#include "tagged_pdf_utils.h"

#include <memory>

using std::unique_ptr;

// ctor
CTaggedPdfLBody::CTaggedPdfLBody(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  // default
  m_html_tag = "div";

  // Check if LBody belongs to List
  // 4.3.5.5.2 L as description list (LBody -> dd)
  PdsObject* parent_obj = m_elem->GetParentObject();
  if (!parent_obj)
    return;

  PdsStructTree* struct_tree = m_doc->GetStructTree();
  PdsStructElement* li_elem = struct_tree->AcquireStructElement(parent_obj);

  if (!li_elem)
    return;

  wstring type = CTaggedPdfUtils::StructElementGetType(li_elem);
  if (type != L"LI")
    return;

  PdsObject* list_obj = li_elem->GetParentObject();
  if (!list_obj)
    return;

  PdsStructElement* list_elem = struct_tree->AcquireStructElement(list_obj);
  if (!list_elem)
    return;

  type = CTaggedPdfUtils::StructElementGetType(list_elem);
  if (type != L"L")
    return;

  unique_ptr<CTaggedPdfList> list(new CTaggedPdfList(m_doc, list_elem, m_config));
  if (list->get_list_type() == CTaggedPdfList::ListType::kDescriptionList)
    m_html_tag = "dd";
}

// dtor
CTaggedPdfLBody::~CTaggedPdfLBody() {
}

void CTaggedPdfLBody::tag_begin() {
  std::string attr = tag_attributes();
  std::stringstream attrs("style=\"display:inline;\" ");
  merge_attributes(attrs, attr);
  m_writer->push_html("<" + m_html_tag + " " + attrs.str() + ">\n");
}
