////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_label_h
#define tagged_pdf_label_h

#include "tagged_pdf_element.h"

class CTaggedPdfLabel : public CTaggedPdfElement
{
public:
  CTaggedPdfLabel(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfLabel();

private:
  bool process_as_list();
  bool process_as_form();
};

#endif // !tagged_pdf_label_h
