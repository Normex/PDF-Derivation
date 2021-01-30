////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef derived_output_writer_h
#define derived_output_writer_h

#include <string>
#include "Pdfix.h"

using std::string;
using std::wstring;
using namespace PDFixSDK;

class CDerivedOutputWriter final
{
public:
  explicit CDerivedOutputWriter(const wstring& path);
  CDerivedOutputWriter(const CDerivedOutputWriter& o) = delete;
  CDerivedOutputWriter& operator=(const CDerivedOutputWriter& o) = delete;

  ~CDerivedOutputWriter();

  void push_html(const string& str);
  void push_js(const string& str);
  void push_css(const string& str);

  bool is_open() const { return m_opened; }
  bool save_and_close();

private:
  PsStream* m_html_mem_stream = nullptr;
  PsStream* m_js_mem_stream = nullptr;
  PsStream* m_css_mem_stream = nullptr;

  PsStream* m_html_file_stream = nullptr;
  PsStream* m_js_file_stream = nullptr;
  PsStream* m_css_file_stream = nullptr;

  bool m_opened = false;
};

#endif // derived_output_writer_h
