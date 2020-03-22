////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_utils_h
#define tagged_pdf_utils_h

#include "Pdfix.h"
#include <string>

using std::string;
using std::wstring;
using namespace PDFixSDK;

class CTaggedPdfUtils final
{
private:
  CTaggedPdfUtils();

  static string get_value(PdsObject* obj);
  static wstring get_text(PdsObject* obj);

public:
  ~CTaggedPdfUtils();

  static string PdfNameGetValue(PdsName* name);
  static wstring PdfNameGetText(PdsName* name);

  static string PdfStringGetValue(PdsString* str);
  static wstring PdfStringGetText(PdsString* str);

  static wstring PdfTextGetText(PdsText* text);

  static wstring PdfDictionaryGetKeyAtIndex(PdsDictionary* dict, int idx);
  static wstring PdfDictionaryGetTextForKey(PdsDictionary* dict, const wstring& key);
  static string PdfDictionaryGetStringForKey(PdsDictionary* dict, const wstring& key);

  static wstring PdfArrayGetTextAtIndex(PdsArray* ar, int idx);

  static wstring StructElementGetType(PdsStructElement* elem);
  static string StructElementGetID(PdsDictionary* structure_element_dict);
  static string StructElementGetAction(PdsDictionary* structure_element_dict);

  static string EscapeHtmlText(const string& str);
};

#endif // tagged_pdf_utils_h
