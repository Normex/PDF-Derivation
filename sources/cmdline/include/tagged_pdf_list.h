////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_list_h
#define tagged_pdf_list_h

#include "tagged_pdf_element.h"

class CTaggedPdfList : public CTaggedPdfElement
{
public:
  enum class ListType {
    kUnorderedList,
    kOrderedList,
    kDescriptionList
  };


  CTaggedPdfList(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfList();
  virtual void tag_begin();
  virtual void tag_end();


  ListType get_list_type() const { return m_list_type; }
  bool is_ordered(const std::string &list_numbering);

private:
  ListType m_list_type = ListType::kUnorderedList;
  bool m_insert_additional_li;
};

#endif // tagged_pdf_list_h
