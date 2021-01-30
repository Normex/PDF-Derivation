////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_caption_h
#define tagged_pdf_caption_h

#include "tagged_pdf_element.h"

class CTaggedPdfCaption : public CTaggedPdfElement
{
private:

public:
  CTaggedPdfCaption(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfCaption();

protected:
};

#endif // tagged_pdf_caption_h
