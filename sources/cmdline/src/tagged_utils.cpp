////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////

// system files
#ifdef _WIN32
#include <Windows.h>
extern HINSTANCE ghInstance;
#elif defined __linux__
#include <linux/limits.h>
#endif

#if defined _MSC_VER
#include <direct.h>
#elif defined __GNUC__
#include <sys/types.h>
#include <sys/stat.h>
#endif


#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <vector>
#include <string>
#include <regex>
#include <codecvt>

#include "tagged_utils.h"

using std::vector;
using std::string;

////////////////////////////////////////////////////////////////////////////////////////////////////
// strings
////////////////////////////////////////////////////////////////////////////////////////////////////
// convert UTF-8 string to wstring
std::wstring utf82w(const std::string& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
  return myconv.from_bytes(str);
}

// convert wstring to UTF-8 string
std::string w2utf8(const std::wstring& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
  return myconv.to_bytes(str);
}

void trim(std::wstring& str, const std::wstring& whitespace) {
  const auto strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return; // no content
  const auto strEnd = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;
  str.erase(0, strBegin);
  str.erase(strRange, str.size() - strRange);
}

void replace_in_str(std::wstring& js, std::wstring find, std::wstring repl) {
  size_t pos = 0;
  while (true) {
    pos = js.find(find, pos);
    if (pos == std::wstring::npos)
      break;
    js.erase(pos, find.length());
    js.insert(pos, repl.c_str());
    pos += repl.length();
  }
}

//std::string file_to_base64(const std::string& path) {  
//  auto pdfix = CPdfToHtml::get_pdf_to_html().GetPdfix();
//  PsStream* stream = pdfix->CreateFileStream(utf82w(path.c_str()).c_str(), kPsReadOnly);
//  auto ret = stream_to_base64(stream);
//  stream->Destroy();
//  return ret;
//}

