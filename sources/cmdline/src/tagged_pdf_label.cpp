////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_label.h"
#include "tagged_pdf_list.h"
#include "tagged_pdf_utils.h"

#include <sstream>
#include <memory>

using std::stringstream;
using std::unique_ptr;

// ctor
CTaggedPdfLabel::CTaggedPdfLabel(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  bool is_processed = process_as_list();
  if (!is_processed)
    is_processed = process_as_form();

  if (!is_processed)
    m_html_tag = "label";
}

// dtor
CTaggedPdfLabel::~CTaggedPdfLabel() {
}

// methods
bool CTaggedPdfLabel::process_as_list() {
  PdsObject* parent_obj = m_elem->GetParentObject();
  PdsStructTree* struct_tree = m_doc->GetStructTree();
  PdsStructElement* parent_elem = struct_tree->AcquireStructElement(parent_obj);

  wstring type = CTaggedPdfUtils::StructElementGetType(parent_elem);

  if (type != L"LI")
    return false;

  // check if Lbl is 1st child of LI (stored in parent_elem)
  if (parent_elem->GetNumKids() > 0 && parent_elem->GetKidType(0) == kPdsStructKidElement) {
    PdsObject* kid_obj = parent_elem->GetKidObject(0);
    PdsStructElement* kid_elem = struct_tree->AcquireStructElement(kid_obj);

    type = CTaggedPdfUtils::StructElementGetType(kid_elem);
    if (type != L"Lbl")
      return false;
  }

  parent_obj = parent_elem->GetParentObject();
  parent_elem = struct_tree->AcquireStructElement(parent_obj);

  type = CTaggedPdfUtils::StructElementGetType(parent_elem);
  if (type != L"L")
    return false;

  unique_ptr<CTaggedPdfList> list(new CTaggedPdfList(m_doc, parent_elem, m_config));
  auto list_type = list->get_list_type();

  if (list_type != CTaggedPdfList::ListType::kDescriptionList) {
    bool is_single = true;
    for (int i = 0; i < m_elem->GetNumKids(); ++i) {
      if (m_elem->GetKidType(i) == kPdsStructKidElement) {
        is_single = false;
        break;
      }
    }

    if (is_single)
      m_html_tag = "span";
    else
      m_html_tag = "div";
  }
  else {
    m_html_tag = "dt";
  }

  return true;
}

bool CTaggedPdfLabel::process_as_form() {
  PdsObject* parent_obj = m_elem->GetParentObject();
  PdsStructTree* struct_tree = m_doc->GetStructTree();
  PdsStructElement* parent_elem = struct_tree->AcquireStructElement(parent_obj);

  wstring type = CTaggedPdfUtils::StructElementGetType(parent_elem);

  if (type != L"Form")
    return false;

  for (int i = 0; i < m_elem->GetNumKids(); ++i) {
    if (m_elem->GetKidType(i) == kPdsStructKidElement) {
      PdsObject* kid_obj = m_elem->GetKidObject(i);
      PdsStructElement* kid_elem = struct_tree->AcquireStructElement(kid_obj);

      type = CTaggedPdfUtils::StructElementGetType(kid_elem);
      if (type == L"Form" || type == L"Figure" || type == L"Formula" || type == L"Caption") {
        m_html_tag = "div";
        return true;
      }
    }
  }
  m_html_tag = "label";
  return true;
}
