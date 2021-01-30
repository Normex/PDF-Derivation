////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_element_factory_h
#define tagged_pdf_element_factory_h

#include <string>
#include <memory>
#include "tagged_pdf_element.h"

using std::string;
using std::wstring;
using std::unique_ptr;

class CTaggedPdfElementFactory final
{
private:
  CTaggedPdfElementFactory();

public:
  ~CTaggedPdfElementFactory();

  // factory method
  static unique_ptr<CTaggedPdfElement> make_tagged_structure_element(PdfDoc* doc, PdsStructElement* elem, CTaggedPdfElementConfig& config);
};

#endif // !tagged_pdf_element_factory_h
