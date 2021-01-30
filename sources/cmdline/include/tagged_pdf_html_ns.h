////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_html_ns_h
#define tagged_pdf_html_ns_h

#include "tagged_pdf_element.h"

class CTaggedPdfHTMLNS : public CTaggedPdfElement
{
public:
  CTaggedPdfHTMLNS(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfHTMLNS();

  virtual void tag_begin();
protected:
};

#endif // tagged_pdf_html_ns_h
