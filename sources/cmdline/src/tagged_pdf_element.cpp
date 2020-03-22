////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_element.h"
#include "tagged_pdf_element_factory.h"
#include "tagged_pdf_attributes.h"
#include "tagged_utils.h"
#include "tagged_pdf_utils.h"

#include <sstream>
#include <cassert>
#include <memory>
#include <regex>
#include <vector>

using std::stringstream;
using std::unique_ptr;
using std::vector;
using namespace PDFixSDK;

bool CTaggedPdfElement::m_math_script_included = false;

//PdfElementToHtmlTagMapper
CPdfElementToHtmlTagMapper::CPdfElementToHtmlTagMapper() {
  m_elem_to_tag[L"Art"] = "article";
  m_elem_to_tag[L"Aside"] = "aside";
  m_elem_to_tag[L"BibEntry"] = "p";
  m_elem_to_tag[L"BlockQuote"] = "blockquote";
  m_elem_to_tag[L"Code"] = "code";
  m_elem_to_tag[L"Document"] = "div";
  m_elem_to_tag[L"DocumentFragment"] = "div";
  m_elem_to_tag[L"Div"] = "div";
  m_elem_to_tag[L"Em"] = "em";
  m_elem_to_tag[L"FENote"] = "div";
  m_elem_to_tag[L"Index"] = "section";
  m_elem_to_tag[L"Note"] = "p";
  m_elem_to_tag[L"P"] = "p";
  m_elem_to_tag[L"Part"] = "div";
  m_elem_to_tag[L"Quote"] = "q";
  m_elem_to_tag[L"RB"] = "rb";
  m_elem_to_tag[L"RP"] = "rp";
  m_elem_to_tag[L"RT"] = "rt";
  m_elem_to_tag[L"Ruby"] = "ruby";
  m_elem_to_tag[L"Sect"] = "section";
  m_elem_to_tag[L"Span"] = "span";
  m_elem_to_tag[L"Strong"] = "strong";
  m_elem_to_tag[L"Sub"] = "span";
  m_elem_to_tag[L"Table"] = "table";
  m_elem_to_tag[L"TBody"] = "tbody";
  m_elem_to_tag[L"TD"] = "td";
  m_elem_to_tag[L"TFoot"] = "tfoot";
  m_elem_to_tag[L"TH"] = "th";
  m_elem_to_tag[L"THead"] = "thead";
  m_elem_to_tag[L"Title"] = "div";
  m_elem_to_tag[L"TOC"] = "ol";
  m_elem_to_tag[L"TOCI"] = "li";
  m_elem_to_tag[L"TR"] = "tr";
  m_elem_to_tag[L"Warichu"] = "span";
  m_elem_to_tag[L"WT"] = "span";
  m_elem_to_tag[L"WP"] = "span";

  //todo: unsuported Annot
  m_elem_to_tag[L"Annot"] = "div";
}

CPdfElementToHtmlTagMapper& CPdfElementToHtmlTagMapper::getInstance() {
  static CPdfElementToHtmlTagMapper mapper;
  return mapper;
}

string CPdfElementToHtmlTagMapper::GetHtmlTag(const wstring& elem_type) const {
  auto value = m_elem_to_tag.find(elem_type);
  if (value != m_elem_to_tag.end())
    return value->second;

  return "";
}

// ctor
CTaggedPdfElement::CTaggedPdfElement(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) :
  m_doc(doc),
  m_elem(elem),
  m_config(config),
  m_writer(config.writer) {
  assert(m_doc);
  assert(m_elem);

  if (m_elem->GetElementObject()->GetObjectType() != kPdsDictionary)
    throw std::exception("Dictionary required");
  m_elem_dict = static_cast<PdsDictionary*> (elem->GetElementObject());
  m_elem_type = CTaggedPdfUtils::StructElementGetType(elem);
  m_html_tag = CPdfElementToHtmlTagMapper::getInstance().GetHtmlTag(m_elem_type);

}

// dtor
CTaggedPdfElement::~CTaggedPdfElement() {
}

// methods
void CTaggedPdfElement::tag_begin() {
  m_writer->push_html("<" + m_html_tag + " " + tag_attributes() + ">\n");
}


void CTaggedPdfElement::tag_end() {
  m_writer->push_html("</" + m_html_tag + ">\n");
}

