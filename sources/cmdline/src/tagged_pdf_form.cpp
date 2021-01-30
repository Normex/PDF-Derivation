////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_form.h"
#include "tagged_utils.h"
#include "tagged_pdf_utils.h"

#include <sstream>
#include <vector>

using std::stringstream;
using std::vector;

// ctor
CTaggedPdfForm::CTaggedPdfForm(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  PdsObject* K_obj = m_elem_dict->Get(L"K");
  if (!K_obj)
    throw std::runtime_error("Can't identify Form object");

  //todo: this needs refactoring
  PdsDictionary* obj_dict = nullptr;
  if (K_obj->GetObjectType() == kPdsDictionary) {
    obj_dict = (static_cast<PdsDictionary*> (K_obj))->GetDictionary(L"Obj");
  }
  else if (K_obj->GetObjectType() == kPdsArray) {
    for (int i = 0; i < (static_cast<PdsArray*> (K_obj))->GetNumObjects(); ++i) {
      PdsDictionary* item = (static_cast<PdsArray*> (K_obj))->GetDictionary(i);

      if (item && item->Known(L"Obj")) {
        obj_dict = item->GetDictionary(L"Obj");
        if (obj_dict)
          break;
      }
    }
  }
  else {
    throw std::runtime_error("Can't identify Form object");
  }

  if (!obj_dict)
    throw std::runtime_error("Can't identify Form object");

  PdsName* ft_name = nullptr;

  if (obj_dict->Known(L"FT")) {
    ft_name = static_cast<PdsName*> (obj_dict->Get(L"FT"));
  }
  else {
    // check Parent
    PdsDictionary* parent = obj_dict->GetDictionary(L"Parent");
    if (parent) {
      if (parent->Known(L"FT")) {
        ft_name = static_cast<PdsName*> (parent->Get(L"FT"));

        PdsArray* kids = parent->GetArray(L"Kids");
        if (kids) {
          for (int i = 0; i < (static_cast<PdsArray*> (kids))->GetNumObjects(); ++i) {
            PdsObject* kid = (static_cast<PdsArray*> (kids))->Get(i);
            if (kid == obj_dict) {
              m_button_idx = i;
              break;
            }
          }
        }

        obj_dict = parent;
      }
    }
  }

  if (!ft_name)
    throw std::runtime_error("Can't identify form type (FT)");

  std::string ft_str;
  ft_str.resize(ft_name->GetValue(nullptr, 0));
  ft_name->GetValue((char*)ft_str.c_str(), static_cast<int> (ft_str.size()));

  if (ft_str == "Btn")
    m_form_field_type = FormFieldType::Btn;
  else if (ft_str == "Tx")
    m_form_field_type = FormFieldType::Tx;
  else if (ft_str == "Ch")
    m_form_field_type = FormFieldType::Ch;
  else if (ft_str == "Sig")
    m_form_field_type = FormFieldType::Sig;
  else
    throw std::runtime_error("Unknown form type (FT)");

  m_form_dict = obj_dict;
  m_is_form = true;
}

// dtor
CTaggedPdfForm::~CTaggedPdfForm() {

}

// methods
bool CTaggedPdfForm::is_annot_flag_active(AnnotFlag flag) {
  unsigned int flags = m_form_dict->GetInteger(L"F", 0);
  if (flags & (1 << (static_cast<unsigned int> (flag) - 1)))
    return true;

  return false;
}

bool CTaggedPdfForm::is_field_flag_active(FieldFlag flag) {
  unsigned int flags = m_form_dict->GetInteger(L"Ff", 0);
  if (flags & (1 << (static_cast<unsigned int> (flag) - 1)))
    return true;

  return false;
}
void CTaggedPdfForm::tag_begin() {
  string tag;
  switch (m_form_field_type) {
  case FormFieldType::Btn:
    process_button_type();
    break;
  case FormFieldType::Tx:
    process_text_type();
    break;
  case FormFieldType::Ch:
    process_choice_type();
    break;
  default:
    break;
  }
  m_writer->push_html(m_html_begin_tag);
}

void CTaggedPdfForm::tag_end() {
  m_writer->push_html(m_html_end_tag);
}

