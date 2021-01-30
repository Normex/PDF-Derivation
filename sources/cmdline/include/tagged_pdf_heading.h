////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_heading_h
#define tagged_pdf_heading_h

#include "tagged_pdf_element.h"

class CTaggedPdfHeading : public CTaggedPdfElement
{
public:
  CTaggedPdfHeading(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfHeading();

private:
};

#endif // !tagged_pdf_heading_h