string CTaggedPdfElement::tag_attributes() {
  string orig_type, id, lang;
  wstring type;
  stringstream attrs;

  orig_type = get_original_se_type(m_elem);
  if (!orig_type.empty())
    attrs << "data-pdf-se-type=\"" << orig_type << "\" ";

  type = CTaggedPdfUtils::StructElementGetType(m_elem);

  if (!type.empty())
    attrs << "data-pdf-se-type-original=\"" << w2utf8(type) << "\" ";

  // 4.3.6.1 General
  // ID
  id = m_config.id;

  if (!type.empty()) {
    //if (type == L"Lbl" && m_html_tag=="label") // rt - for len pre Lbl pod Formom
    //  attrs << "for=\"" << id << "\" ";
    //else
    attrs << "id=\"" << id << "\" ";
  }

  // 4.3.6.2 Lang
  if (m_elem_dict->Known(L"Lang")) {
    lang.resize(m_elem_dict->GetString(L"Lang", nullptr, 0));
    m_elem_dict->GetString(L"Lang", const_cast<char*> (lang.c_str()), static_cast<int> (lang.size()));
    attrs << "lang=\"" << lang << "\" ";
  }

  // styling form attr dict
  CTaggedPdfAttributes attributes(m_doc, m_elem, m_config.classmap);
  string attributes_str = attributes.attributes();

  merge_attributes(attrs, attributes_str);
  return attrs.str();
}


void CTaggedPdfElement::tag_element() {
  // 4.3.5.7 NonStruct, Private and Artifact
  // "Artifact" || "Private" are ignored with all kids
  if (m_elem_type == L"Artifact" || m_elem_type == L"Private")
    return;
  //NonStruct ignore SE but process kids
  if (m_elem_type == L"NonStruct") {
    tag_substructure();
    return;
  }

  m_config.id = CTaggedPdfUtils::StructElementGetID(m_elem_dict);

  // ID-For pair for Form-Lbl
  static string form_id{ "" };
  if (m_elem_type == L"Form") {
    form_id = m_config.id;
  }
  if (m_elem_type == L"Lbl" && !form_id.empty()) {
    m_config.id = form_id;
    form_id.clear();
  }

  //tag_begin postponed 
  if (!m_is_form)
    tag_begin();

  // process /AF - associated files  
  bool af_stops_processing = false;
  if (m_elem_dict->Known(L"AF")) {
    PdsObject* af = m_elem_dict->Get(L"AF");
    if (af) {
      PdfObjectType af_type = af->GetObjectType();
      if (af_type == kPdsDictionary) {
        af_stops_processing |= process_af((PdsDictionary*)af);
      }
      else if (af_type == kPdsArray) {
        for (int i = 0; i < ((PdsArray*)af)->GetNumObjects(); i++)
          af_stops_processing |= process_af((PdsDictionary*)(((PdsArray*)af)->Get(i)));
      }
    }
  }
  if (!af_stops_processing)
    tag_substructure();

  tag_end();
}

