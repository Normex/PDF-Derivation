////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include <iostream>
#include <memory>
#include <fstream>
#include <ctime>
#include <utility>
#include "tagged_pdf_html_doc.h"
#include "tagged_pdf_form.h"
#include "tagged_utils.h"
#include "tagged_pdf_scripts.h"
#include "derived_output_writer.h"
#include "tagged_pdf_element_factory.h"

using std::unique_ptr;

CTaggedPdfHtmlDoc::Page CTaggedPdfHtmlDoc::m_actual_page;

//ctor
CTaggedPdfHtmlDoc::CTaggedPdfHtmlDoc(PdfDoc* doc) {
  m_doc = doc;
  m_actual_page.m_page = nullptr;
  m_actual_page.m_page_num = -1;
  m_classmap = std::make_unique<CClassMap>(m_doc);
}

CTaggedPdfHtmlDoc::~CTaggedPdfHtmlDoc() {
  if (m_writer->is_open())
    m_writer->save_and_close();

  delete m_writer;

  m_actual_page.m_page = nullptr;
  m_actual_page.m_page_num = -1;
}

void CTaggedPdfHtmlDoc::save(const std::wstring& path) {
  m_writer = new CDerivedOutputWriter(path);

  m_path = path;

  std::wstring lang;
  lang.resize(m_doc->GetLang(nullptr, 0));
  m_doc->GetLang((wchar_t*)lang.c_str(), (int)lang.size());

  m_writer->push_html("<!DOCTYPE html> \n");

  if (lang.empty())
    m_writer->push_html("<html>\n");
  else
    m_writer->push_html("<html lang=\"" + w2utf8(lang) + "\">\n");
  m_writer->push_html("<head>\n");

  // prepare time and versin number
  //todo: time_t rawtime;
  //tm timeinfo;
  //errno_t result = localtime_s(&timeinfo, &rawtime);
  //char time_buff[50];
  //asctime_s(time_buff, 32, &timeinfo);
  //std::string time_str = time_buff;
  std::string time_str = "";
  std::string ver_str;

  //todo:
  ver_str = std::to_string(1) // CTaggedPdf::get_tagged_pdf().GetVersionMajor())
    + "." + std::to_string(1) // CTaggedPdf::get_tagged_pdf().GetVersionMinor())
    + "." + std::to_string(1); // CTaggedPdf::get_tagged_pdf().GetVersionPatch());

  ver_str +=
    " (PDFix "
    + std::to_string(PDFixSDK::GetPdfix()->GetVersionMajor())
    + "." + std::to_string(PDFixSDK::GetPdfix()->GetVersionMinor())
    + "." + std::to_string(PDFixSDK::GetPdfix()->GetVersionPatch())
    + ")";

  std::wstring title;
  title.resize(m_doc->GetInfo(L"Title", nullptr, 0));
  m_doc->GetInfo(L"Title", (wchar_t*)title.c_str(), (int)title.size());
  m_writer->push_html("<title>" + w2utf8(title) + "</title>");

  m_writer->push_html("<meta name=\"application-name\" content=\"PDF-Derivation " + ver_str + "\">\n");
  m_writer->push_html("<meta name=\"application-url\" content=\"https://github.com/Normex/PDF-Derivation\">\n");
  m_writer->push_html("<meta name=\"author\" content=\"Normex s.r.o.\">\n");
  m_writer->push_html("<meta name=\"description\" content=\"Produced by deriving pdf file into html according to 'Deriving HTML from PDF - A usage specification for tagged ISO 32000 - 2 files' published by PDF Association (www.pdfa.org)\"> \n");
  m_writer->push_html("<meta name=\"revised\" content=\"" + time_str + "\">\n");
  m_writer->push_html("<meta name=\"copyright\" content=\"Normex, PDFix (https://pdfix.net)\"> \n");
  m_writer->push_html("<meta charset=\"UTF-8\">\n");
  m_writer->push_html("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n");


  // link to css
  m_writer->push_html("<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">\n");

  if (has_form()) {
    auto save_script = [=](const char*buffer, const std::wstring file_name) {
      wstring file_path = m_path + file_name;
      PsStream *file_stream = PDFixSDK::GetPdfix()->CreateFileStream(file_path.c_str(), kPsTruncate);
      if (!file_stream || !file_stream->Write(0, (const uint8_t*)buffer, (int)strlen(buffer)))
        throw std::runtime_error("Can't write script file");
      file_stream->Destroy();
    };

    save_script(g_pdfix_forms_js, L"pdfix_forms.js");
    save_script(g_pdfix_acroform_js, L"pdfix_acroform.js");

    m_writer->push_html("<script src=\"pdfix_acroform.js\"></script>\n");
    m_writer->push_html("<script src=\"pdfix_forms.js\"></script>\n");
  }
  // link to js
  m_writer->push_html("<script src=\"script.js\"></script>\n");

  m_writer->push_html("</head>\n");
  m_writer->push_html("<body>\n");

  if (has_form()) {
    m_writer->push_html("<form>\n");
  }

  PdsStructTree* struct_tree = m_doc->GetStructTree();
  if (!struct_tree)
    throw std::runtime_error("Can't acquire structure tree root");
  for (auto i = 0; i < struct_tree->GetNumKids(); i++) {
    PdsObject* obj = struct_tree->GetKidObject(i);
    PdsStructElement* struct_elem = struct_tree->AcquireStructElement(obj);

    CTaggedPdfElementConfig elem_config;
    elem_config.data_path = m_path;
    elem_config.writer = m_writer;
    elem_config.classmap = m_classmap.get();

    unique_ptr<CTaggedPdfElement> tagged_elem = CTaggedPdfElementFactory::make_tagged_structure_element(m_doc, struct_elem, elem_config);
    if (tagged_elem)
      tagged_elem->tag_element();
    struct_elem->Release();
  }

  setup_html_end();
  m_writer->save_and_close();
}

void CTaggedPdfHtmlDoc::setup_html_end() {
  if (has_form()) {
    m_writer->push_html("</form>\n");
  }

  std::string str;
  str = "</body>\n";
  str += "</html>\n";
  m_writer->push_html(str);

  // write css styles at the end.
  string css_style = m_classmap->get_css_style_content();
  if (!css_style.empty())
    m_writer->push_css(css_style);
}

void CTaggedPdfHtmlDoc::process_class_map(PdsStructTree* tree) {
  const std::wstring prefix_string = L".";
  PdsClassMap* class_map = tree->GetClassMap();
  if (class_map == nullptr)
    return;
  process_css(class_map, prefix_string);
}

void CTaggedPdfHtmlDoc::process_css(PdsClassMap* class_map, std::wstring prefix_string) {
  bool standard_property = false;
}

void CTaggedPdfHtmlDoc::process_content(PdsStructElement* kid, PdsStream* stream) {
}