void CTaggedPdfForm::process_button_type() {
  ButtonType button_type = ButtonType::kUnknownButton;

  if (is_field_flag_active(FieldFlag::kPushButton)) {
    m_html_begin_tag = "<button";
    m_html_end_tag = "</button>\n";
    button_type = ButtonType::kPushButton;
  }
  else {
    if (is_field_flag_active(FieldFlag::kRadioButton)) {
      m_html_begin_tag = "<input type=\"radio\"";
      m_html_end_tag = "";
      button_type = ButtonType::kRadioButton;
    }
    else {
      m_html_begin_tag = "<input type=\"checkbox\" ";
      m_html_end_tag = "";
      button_type = ButtonType::kCheckBox;
    }
  }
  m_html_begin_tag += process_common_form_field_attributes();

  // is hidden?
  if (is_annot_flag_active(AnnotFlag::kHidden)) {
    m_html_begin_tag += " hidden=\"hidden\" ";
  }

  PdsDictionary* action_obj = m_form_dict->GetDictionary(L"A");
  if (action_obj) {
    string value;
    value.resize(action_obj->GetString(L"S", nullptr, 0));
    action_obj->GetString(L"S", (char*)value.c_str(), static_cast<int> (value.size()));

    if (value == "SubmitForm")
      m_html_begin_tag += " type=\"submit\"";
    else if (value == "ResetForm")
      m_html_begin_tag += " type=\"reset\"";
    else if (value == "ImportData")
      ; //todo:
    else if (value == "JavaScript")
      m_html_begin_tag += " type=\"button\"";
    else
      throw std::runtime_error("Unsupported action in form object");

    //todo: refactor. this code will be needed in other AFs
    PdsDictionary* f_obj = action_obj->GetDictionary(L"F");
    if (f_obj) {
      value.clear();
      value.resize(f_obj->GetString(L"FS", nullptr, 0));
      f_obj->GetString(L"FS", (char*)value.c_str(), static_cast<int> (value.size()));
      if (value == "URL") {
        std::string url;
        url.resize(f_obj->GetString(L"F", nullptr, 0));
        f_obj->GetString(L"F", (char*)url.c_str(), static_cast<int> (url.size()));
        if (!url.empty())
          m_html_begin_tag += " formaction=\"" + url + "\"";
      }
    }

    // check the flags
    int flags = action_obj->GetInteger(L"Flags", 0);
    // check ExportFormat flag
    if (flags & (1 << (3 - 1))) {
      // determine if GET or POST method should be used
      if (flags & (1 << (4 - 1)))
        m_html_begin_tag += " formmethod=\"get\"";
      else
        m_html_begin_tag += " formmethod=\"post\"";
    }
  } // end of action_obj

  if (button_type != ButtonType::kPushButton) {

    if (m_form_dict->Known(L"Opt")) {
      PdsArray* opt = m_form_dict->GetArray(L"Opt");
      if (opt && m_button_idx > -1 && m_button_idx < opt->GetNumObjects()) {
        string value;
        value.resize(opt->GetString(m_button_idx, nullptr, 0));
        opt->GetString(m_button_idx, (char*)value.c_str(), static_cast<int> (value.size()));

        m_html_begin_tag += " value=\"";
        m_html_begin_tag += value;
        m_html_begin_tag += "\"";
      }
    }
    else {
      PdsDictionary* ap = m_form_dict->GetDictionary(L"AP");
      if (!ap) {
        PdsArray* kids = m_form_dict->GetArray(L"Kids");
        if (kids && m_button_idx > -1 && m_button_idx < kids->GetNumObjects()) {
          PdsDictionary *kid = kids->GetDictionary(m_button_idx);
          if (kid)
            ap = kid->GetDictionary(L"AP");
        }
      }

      if (ap) {
        PdsDictionary* n = ap->GetDictionary(L"N");
        if (n) {
          int keys_count = n->GetNumKeys();
          for (int i = 0; i < keys_count; ++i) {
            std::wstring key = CTaggedPdfUtils::PdfDictionaryGetKeyAtIndex(n, i);

            if (key != L"Off") {
              m_html_begin_tag += " value=\"";
              m_html_begin_tag += w2utf8(key);
              m_html_begin_tag += "\"";
              break;
            }
          }
        }
      }
    }

    PdsObject* as = nullptr;
    if (m_form_dict->Known(L"AS")) {
      as = m_form_dict->Get(L"AS");
    }
    else {
      PdsArray* kids = m_form_dict->GetArray(L"Kids");

      if (m_button_idx > -1 && m_button_idx < kids->GetNumObjects()) {
        PdsDictionary* kid = kids->GetDictionary(m_button_idx);
        if (kid)
          as = kid->Get(L"AS");
      }
    }

    if (as && as->GetObjectType() == kPdsName) {
      std::string value;
      value.resize((static_cast<PdsName*> (as))->GetValue(nullptr, 0));
      (static_cast<PdsName*> (as))->GetValue((char*)value.c_str(), static_cast<int> (value.size()));

      if (value != "Off")
        m_html_begin_tag += " checked";
    }
  }

  m_html_begin_tag += " " + tag_attributes();
  m_html_begin_tag += ">";
  if (button_type == ButtonType::kPushButton) {
    m_html_end_tag = "</button>\n";
    std::string tu;
    tu.resize(m_form_dict->GetString(L"TU", nullptr, 0));
    m_form_dict->GetString(L"TU", (char*)tu.c_str(), static_cast<int> (tu.size()));
    m_html_begin_tag += tu;
  }
}

