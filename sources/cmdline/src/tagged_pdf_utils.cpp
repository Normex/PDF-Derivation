////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "tagged_pdf_utils.h"
#include "tagged_utils.h"
#include <sstream>


using std::stringstream;

// ctor
CTaggedPdfUtils::CTaggedPdfUtils() {
}

// dtor
CTaggedPdfUtils::~CTaggedPdfUtils() {
}

// methods
string CTaggedPdfUtils::get_value(PdsObject* obj) {
  string ret;
  switch (obj->GetObjectType()) {
  case kPdsName:
    ret.resize((static_cast<PdsName*> (obj))->GetValue(nullptr, 0));
    (static_cast<PdsName*> (obj))->GetValue(const_cast<char*> (ret.c_str()), static_cast<int> (ret.size()));
    break;
  case kPdsString:
    ret.resize((static_cast<PdsString*> (obj))->GetValue(nullptr, 0));
    (static_cast<PdsString*> (obj))->GetValue(const_cast<char*> (ret.c_str()), static_cast<int> (ret.size()));
    break;
  default:
    break;
  }

  return ret;
}

wstring CTaggedPdfUtils::get_text(PdsObject* obj) {
  wstring ret;
  switch (obj->GetObjectType()) {
  case kPdsName:
    ret.resize((static_cast<PdsName*> (obj))->GetText(nullptr, 0));
    (static_cast<PdsName*> (obj))->GetText(const_cast<wchar_t*> (ret.c_str()), static_cast<int> (ret.size()));
    break;
  case kPdsString:
    ret.resize((static_cast<PdsString*> (obj))->GetText(nullptr, 0));
    (static_cast<PdsString*> (obj))->GetText(const_cast<wchar_t*> (ret.c_str()), static_cast<int> (ret.size()));
    break;
  default:
    break;
  }

  return ret;
}

string CTaggedPdfUtils::PdfNameGetValue(PdsName* name) {
  return get_value(static_cast<PdsObject*> (name));
}

wstring CTaggedPdfUtils::PdfNameGetText(PdsName* name) {
  return get_text(static_cast<PdsObject*> (name));
}

string CTaggedPdfUtils::PdfStringGetValue(PdsString* str) {
  return get_value(static_cast<PdsString*> (str));
}

wstring CTaggedPdfUtils::PdfStringGetText(PdsString* str) {
  return get_text(static_cast<PdsString*> (str));
}

wstring CTaggedPdfUtils::PdfTextGetText(PdsText* text) {
  wstring ret;
  ret.resize(text->GetText(nullptr, 0));
  text->GetText(const_cast<wchar_t*> (ret.c_str()), static_cast<int> (ret.size()));

  return ret;
}

wstring CTaggedPdfUtils::PdfDictionaryGetKeyAtIndex(PdsDictionary* dict, int idx) {
  wstring ret;
  ret.resize(dict->GetKey(idx, nullptr, 0));
  dict->GetKey(idx, const_cast<wchar_t*> (ret.c_str()), static_cast<int> (ret.size()));

  return ret;
}

wstring CTaggedPdfUtils::PdfDictionaryGetTextForKey(PdsDictionary* dict, const wstring& key) {
  wstring ret;
  ret.resize(dict->GetText(key.c_str(), nullptr, 0));
  dict->GetText(key.c_str(), const_cast<wchar_t*> (ret.c_str()), static_cast<int> (ret.size()));

  return ret;
}

string CTaggedPdfUtils::PdfDictionaryGetStringForKey(PdsDictionary* dict, const wstring& key) {
  string ret;
  ret.resize(dict->GetString(key.c_str(), nullptr, 00));
  dict->GetString(key.c_str(), const_cast<char*> (ret.c_str()), static_cast<int> (ret.size()));

  return ret;
}

wstring CTaggedPdfUtils::PdfArrayGetTextAtIndex(PdsArray* ar, int idx) {
  wstring ret;
  ret.resize(ar->GetText(idx, nullptr, 0));
  ar->GetText(idx, const_cast<wchar_t*> (ret.c_str()), static_cast<int> (ret.size()));

  return ret;
}

