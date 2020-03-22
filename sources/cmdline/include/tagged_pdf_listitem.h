////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_listitem_h
#define tagged_pdf_listitem_h

#include "tagged_pdf_element.h"

class CTaggedPdfListItem : public CTaggedPdfElement
{
public:
  CTaggedPdfListItem(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfListItem();
};

#endif // tagged_pdf_listitem_h
