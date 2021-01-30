////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_element_h
#define tagged_pdf_element_h

#include "Pdfix.h"

#include "derived_output_writer.h"
#include "tagged_pdf_attributes.h"
#include "tagged_pdf_html_doc.h"

#include <string>
#include <sstream>
#include <map>

using std::string;
using std::wstring;
using std::stringstream;

using PdfElementToHtmlTagMap = std::map<wstring, string>;

class CPdfElementToHtmlTagMapper {
private:
  PdfElementToHtmlTagMap m_elem_to_tag;

  CPdfElementToHtmlTagMapper();
  CPdfElementToHtmlTagMapper(const CPdfElementToHtmlTagMapper& m) = delete;
  CPdfElementToHtmlTagMapper& operator=(const CPdfElementToHtmlTagMapper& m) = delete;

public:
  ~CPdfElementToHtmlTagMapper() = default;

  static CPdfElementToHtmlTagMapper& getInstance();

  string GetHtmlTag(const wstring& elem_type) const;
};

struct CTaggedPdfElementConfig final {
  wstring data_path;
  string id;
  CDerivedOutputWriter* writer = nullptr;
  CClassMap* classmap = nullptr;
};

/*
 the process of HTML generation is as follows:
 ctr (set the m_html_tag)

 call virtual functions in this order:

  -> tag_element
        -> tag_begin
          -> tag_attributes
      -> tag_substructure
      -> tag_end (uzavrie m_html_tag)
*/
class CTaggedPdfElement
{
public:
  CTaggedPdfElement(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  virtual ~CTaggedPdfElement();
  virtual void tag_element();
  virtual void tag_substructure();
  virtual void tag_begin();
  virtual void tag_end();

  virtual string process_text(PdsText* obj);
  virtual string process_image(PdfPage* page, PdsImage* obj);
  virtual string process_pagemap_image(PdfPage* page, PdsPageObject* obj);
  virtual string process_path(PdfPage* page, PdsPath* obj);
  virtual string process_shading(PdfPage* page, PdsShading* obj);

protected:
  enum class AFSubType {
    MATHML,
    SVG,
    IMG,
    CSS,
    JS,
    HTML
  };

  virtual string tag_attributes();
  bool process_af(PdsDictionary* af_dict);

  PdfDoc* m_doc = nullptr;
  PdsStructElement* m_elem = nullptr;
  PdsDictionary* m_elem_dict = nullptr;
  CTaggedPdfElementConfig m_config;
  wstring m_elem_type;
  string m_html_tag;
  CDerivedOutputWriter* m_writer = nullptr;
  bool m_is_form = false;
  static bool m_math_script_included;
};

#endif // tagged_pdf_element_h
