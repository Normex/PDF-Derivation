////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_link_h
#define tagged_pdf_link_h

#include "tagged_pdf_element.h"

class CTaggedPdfLink : public CTaggedPdfElement
{
public:
  CTaggedPdfLink(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfLink();

  virtual void tag_begin();
};

#endif // tagged_pdf_link_h
