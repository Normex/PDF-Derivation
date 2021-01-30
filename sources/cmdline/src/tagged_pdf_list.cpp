////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_list.h"
#include "tagged_pdf_utils.h"

// ctor
CTaggedPdfList::CTaggedPdfList(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  m_html_tag = "ul";
  m_insert_additional_li = false;
  m_list_type = ListType::kUnorderedList;

  //check parent. If "L" we'd need to insert <li>
  PdsObject* parent_obj = m_elem->GetParentObject();
  if (parent_obj) {
    PdsStructTree* structure_tree = m_doc->GetStructTree();
    wstring type;
    if (structure_tree) {
      PdsStructElement* parent_elem = structure_tree->AcquireStructElement(parent_obj);
      type = CTaggedPdfUtils::StructElementGetType(parent_elem);
      m_insert_additional_li = type == L"L";
    }
  }

  int attrs_count = m_elem->GetNumAttrObjects();
  for (int i = 0; i < attrs_count; ++i) {
    PdsObject* attr_obj = m_elem->GetAttrObject(i);

    // now assume attr obj is dict (it can be array (attr rev_num), or stream) todo: fix it later
    //todo: Classmap, array 
    if (attr_obj->GetObjectType() != kPdsDictionary)
      continue;

    PdsDictionary* attr_dict = static_cast<PdsDictionary*> (attr_obj);
    if (attr_dict->Known(L"ListNumbering")) {
      string value;
      value.resize(attr_dict->GetString(L"ListNumbering", nullptr, 0));
      attr_dict->GetString(L"ListNumbering", const_cast<char*> (value.c_str()), static_cast<int> (value.size()));

      if (is_ordered(value)) {
        m_html_tag = "ol";
        m_list_type = ListType::kOrderedList;
      }
      if (value == "Description") {
        m_html_tag = "dl";
        m_list_type = ListType::kDescriptionList;
      }
    }
  }
}

// dtor
CTaggedPdfList::~CTaggedPdfList() {
}

void CTaggedPdfList::tag_begin() {
  std::string attr = tag_attributes();
  //  if (m_list_type == ListType::kUnorderedList) 
  std::stringstream attrs("style=\"list-style-type:none;\" ");
  merge_attributes(attrs, attr);
  if (m_insert_additional_li)
    m_writer->push_html("<li data-pdf-se-type=\"NONE\">\n");
  m_writer->push_html("<" + m_html_tag + " " + attrs.str() + ">\n");
}

void CTaggedPdfList::tag_end() {
  m_writer->push_html("</" + m_html_tag + ">\n");
  if (m_insert_additional_li)
    m_writer->push_html("</li>\n");
}


bool CTaggedPdfList::is_ordered(const std::string &list_numbering)
{
  const char *ordered_attrs[] = { "Ordered", "Decimal", "UpperRoman", "LowerRoman", "UpperAlpha", "LowerAlpha" };
  //"None", "Unordered", "Disc", "Circle", "Square"
  for (auto id : ordered_attrs)
    if (list_numbering == id) return true;
  return false;
}