void CTaggedPdfElement::tag_substructure() {
  PdsStructTree* struct_tree = m_doc->GetStructTree();
  // 4.3.6.3 Replacement text 
  wstring actual_text;
  actual_text.resize(m_elem->GetActualText(nullptr, 0));
  m_elem->GetActualText(const_cast<wchar_t*> (actual_text.c_str()), actual_text.size());
  if (!actual_text.empty()) {
    actual_text.erase(std::remove(actual_text.begin(), actual_text.end(), '\0'), actual_text.end());
    string at = CTaggedPdfUtils::EscapeHtmlText(w2utf8(actual_text));
    m_writer->push_html(at);

    return; // don't process kids
  }

  // iterate structure element's kids
  for (int i = 0; i < m_elem->GetNumKids(); i++) {
    switch (m_elem->GetKidType(i)) {
    case PdfStructElementType::kPdsStructKidElement: {
      PdsObject* kid_object = m_elem->GetKidObject(i);
      if (!kid_object)
        throw std::exception("Can't acquire kid object");
      PdsStructElement* kid_struct_element = struct_tree->AcquireStructElement(kid_object);
      if (!kid_struct_element)
        throw std::exception("Can't acquire kid object");

      unique_ptr<CTaggedPdfElement> tagged_kid_element = CTaggedPdfElementFactory::make_tagged_structure_element(m_doc, kid_struct_element, m_config);
      tagged_kid_element->tag_element();
      break;
    }
    case PdfStructElementType::kPdsStructKidPageContent: {
      int kid_mcid = m_elem->GetKidMcid(i);
      int kid_page = m_elem->GetKidPageNumber(i);
      if (CTaggedPdfHtmlDoc::m_actual_page.m_page_num != kid_page) {
        if (CTaggedPdfHtmlDoc::m_actual_page.m_page)
          CTaggedPdfHtmlDoc::m_actual_page.m_page->Release();

        CTaggedPdfHtmlDoc::m_actual_page.m_page_num = kid_page;
        CTaggedPdfHtmlDoc::m_actual_page.m_page = m_doc->AcquirePage(CTaggedPdfHtmlDoc::m_actual_page.m_page_num);
        for (int i = 0; i < CTaggedPdfHtmlDoc::m_actual_page.m_page->GetNumPageObjects(); i++) {
          PdsPageObject* page_object = CTaggedPdfHtmlDoc::m_actual_page.m_page->GetPageObject(i);
          page_object->SetRender(false);
        }
      }
      if (!CTaggedPdfHtmlDoc::m_actual_page.m_page)
        throw std::exception("Can't locate page for structure element");

      //todo: check new mcid handling
      for (int i = 0; i < CTaggedPdfHtmlDoc::m_actual_page.m_page->GetNumPageObjects(); i++) {
        PdsPageObject* page_object = CTaggedPdfHtmlDoc::m_actual_page.m_page->GetPageObject(i);
        PdsContentMark* mc = page_object->GetContentMark();
        if (mc)
          if (mc->GetTagMcid() == kid_mcid) {
            switch (page_object->GetObjectType()) {
            case kPdsPageText:
              m_writer->push_html(process_text((PdsText*)page_object));
              break;
            case kPdsPageImage:
              m_writer->push_html(process_image(CTaggedPdfHtmlDoc::m_actual_page.m_page, (PdsImage*)page_object));
              break;
            case kPdsPagePath:
              m_writer->push_html(process_path(CTaggedPdfHtmlDoc::m_actual_page.m_page, (PdsPath*)page_object));
              break;
            case kPdsPageShading:
              m_writer->push_html(process_shading(CTaggedPdfHtmlDoc::m_actual_page.m_page, (PdsShading*)page_object));
              break;
            case kPdsPageForm:
              break;
            case kPdsPageUnknown:
              break;
            default:
              throw std::exception("Invalid object type");
              break;
            }
          }
      }
    }

// MCR - Marked Content Reference
// ISO 32000 14.7.5.2 Marked-content sequences as content items
    case PdfStructElementType::kPdsStructKidStreamContent: {
      break;
    }

// OBJR - Object Reference
// ISO 32000 14.7.5.3 PDF objects as content items
    case PdfStructElementType::kPdsStructKidObject: {
      // OBJR for Form, print proper form tag
      if (m_is_form)
        tag_begin();
      break;
    }
    default:
      break;
    }
  }
}

string CTaggedPdfElement::process_text(PdsText* obj) {
  wstring unicode_text = CTaggedPdfUtils::PdfTextGetText(obj);
  std::string str = "";

  str = w2utf8(unicode_text);
  str = CTaggedPdfUtils::EscapeHtmlText(str);

  return str;
}

string CTaggedPdfElement::process_image(PdfPage* page, PdsImage* obj) {
  return process_pagemap_image(page, (PdsPageObject*)obj);
}

