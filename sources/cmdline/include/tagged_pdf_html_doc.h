////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_html_doc_h
#define tagged_pdf_html_doc_h

#include <cstring>
#include <memory>
#include "tagged_utils.h"
#include "derived_output_writer.h"
#include "tagged_pdf_classmap.h"

using std::unique_ptr;
using namespace PDFixSDK;

typedef struct _PdfTaggedParams {
  int width;                           /*!< Rendered width of the page in pixels (defies rendering output quality). */
  PdfImageParams image_params;         /*!< Image params specifying format and quality of the images used for the conversion. */
  _PdfTaggedParams() {
    width = 1200;
  }
} PdfTaggedParams;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Class CTaggedPdfHtmlDoc
////////////////////////////////////////////////////////////////////////////////////////////////////
class CTaggedPdfHtmlDoc {
public:
  struct Page {
    PdfPage* m_page = nullptr;
    int m_page_num = -1;
  };

  static Page m_actual_page;

  // Constructs html document from PDFDoc object
  CTaggedPdfHtmlDoc(PdfDoc* doc);
  // Close PDF documents
  ~CTaggedPdfHtmlDoc();

  void setup_html_end();
  void process_class_map(PdsStructTree* tree);
  void process_css(PdsClassMap* class_map, std::wstring prefix_string);
  void process_content(PdsStructElement* elem, PdsStream* stream = nullptr);
  void save(const std::wstring& path);

private:
  PdfDoc* m_doc;                    // PDF document object
  PdfTaggedParams m_custom_params;  // conversion custom params
  std::wstring m_path;              // output path set when saving html to file not stream
  CDerivedOutputWriter* m_writer = nullptr;
  unique_ptr<CClassMap> m_classmap = nullptr;

  bool has_form() const { return m_doc->GetNumFormFields() > 0; }
};

#endif