wstring CTaggedPdfUtils::StructElementGetType(PdsStructElement* elem) {
  wstring ret;
  ret.resize(elem->GetType(true, nullptr, 0));
  elem->GetType(true, const_cast<wchar_t*> (ret.c_str()), static_cast<int> (ret.size()));

  return ret;
}

string CTaggedPdfUtils::StructElementGetID(PdsDictionary* structure_element_dict) {
  string id;
  if (structure_element_dict->Known(L"ID"))
    id = CTaggedPdfUtils::PdfDictionaryGetStringForKey(structure_element_dict, L"ID");
  else if (structure_element_dict->GetId() == 0) id = generate_id();
  else id = std::to_string(structure_element_dict->GetId());

  return id;
}

string CTaggedPdfUtils::StructElementGetAction(PdsDictionary* structure_element_dict) {
  string href("\"\"");

  //todo: have to go through cos objects for now because we are missing PdsReference 
  //when that's available we can rewrite 
  PdsObject* k_obj = structure_element_dict->Get(L"K");
  if (!k_obj)
    return href;

  PdsDictionary* objr_dict = nullptr;
  PdfObjectType k_type = k_obj->GetObjectType();
  if (k_type == kPdsDictionary)
    objr_dict = (PdsDictionary*)k_obj;
  else if (k_type == kPdsArray) {
    PdsArray* k_array = (PdsArray*)k_obj;
    for (int i = 0; i < k_array->GetNumObjects(); i++) {
      PdsObject* item = k_array->Get(i);
      if (item->GetObjectType() != kPdsDictionary)
        continue;
      objr_dict = (static_cast<PdsDictionary*> (item))->GetDictionary(L"Obj");
      if (objr_dict)
        break;
    }
  }
  //  can't find OBJR
  if (!objr_dict)
    return href;

  PdsDictionary* a_dict = objr_dict->GetDictionary(L"A");
  //todo: array of actions
  if (a_dict) {
    string action_subtype = CTaggedPdfUtils::PdfDictionaryGetStringForKey(a_dict, L"S");
    if (action_subtype == "URI")
      href = "\"" + CTaggedPdfUtils::PdfDictionaryGetStringForKey(a_dict, L"URI") + "\"";
    else if (action_subtype == "GoTo") {
      // check structure destination (2.0)
      PdsArray* sd_array = a_dict->GetArray(L"SD");
      if (sd_array) {
        PdsDictionary* sd = sd_array->GetDictionary(0);
        if (sd)
          href = "#" + CTaggedPdfUtils::StructElementGetID(sd);
      }
      else {
        //todo: other /Goto actions

        href = "\"\"";
      }
    }
    else if (action_subtype == "JavaScript") {
      // JS under event
      if (a_dict->Known(L"JS")) {
        PdsObject* js = a_dict->Get(L"JS");
        std::string js_str;
        if (js) {
          if (js->GetObjectType() == kPdsString) {
            js_str = CTaggedPdfUtils::PdfStringGetValue(static_cast<PdsString*> (js));
          }
          else if (js->GetObjectType() == kPdsStream) {
            int len = (static_cast<PdsStream*> (js))->GetSize();
            unique_charp buffer((char*)malloc(len));
            (static_cast<PdsStream*> (js))->Read(0, (uint8_t*)buffer.get(), len);
            js_str = buffer.get();
          }
        }
        //todo: write js somehow into the href, include js libraries?
        href = "\"\"";
      }
    }
    else {
      //todo: different type of actions
      href = "\"\"";
    }
  }
  return href;
}

string CTaggedPdfUtils::EscapeHtmlText(const string& str) {
  stringstream s;

  for (const auto& c : str) {
    switch (c) {
    case '&':
      s << "&amp;";
      break;
    case '<':
      s << "&lt;";
      break;
    case '>':
      s << "&gt;";
      break;
    case '"':
      s << "&quot;";
      break;
    case '\'':
      s << "&apos;";
      break;
    default:
      s << c;
    }
  }

  return s.str();
}
