////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_figure_h
#define tagged_pdf_figure_h

#include "tagged_pdf_element.h"

class CTaggedPdfFigure : public CTaggedPdfElement
{
public:
  CTaggedPdfFigure(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfFigure();

  virtual void tag_begin();
  virtual void tag_end();
  virtual void tag_substructure();
private:
  bool m_use_figure_elem;

  string render(PdsStructElement* elem);
  string render_now();
  void expand_rect(PdfRect& other_rect);

  PdfRect m_obj_rect;
};

#endif // !tagged_pdf_figure_h