void CTaggedPdfForm::process_text_type() {
  if (!is_field_flag_active(FieldFlag::kMultiline)) {
    m_html_begin_tag = "<input type=";
    m_html_end_tag = "";

    if (is_field_flag_active(FieldFlag::kPassword))
      m_html_begin_tag += "\"password\" ";
    else if (is_field_flag_active(FieldFlag::kFileSelect))
      m_html_begin_tag += "\"file\" ";
    else
      m_html_begin_tag += "\"text\" ";
  }
  else {
    m_html_begin_tag = "<textarea>\n";
    m_html_end_tag = "</textarea\n";
  }

  if (!is_field_flag_active(FieldFlag::kRichText) && !m_form_dict->Known(L"RV")) {
    std::string value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(m_form_dict, L"V");
    if (value != "") {
      m_html_begin_tag += "value=\"";
      m_html_begin_tag += value;
      m_html_begin_tag += "\" ";
    }
  }

  if (!is_field_flag_active(FieldFlag::kDoNotSpellCheck))
    m_html_begin_tag += "spellcheck=\"true\" ";

  m_html_begin_tag += process_common_form_field_attributes();

  int max_len_value = m_form_dict->GetInteger(L"MaxLen", -1);
  if (max_len_value != -1) {
    m_html_begin_tag += " maxlength=\"";
    m_html_begin_tag += std::to_string(max_len_value);
    m_html_begin_tag += "\" ";
  }

  m_html_begin_tag += " " + tag_attributes() + ">\n";
}

void CTaggedPdfForm::process_choice_type() {
  if (is_field_flag_active(FieldFlag::kCombo))
    m_html_begin_tag = "<select ";
  else
    m_html_begin_tag = "<select size=\"3\" ";

  m_html_end_tag = "</select>\n";

  m_html_begin_tag += process_common_form_field_attributes();
  // end select beginning tag
  m_html_begin_tag += " " + tag_attributes() + ">\n";

  PdsArray *opt = m_form_dict->GetArray(L"Opt");
  if (opt) {
    for (int i = 0; i < opt->GetNumObjects(); ++i) {
      std::string value;
      value.resize(opt->GetString(i, nullptr, 0));
      opt->GetString(i, (char*)value.c_str(), static_cast<int> (value.size()));
      //todo: can we have an empty string? do we want to add it? 
      //now we are adding also empty strings
      m_html_begin_tag += "<option>\n";
      m_html_begin_tag += value;
      m_html_begin_tag += "\n</option>\n";
    }
  }
}

std::string CTaggedPdfForm::process_common_form_field_attributes() {
  std::string out_str("");

  // name
  std::string name;
  name.resize(m_form_dict->GetString(L"T", nullptr, 0));
  m_form_dict->GetString(L"T", (char*)name.c_str(), static_cast<int> (name.size()));
  if (name != "") {
    out_str += " name=\"";
    out_str += name;
    out_str += "\"";
  }

  //IDs for javascript
  if (!m_config.id.empty()) {
    out_str += " data-field-id=\"" + m_config.id + "\"";
    out_str += " data-annot-id=\"" + m_config.id + "\"";
  }

  // Generate javascript finctions based on Action
  std::string js;
  js = "all_fields.push({name:\"" + name + "\", id:\"" + m_config.id + "\", annots:[]});\n";
  //field actions
  js += get_form_field_event_html(m_config.id, L"K");
  js += get_form_field_event_html(m_config.id, L"F");
  js += get_form_field_event_html(m_config.id, L"V");
  js += get_form_field_event_html(m_config.id, L"C");

  //annot actions
  js += get_form_field_event_html(m_config.id, L"E"); //Enter
  js += get_form_field_event_html(m_config.id, L"X"); //Exit 
  js += get_form_field_event_html(m_config.id, L"D");
  js += get_form_field_event_html(m_config.id, L"Fo");
  js += get_form_field_event_html(m_config.id, L"Bl");
  //todo: page actions are not handled PO, PC, PV, Pl
  // from A or AA ?
  js += get_form_field_event_html(m_config.id, L"U");

  m_writer->push_js(js);

  return out_str;
}

string CTaggedPdfForm::get_form_field_event_html(const string& field_id, const wstring& trigger_event) {
  PdsDictionary *action = nullptr;
  if (trigger_event == L"U")
    action = m_form_dict->GetDictionary(L"A");
  else {
    // AA in Field dictionary
    PdsDictionary* aa = m_form_dict->GetDictionary(L"AA");
    if (!aa) return "";
    // event under AA
    action = aa->GetDictionary(trigger_event.c_str());
  }
  if (!action) return "";

  // JS under event
  if (!action->Known(L"JS")) return "";
  PdsObject* js = action->Get(L"JS");

  if (!js) return "";
  std::string js_str;
  if (js->GetObjectType() == kPdsString) {
    js_str = CTaggedPdfUtils::PdfStringGetValue(static_cast<PdsString*> (js));
  }
  else if (js->GetObjectType() == kPdsStream) {
    int len = (static_cast<PdsStream*> (js))->GetSize();
    unique_charp buffer((char*)malloc(len));
    (static_cast<PdsStream*> (js))->Read(0, (uint8_t*)buffer.get(), len);
    js_str = buffer.get();
  }
  else return "";

  std::string event_js;
  if (js_str.length()) {
    event_js = "function ";
    event_js += w2utf8(trigger_event.c_str());
    event_js += field_id + "() {\n";
    //event_js += w2utf8(js_str.c_str());
    event_js += js_str;
    event_js += "\n}\n";
  }
  return event_js;
}
