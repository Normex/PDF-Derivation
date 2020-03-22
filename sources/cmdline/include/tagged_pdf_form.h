////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef tagged_pdf_form_h
#define tagged_pdf_form_h

#include "tagged_pdf_element.h"

class CTaggedPdfForm : public CTaggedPdfElement
{
private:
  enum class FormFieldType {
    Unknown,
    Btn,
    Tx,
    Ch,
    Sig
  };

  // query methods and query types (enum)
  // Annotation flags
  enum class AnnotFlag : unsigned int {
    kHidden = 2
  };

  enum class FieldFlag : unsigned int {
    kMultiline = 13,
    kPassword = 14,
    kRadioButton = 16,
    kPushButton = 17,
    kCombo = 18,
    kFileSelect = 21,
    kDoNotSpellCheck = 23,
    kRichText = 26
  };

  enum class ButtonType {
    kPushButton, kRadioButton, kCheckBox, kUnknownButton
  };

public:
  CTaggedPdfForm(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config);
  ~CTaggedPdfForm();

  virtual void tag_begin();
  virtual void tag_end();

protected:
  //virtual string process_tag();

private:
  PdsDictionary* m_form_dict = nullptr;
  FormFieldType m_form_field_type = FormFieldType::Unknown;
  string m_html_begin_tag;
  string m_html_end_tag;
  int m_button_idx = -1;

  bool is_annot_flag_active(AnnotFlag flag);
  bool is_field_flag_active(FieldFlag flag);

  void process_button_type();
  void process_text_type();
  void process_choice_type();

  string process_common_form_field_attributes();
  string get_form_field_event_html(const string& field_id, const wstring& trigger_event);
};

#endif // tagged_pdf_form_h
