////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "derived_output_writer.h"
#include "tagged_utils.h"
#include <vector>

using std::vector;

// ctor
CDerivedOutputWriter::CDerivedOutputWriter(const wstring& path) {
  Pdfix* pdfix = GetPdfix();
  wstring html_path = path + L"index.html";

  m_html_mem_stream = pdfix->CreateMemStream();
  m_html_file_stream = pdfix->CreateFileStream(html_path.c_str(), kPsTruncate);
  if (!m_html_file_stream || !m_html_file_stream)
    throw std::runtime_error("Error preparing output");


  m_css_mem_stream = pdfix->CreateMemStream();
  wstring css_path = path + L"style.css";
  m_css_file_stream = pdfix->CreateFileStream(css_path.c_str(), kPsTruncate);
  if (!m_css_mem_stream || !m_css_file_stream)
    throw std::runtime_error("Error preparing output");

  m_js_mem_stream = pdfix->CreateMemStream();
  wstring js_path = path + L"script.js";
  m_js_file_stream = pdfix->CreateFileStream(js_path.c_str(), kPsTruncate);
  if (!m_js_mem_stream || !m_js_file_stream)
    throw std::runtime_error("Error preparing output");

  m_opened = true;
}

// dtor
CDerivedOutputWriter::~CDerivedOutputWriter() {

}

// methods
void CDerivedOutputWriter::push_html(const string& str) {
  if (!str.empty()) {
    m_html_mem_stream->Write(m_html_mem_stream->GetPos(), (const uint8_t*)str.c_str(), static_cast<int> (str.length()));
  }
}

void CDerivedOutputWriter::push_js(const string& str) {
  if (!str.empty()) {
    m_js_mem_stream->Write(m_js_mem_stream->GetPos(), (const uint8_t*)str.c_str(), static_cast<int> (str.length()));
  }
}

void CDerivedOutputWriter::push_css(const string& str) {
  if (!str.empty()) {
    m_css_mem_stream->Write(m_css_mem_stream->GetPos(), (const uint8_t*)str.c_str(), static_cast<int> (str.length()));
  }
}

bool CDerivedOutputWriter::save_and_close() {
  if (!m_opened)
    return false;

  int html_size = m_html_mem_stream->GetSize();
  int js_size = m_js_mem_stream->GetSize();
  int css_size = m_css_mem_stream->GetSize();

  vector<uint8_t> html_data(html_size), js_data(js_size), css_data(css_size);

  if (html_size > 0) {
    if (!m_html_mem_stream->Read(0, &html_data[0], html_size))
      throw std::runtime_error("Error reading HTML stream");


    if (!m_html_file_stream->Write(m_html_file_stream->GetSize(), &html_data[0], html_size))
      throw std::runtime_error("Error writing HTML stream");

    m_html_file_stream->Flush();
  }


  if (js_size > 0) {
    if (!m_js_mem_stream->Read(0, &js_data[0], js_size))
      throw std::runtime_error("Error reading JS stream");


    if (!m_js_file_stream->Write(m_js_file_stream->GetSize(), &js_data[0], js_size))
      throw std::runtime_error("Error writing JS stream");

    m_js_file_stream->Flush();
  }

  if (css_size > 0) {
    if (!m_css_mem_stream->Read(0, &css_data[0], css_size))
      throw std::runtime_error("Error reading CSS stream");


    if (!m_css_file_stream->Write(m_css_file_stream->GetSize(), &css_data[0], css_size))
      throw std::runtime_error("Error writing CSS stream");

    m_css_file_stream->Flush();
  }

  m_html_mem_stream->Destroy();
  m_js_mem_stream->Destroy();
  m_css_mem_stream->Destroy();
  m_html_mem_stream = m_js_mem_stream = m_css_mem_stream = nullptr;

  m_html_file_stream->Destroy();
  m_js_file_stream->Destroy();
  m_css_file_stream->Destroy();
  m_html_file_stream = m_js_file_stream = m_css_file_stream = nullptr;

  m_opened = false;

  return true;
}
