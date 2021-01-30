////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef pdf_html_utils_h
#define pdf_html_utils_h

#include <algorithm>
#include <string>
#include <cstring>
#include <iomanip>

#include "Pdfix.h"
using namespace PDFixSDK;

#if defined (__ANDROID__) || defined (__APPLE__) || defined (__GNUC__)
typedef unsigned char BYTE;
typedef unsigned short WORD;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// strings
////////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring utf82w(const std::string& str);
std::string w2utf8(const std::wstring& wstr);
void trim(std::wstring& str, const std::wstring& whitespace = L" \t\n\r");
std::string stream_to_base64(PsStream* stream);
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

////////////////////////////////////////////////////////////////////////////////////////////////////
// files folders
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetAbsolutePath(const std::string& path);
bool folder_exists(const std::wstring& path);
bool file_exists(const std::wstring& path);
std::wstring check_folder_path(const std::wstring& path);
std::string get_path_dir(const std::string& path);


////////////////////////////////////////////////////////////////////////////////////////////////////
// pointers
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct MallocDeleter {
  void operator() (T* ptr) {
    if (ptr) {
      free(ptr);
      ptr = nullptr;
    }
  }
};
typedef std::unique_ptr<char, MallocDeleter<char> > unique_charp;
typedef std::unique_ptr<wchar_t, MallocDeleter<wchar_t> > unique_wcharp;
typedef std::unique_ptr<int, MallocDeleter<int> > unique_intp;
typedef std::unique_ptr<BYTE, MallocDeleter<BYTE> > unique_BYTEp;

////////////////////////////////////////////////////////////////////////////////////////////////////
// derivation
////////////////////////////////////////////////////////////////////////////////////////////////////
void fix_js(std::wstring& js);
std::string get_new_html_class_name();
std::string color2hexstr(const PdfRGB& color);
std::string get_form_field_id(PdfFormField* field);
std::wstring encode_html_text(const std::wstring& text);
std::wstring encode_js_text(const std::wstring& text);
std::string generate_id();
std::string get_original_se_type(PdsStructElement* elem);
void merge_attributes(std::stringstream& attrs, std::string& attributes_str);

#endif