string CTaggedPdfElement::process_pagemap_image(PdfPage* page, PdsPageObject* obj) {
  static int image_counter = 0;
  std::string str;

  PdfPageRenderParams render_params;
  PdfImageParams image_params;
  PdfRect obj_rect;
  PdfDevRect obj_dev_rect;
  obj->GetBBox(&obj_rect);

  obj->SetRender(true);
  PdfPageView* page_view = page->AcquirePageView(1.0, kRotate0);
  page_view->RectToDevice(&obj_rect, &obj_dev_rect);

  std::wstring image_name = L"img" + std::to_wstring(image_counter++) + L".png";

  int width, height;
  width = page_view->GetDeviceWidth();
  height = page_view->GetDeviceHeight();
  page_view->GetDeviceMatrix(&render_params.matrix);

  //4.3.6.4 Alternate description
  wstring alt_text;
  alt_text.resize(m_elem->GetAlt(nullptr, 0));
  m_elem->GetAlt(const_cast<wchar_t*> (alt_text.c_str()), alt_text.size());

  str += "<img ";
  str += "src=\"" + w2utf8(image_name) + "\"";
  if (!alt_text.empty()) {
    alt_text.erase(std::remove(alt_text.begin(), alt_text.end(), '\0'), alt_text.end());
    str += " alt=\"" + CTaggedPdfUtils::EscapeHtmlText(w2utf8(alt_text)) + "\"";
  }
  str += " width=\"" + std::to_string(obj_dev_rect.right - obj_dev_rect.left) + "\"";
  str += " height=\"" + std::to_string(obj_dev_rect.bottom - obj_dev_rect.top) + "\"";
  str += ">";

  page_view->Release();

  PsImage* image = GetPdfix()->CreateImage(width, height, kImageDIBFormatArgb);
  std::wstring image_path = m_config.data_path + image_name;
  PsStream *image_file_stream = GetPdfix()->CreateFileStream(image_path.c_str(), kPsTruncate);
  if (!image_file_stream)
    throw std::exception("Can't create file for an image");
  if (!image)
    throw std::exception("Can't create image");

  render_params.image = image;
  //render_params.render_flags = kRenderNoText | kRenderNoBackground;
  page->DrawContent(&render_params, nullptr, nullptr);
  //whole page
  //image->SaveToStream(image_file_stream, &image_params);
  //just the Rect in which the Object is
  image->SaveRectToStream(image_file_stream, &image_params, &obj_dev_rect);
  image->Destroy();
  image_file_stream->Destroy();
  obj->SetRender(false);
  return str;
}

string CTaggedPdfElement::process_path(PdfPage* page, PdsPath* obj) {
  return process_pagemap_image(page, (PdsPageObject*)obj);
}

string CTaggedPdfElement::process_shading(PdfPage* page, PdsShading* obj) {
  return process_pagemap_image(page, (PdsPageObject*)obj);
}