std::string stream_to_base64(PsStream* stream) {
  auto len = stream->GetSize();
  unique_charp buffer((char*)malloc(len));
  stream->Read(0, (uint8_t*)buffer.get(), len);
  return base64_encode((unsigned char*)buffer.get(), len);
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// files folders
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetAbsolutePath(const std::string& path) {
  std::string result;
#ifndef _WIN32
  if (path.length() && path.front() == '/') {
    result = path;
  }
  else {
    result.resize(PATH_MAX);
    realpath(path.c_str(), (char*)result.c_str());
  }
#else
  std::string dir;
  dir.resize(_MAX_PATH);
  GetModuleFileNameA(NULL, (char*)dir.c_str(), _MAX_PATH);
  dir.erase(dir.begin() + dir.find_last_of("\\/") + 1, dir.end());
  SetCurrentDirectoryA(dir.c_str());
  result.resize(_MAX_PATH);
  GetFullPathNameA(path.c_str(), _MAX_PATH, (char*)result.c_str(), NULL);
#endif
  result.resize(strlen(result.c_str()));
  return result;
}

bool folder_exists(const std::string& path) {
  struct stat s;
  if (stat(path.c_str(), &s) == 0) {
    if (s.st_mode & S_IFDIR) return true;
  }
  return false;
}

bool folder_exists(const std::wstring& path) {
  return folder_exists(w2utf8(path.c_str()));
}

bool file_exists(const std::string& path) {
  struct stat s;
  if (stat(path.c_str(), &s) == 0) {
    if (s.st_mode & S_IFREG) return true;
  }
  return false;
}

bool file_exists(const std::wstring& path) {
  return file_exists(w2utf8(path.c_str()));
}

std::wstring check_folder_path(const std::wstring& path) {
  std::wstring result = path;
  if (result.back() != '/' && result.back() != '\\')
    result += L"/";
  return result;
}

std::string get_path_dir(const std::string& path) {
  auto pos = path.find_last_of("\\/");
  if (pos == std::string::npos) return path;
  std::string dir(path.begin(), path.begin() + pos);
  return dir;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// derivation
////////////////////////////////////////////////////////////////////////////////////////////////////
void fix_js(std::wstring& js) {
  trim(js);
  return;
}

std::string get_new_html_class_name() {
  static int counter = 0;
  return std::to_string(counter++);
}

std::string color2hexstr(const PdfRGB& color) {
  std::stringstream stream;
  stream << "#";
  stream << std::setfill('0');
  stream << std::hex << std::setw(2) << color.r;
  stream << std::hex << std::setw(2) << color.g;
  stream << std::hex << std::setw(2) << color.b;
  return stream.str();
}

std::string get_form_field_id(PdfFormField* field) {
  return std::to_string(std::abs(reinterpret_cast<long long> (field)));
}


std::wstring encode_html_text(const std::wstring& text) {
  // https://www.w3schools.com/html/html_charset.asp
  std::wstring result;
  for (auto& c : text) {
    switch (c) {
    case '<': result += L"&#60;"; break;
    case '>': result += L"&#62;"; break;
    case '&': result += L"&#38;"; break;
    case '"': result += L"&#34;"; break;
    case '\'': result += L"&#39;"; break;
    case '¢': result += L"&#162;"; break;
    case '£': result += L"&#163;"; break;
    case '¥': result += L"&#165;"; break;
    case '€': result += L"&#128;"; break;
    case '©': result += L"&#169;"; break;
    case '®': result += L"&#174;"; break;
    default: result += c;
    }
  }
  return result;
}

std::wstring encode_js_text(const std::wstring& text) {
  std::wstring result;
  for (auto& c : text) {
    if (c < 32)
      result.push_back(' ');
    else
      result.push_back(c);
  }
  return result;
}

std::string generate_id() {
  static std::string str = "0123456789abcdefghijklmnopqrstuvwxyz";

  std::random_device rd;
  std::mt19937 generator(rd());

  std::shuffle(str.begin(), str.end(), generator);

  return str.substr(0, str.size() - (str.size() / 2));
}

std::string get_original_se_type(PdsStructElement* elem) {
  PdsObject* obj = elem->GetElementObject();
  if (obj->GetObjectType() != kPdsDictionary)
    return "";

  if ((static_cast<PdsDictionary*> (obj))->Known(L"S")) {
    PdsObject* s = (static_cast<PdsDictionary*> (obj))->Get(L"S");

    if (s->GetObjectType() != kPdsName)
      throw std::runtime_error("Name expected as structure element type");

    std::string value;
    value.resize((static_cast<PdsName*> (s))->GetValue(nullptr, 0));
    (static_cast<PdsName*> (s))->GetValue((char*)value.c_str(), static_cast<int> (value.size()));

    return value;
  }

  return "";
}

//  - compare strings and remove duplicated attribute values(for example id, class, ...) - attributes from pdf attribute objects will stay
//  - or merge attribute values (for example style="...")
void merge_attributes(std::stringstream& attrs, std::string& attributes_str) {
  std::regex reg_attr("\\S+=\\\"+[A-Za-z0-9:.; _-]+\\\"");
  std::regex reg_attr_first("\\S+=");

  std::smatch wm_attr, wm_attr_first, wm;

  std::string attributes_str_c = attributes_str;
  std::string attrs_str = attrs.str();

  if (!attributes_str.empty()) {
    while (std::regex_search(attributes_str_c, wm_attr, reg_attr)) {
      for (auto x : wm_attr) {
        std::string to_repl = x;

        if (std::regex_search(to_repl, wm_attr_first, reg_attr_first)) {
          std::string str = wm_attr_first[0];
          str.erase(str.end() - 1);

          std::regex reg("((^" + str + ")|( " + str + "))+=\\\"+[A-Za-z0-9:.; _-]+\\\"");

          if (str == "style") {
            if (std::regex_search(attrs_str, wm, reg)) {
              std::size_t start_pos = attrs_str.find("\"");

              if (start_pos != std::string::npos) {
                std::size_t end_pos = attrs_str.find("\"", start_pos + 1);
                string val = attrs_str.substr(start_pos + 1, end_pos - start_pos - 1);
                std::size_t attr_pos = attributes_str.find(str);
                attributes_str.insert(attr_pos + 7, val);
              }
            }
          }

          if (std::regex_search(attrs_str, wm, reg)) {
            attrs_str = std::regex_replace(attrs_str, reg, "");
            attrs.str("");
            attrs.clear();
            attrs << attrs_str;
          }
        }
      }
      attributes_str_c = wm_attr.suffix().str();
    }
    attrs.seekp(0, std::ios_base::end);
    attrs << attributes_str;
  }
}
