////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)

#include "tagged_pdf_element_factory.h"
#include "tagged_pdf_link.h"
#include "tagged_pdf_caption.h"
#include "tagged_pdf_html_ns.h"
#include "tagged_pdf_heading.h"
#include "tagged_pdf_figure.h"
#include "tagged_pdf_form.h"
#include "tagged_pdf_list.h"
#include "tagged_pdf_listitem.h"
#include "tagged_pdf_label.h"
#include "tagged_pdf_utils.h"
#include "tagged_pdf_lbody.h"

#include <utility>
#include <regex>

CTaggedPdfElementFactory::CTaggedPdfElementFactory() {

}

CTaggedPdfElementFactory::~CTaggedPdfElementFactory() {

}

unique_ptr<CTaggedPdfElement> CTaggedPdfElementFactory::make_tagged_structure_element(PdfDoc* doc, PdsStructElement* elem, CTaggedPdfElementConfig& config) {
  wstring elem_type = CTaggedPdfUtils::StructElementGetType(elem);

  auto is_heading_elem = [](const wstring& elem_type) -> bool {
    if (elem_type == L"H")
      return true;

    std::wregex r(L"H\\d{1,}");
    if (std::regex_match(elem_type, r))
      return true;

    return false;
  };

  //check namespace
  std::string ns("");
  PdsDictionary* elem_dict = static_cast<PdsDictionary*> (elem->GetElementObject());
  if (elem_dict) {
    PdsDictionary* ns_dict = elem_dict->GetDictionary(L"NS");
    if (ns_dict) {
      ns.resize(ns_dict->GetString(L"NS", nullptr, 0));
      ns_dict->GetString(L"NS", (char*)ns.c_str(), static_cast<int> (ns.size()));
    }
  }

  if (ns == "http://www.w3.org/1999/xhtml") {
    return unique_ptr<CTaggedPdfHTMLNS>(new CTaggedPdfHTMLNS(doc, elem, config));
  }

  if (elem_type == L"Link" || elem_type == L"Reference") {
    return unique_ptr<CTaggedPdfLink>(new CTaggedPdfLink(doc, elem, config));
  }
  else if (elem_type == L"Caption") {
    return unique_ptr<CTaggedPdfCaption>(new CTaggedPdfCaption(doc, elem, config));
  }
  else if (elem_type == L"Figure" || elem_type == L"Formula") {
    return unique_ptr<CTaggedPdfFigure>(new CTaggedPdfFigure(doc, elem, config));
  }
  else if (elem_type == L"Form") {
    return unique_ptr<CTaggedPdfForm>(new CTaggedPdfForm(doc, elem, config));
  }
  else if (is_heading_elem(elem_type)) {
    return unique_ptr<CTaggedPdfHeading>(new CTaggedPdfHeading(doc, elem, config));
  }
  else if (elem_type == L"L") {
    return unique_ptr<CTaggedPdfList>(new CTaggedPdfList(doc, elem, config));
  }
  else if (elem_type == L"LI") {
    return unique_ptr<CTaggedPdfListItem>(new CTaggedPdfListItem(doc, elem, config));
  }
  else if (elem_type == L"Lbl") {
    return unique_ptr<CTaggedPdfLabel>(new CTaggedPdfLabel(doc, elem, config));
  }
  else if (elem_type == L"LBody") {
    return unique_ptr<CTaggedPdfLBody>(new CTaggedPdfLBody(doc, elem, config));
  }
  else {
    return unique_ptr<CTaggedPdfElement>(new CTaggedPdfElement(doc, elem, config));
  }

  return nullptr;
}