bool CTaggedPdfElement::process_af(PdsDictionary* af_dict) {
  if (af_dict == nullptr)
    return false;

  PdsDictionary* ef_dict = af_dict->GetDictionary(L"EF");

  auto build_af_html = [this, ef_dict](std::string src_string, AFSubType af_subtype) {
    std::string str;
    switch (af_subtype) {
    case AFSubType::MATHML: {
      if (!m_math_script_included) {
        //str = "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.7.1/katex.min.css\" integrity=\"sha384-wITovz90syo1dJWVh32uuETPVEtGigN07tkttEqPv+uR2SE/mbQcG7ATL28aI9H0\" crossorigin=\"anonymous\">";
        //str = "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.7.1/katex.min.js\" integrity=\"sha384-/y1Nn9+QQAipbNQWU65krzJralCnuOasHncUFXGkdwntGeSvQicrYkiUBwsgUqc1\" crossorigin=\"anonymous\"></script>";
        str = "<script src = \"https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML\" type = \"application/javascript\">	</script>";

        m_math_script_included = true;
        m_writer->push_html(str);
      }
      m_writer->push_html(src_string);
      break;
    }
    case AFSubType::SVG: {
      //todo #112: AF Image and SVG Dimensions
      //int width = 100;
      //int height = 100;
      str = "<img ";
      //if (ef_dict != nullptr) {
      //  str += "width=\"" + std::to_string(width) + "\" ";
      //  str += "height=\"" + std::to_string(height) + "\" ";
      //}
      str += "src=\"";
      str += src_string;
      str += "\" ";
      str += ">\n";
      m_writer->push_html(str);
      break;
    }
    case AFSubType::IMG: {
      //todo #112: AF Image and SVG Dimensions
      int width = 100;
      int height = 100;
      str = "<img ";
      if (ef_dict != nullptr) {
        str += "width=\"" + std::to_string(width) + "\" ";
        str += "height=\"" + std::to_string(height) + "\" ";
      }
      str += "src=\"";
      str += src_string;
      str += "\" ";
      str += ">\n";
      m_writer->push_html(str);
      break;
    }
    case AFSubType::CSS: {
      str = "<style>@import url(";
      str += src_string;
      str += "); </style>\n";
      m_writer->push_html(str);
      break;
    }
    case AFSubType::JS: {
      str = "<script ";
      str += "src=\"";
      str += src_string;
      str += "\" ";
      str += "type=\"application/javascript\"> </script>\n";
      m_writer->push_html(str);
      break;
    }
    case AFSubType::HTML: {
      if (ef_dict == nullptr) {
        str = "<a rel=\"import\" ";
        str += "href=\"";
        str += src_string;
        str += "\"</a>";
        m_writer->push_html(str);
      }
      break;
    }
    default:
      break;
    }
  };

  // if EF exists then Embedded file
  if (ef_dict != nullptr) {

    // take /UF as URL and 
    std::string uf;
    uf.resize(af_dict->GetString(L"UF", nullptr, 0));
    af_dict->GetString(L"UF", (char*)uf.c_str(), static_cast<int> (uf.size()));

    PdsStream* f = ef_dict->GetStream(L"F");
    if (f) {
      PdsDictionary *f_dict = f->GetStreamDict();
      std::string subtype;
      subtype.resize(f_dict->GetString(L"Subtype", nullptr, 0));
      f_dict->GetString(L"Subtype", (char*)subtype.c_str(), static_cast<int> (subtype.size()));

      int len = f->GetSize();
      unique_charp buffer((char*)malloc(len));
      f->Read(0, (uint8_t*)buffer.get(), len);
      // sometimes we save content of the AF to file, sometimes directly into the html output
      auto save_af_to_file = [&]() {
        std::wstring ws_tmp(uf.begin(), uf.end());
        wstring file_name = m_config.data_path + ws_tmp;
        PsStream *file_stream = GetPdfix()->CreateFileStream(file_name.c_str(), kPsTruncate);
        if (!file_stream || !file_stream->Write(0, (const uint8_t*)buffer.get(), len))
          throw std::exception("Can't save associated file");
        file_stream->Destroy();
      };

      //based on subtype process the AF
      if (subtype == "application/mathml+xml") {
        std::string xx = buffer.get();
        xx.resize(len);
        build_af_html(xx, AFSubType::MATHML);
      }
      if (subtype == "image/svg+xml") {
        save_af_to_file();
        build_af_html(uf, AFSubType::SVG);
      }
      if (subtype == "image/jpeg" || subtype == "image/png" || subtype == "image/gif") {
        save_af_to_file();
        build_af_html(uf, AFSubType::IMG);
      }
      if (subtype == "text/css") {
        save_af_to_file();
        build_af_html(uf, AFSubType::CSS);
      }
      else if (subtype == "text/javascript" || subtype == "application/javascript") {
        save_af_to_file();
        build_af_html(uf, AFSubType::JS);
      }
      else if (subtype == "text/html" || subtype == "application/xhtml+xml") {
        save_af_to_file();
        build_af_html(uf, AFSubType::HTML);
      }
      else {
      }
    }
    std::string relationship;
    relationship.resize(af_dict->GetString(L"AFRelationship", nullptr, 0));
    af_dict->GetString(L"AFRelationship", (char*)relationship.c_str(), static_cast<int> (relationship.size()));
    if (relationship == "Alternative")
      return true;
  }
  else {
    //URL ?
    //8.4.6.2 Identification of the URL
    std::string url;
    url.resize(af_dict->GetString(L"F", nullptr, 0));
    af_dict->GetString(L"F", (char*)url.c_str(), static_cast<int> (url.size()));
    if (url == "")
      return false;

    auto check_url = [url](std::vector<std::string> const &extensions) {
      for (auto const &ext : extensions) {
        //compare extensions
        if (url.find(ext) != std::string::npos) {
          //check url scheme
          std::string regex_string = "(https?|ftp)://.*";
          std::regex url_regex(regex_string);
          return std::regex_match(url.c_str(), url_regex);
        }
      }
      return false;
    };

    //std::vector <std::string> extensions = { ".xml", ".mathml" };
    //8.4.6.3 Identification of media type
    if (check_url({ ".xml", ".mathml" })) {
      build_af_html(url, AFSubType::MATHML);
    }
    if (check_url({ ".svg" })) {
      build_af_html(url, AFSubType::SVG);
    }
    if (check_url({ ".gif", ".png", ".jpg", ".jpeg" })) {
      build_af_html(url, AFSubType::IMG);
    }
    if (check_url({ ".css" })) {
      build_af_html(url, AFSubType::CSS);
    }
    else if (check_url({ ".js" })) {
      build_af_html(url, AFSubType::JS);
    }
    else if (check_url({ ".htm", ".html", ".xhtml" })) {
      build_af_html(url, AFSubType::HTML);
    }
    else {
    }
  }
  return false;
}
