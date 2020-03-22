////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_lbody_h
#define tagged_pdf_lbody_h

#include "tagged_pdf_element.h"

class CTaggedPdfLBody : public CTaggedPdfElement
{
public:
  CTaggedPdfLBody(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfLBody();

  virtual void tag_begin();
};

#endif // tagged_pdf_lbody_h
