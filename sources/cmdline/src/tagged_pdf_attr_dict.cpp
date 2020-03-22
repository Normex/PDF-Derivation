////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_attr_dict.h"
#include "tagged_pdf_utils.h"
#include "tagged_utils.h"

#include <sstream>
#include <algorithm>
#include <cctype>

using std::wstring;
using std::stringstream;

// ctor
CAttributeDictProcessor::CAttributeDictProcessor() {
}

// dtor
CAttributeDictProcessor::~CAttributeDictProcessor() {
}

// methods
void CAttributeDictProcessor::process_table_owner(PdsDictionary* attr_dict, attribute_map& out_html_attrs, attribute_map& out_css_attrs, wstring type) {
  for (int i = 0; i < attr_dict->GetNumKeys(); ++i) {
    wstring key = CTaggedPdfUtils::PdfDictionaryGetKeyAtIndex(attr_dict, i);

    // 4.3.7.5 Table standard structure attribute owner
    // Table 2
    if ((key == L"ColSpan" || key == L"RowSpan") && (type == L"TD" || type == L"TH")) {
      int num = attr_dict->GetInteger(key.c_str(), 0);
      if (key == L"ColSpan")
        out_html_attrs.insert({ "colspan", std::to_string(num) });
      else
        out_html_attrs.insert({ "rowspan", std::to_string(num) });
    }
    else if (key == L"Headers") {
      PdsArray* headers = attr_dict->GetArray(key.c_str());
      stringstream values;

      for (int i = 0; i < headers->GetNumObjects(); ++i) {
        string item_value;
        item_value.resize(headers->GetString(i, nullptr, 0));
        headers->GetString(i, const_cast<char*> (item_value.c_str()), static_cast<int> (item_value.size()));

        values << item_value << " ";
      }

      out_html_attrs.insert({ "headers", values.str() });
    }
    else if (key == L"Scope") {
      string value;
      value.resize(attr_dict->GetString(key.c_str(), nullptr, 0));
      attr_dict->GetString(key.c_str(), const_cast<char*> (value.c_str()), static_cast<int> (value.size()));

      if (value == "Column")
        value = "col";
      if (value == "Row")
        value = "row";
      if (value == "Both") //todo: ??
        value = "row";

      if (!value.empty())
        out_html_attrs.insert({ "scope", value });
    }
    else if (key == L"Short") {
      wstring value = CTaggedPdfUtils::PdfDictionaryGetTextForKey(attr_dict, key);
      out_html_attrs.insert({ "abbr", w2utf8(value) });
    }
    else if (key == L"TBorderStyle") {
      PdsObject* attr_value = attr_dict->Get(key.c_str());
      if (attr_value->GetObjectType() == kPdsName) {
        string value;
        value.resize((static_cast<PdsName*> (attr_value))->GetValue(nullptr, 0));
        (static_cast<PdsName*> (attr_value))->GetValue(const_cast<char*> (value.c_str()), static_cast<int> (value.size()));

        out_css_attrs.insert({ "border-style", value });
      }
      else if (attr_value->GetObjectType() == kPdsArray) {
        stringstream values;

        PdsArray* styles = static_cast<PdsArray*> (attr_value);
        for (int i = 0; i < styles->GetNumObjects(); ++i) {
          string value;

          value.resize(styles->GetString(i, nullptr, 0));
          styles->GetString(i, const_cast<char*> (value.c_str()), static_cast<int> (value.size()));

          if (!value.empty())
            values << value << ";";
        }

        out_css_attrs.insert({ "border-style", values.str() });
      }
      else {
        continue;
      }
    }
    else if (key == L"TPadding") {
      PdsObject* attr_value = attr_dict->Get(key.c_str());

      if (attr_value->GetObjectType() == kPdsNumber) {
        int num = (static_cast<PdsNumber*> (attr_value))->GetIntegerValue();
        out_css_attrs.insert({ "padding", std::to_string(num) });
      }
      else if (attr_value->GetObjectType() == kPdsArray) {
        stringstream values;
        PdsArray* paddings = static_cast<PdsArray*> (attr_value);

        for (int i = 0; i < paddings->GetNumObjects(); ++i) {
          values << paddings->GetInteger(i) << "; ";
        }
        out_css_attrs.insert({ "padding", values.str() });
      }
      else {
        continue;
      }
    }
  }
}

void CAttributeDictProcessor::process_layout_owner(PdsDictionary* attr_dict, attribute_map& out_html_attrs, attribute_map& out_css_attrs) {
  auto pdfRGB2HtmlRGBString = [](PdsArray* pdf_color) -> string {
    int pdf_color_size = pdf_color->GetNumObjects();

    if (pdf_color_size != 3)
      return "";

    stringstream rgb_str;
    rgb_str << "rgb(";

    for (int i = 0; i < pdf_color_size; ++i) {
      int color_value = static_cast<int> (pdf_color->GetNumber(i) * 100);  //0.0-0.1 * 100 = %
      rgb_str << std::to_string(color_value) << "%";

      if (i < 2)
        rgb_str << ",";
    }

    rgb_str << ")";

    return rgb_str.str();
  };

  for (int i = 0; i < attr_dict->GetNumKeys(); ++i) {
    wstring key = CTaggedPdfUtils::PdfDictionaryGetKeyAtIndex(attr_dict, i);

    // 4.3.7.6 Table standard structure attribute owner
    // Table 4
    if (key == L"Placement") {
      string value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(attr_dict, key);
      string block = "display";
      string block_value;

      if (value == "Block")
        block_value = "block";
      else if (value == "Inline")
        block_value = "inline";
      else if (value == "Before") {
        block = "float";
        block_value = "left";
      }
      else if (value == "Start") {
        block = "float";
        block_value = "left";
      }
      else if (value == "End") {
        block = "float";
        block_value = "right";
      }
      else
        continue;

      out_css_attrs.insert({ block, block_value });
    }
    else if (key == L"WritingMode") {
      string value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(attr_dict, key);
      string mode;

      if (value == "LrTb")
        mode = "horizontal-tb";
      else if (value == "RlTb")
        mode = "vertical-rl";
      else if (value == "TbRl")
        mode = "vertical-lr";
      else if (value == "TbLr")
        mode = "horizontal-tb";
      else if (value == "LrBt")
        mode = "horizonatal-tb";
      else if (value == "RlBt")
        mode = "horizontal-tb";
      else if (value == "BtRl")
        mode = "horizontal-tb";
      else if (value == "BtLr")
        mode = "horizontal-tb";
      else
        continue;

      out_css_attrs.insert({ "writing-mode", mode });
    }
    else if (key == L"BackgroundColor"
      || key == L"BorderColor"
      || key == L"Color"
      || key == L"TextDecorationColor")
    {
      string css;
      if (key == L"BackgroundColor")
        css = "background-color";
      else if (key == L"BorderColor")
        css = "border-color";
      else if (key == L"Color")
        css = "color";
      else if (key == L"TextDecorationColor")
        css = "text-decoration-color";

      PdsArray* colors = attr_dict->GetArray(key.c_str());
      if (colors) {
        string css_colors = pdfRGB2HtmlRGBString(colors);
        if (!css_colors.empty())
          out_css_attrs.insert({ css, css_colors });
      }
    }
    else if (key == L"BorderStyle") {
      PdsObject* style = attr_dict->Get(key.c_str());
      if (style->GetObjectType() == kPdsName) {
        string value = w2utf8(CTaggedPdfUtils::PdfNameGetText(static_cast<PdsName*> (style)));
        std::transform(value.begin(), value.end(), value.begin(), std::tolower);
        out_css_attrs.insert({ "border-style", value });
      }
      else if (style->GetObjectType() == kPdsArray) {
        PdsArray* styles = static_cast<PdsArray*> (style);

        int styles_count = styles->GetNumObjects();
        stringstream values;
        for (int i = 0; i < styles_count; ++i) {
          string value = w2utf8(CTaggedPdfUtils::PdfArrayGetTextAtIndex(styles, i));
          std::transform(value.begin(), value.end(), value.begin(), std::tolower);

          if (i < (styles_count - 1))
            values << value << " ";
        }

        out_css_attrs.insert({ "border-style", values.str() });
      }
      else
        continue;
    }
    else if (key == L"BorderThickness"
      || key == L"Padding"
      || key == L"TPadding"
      || key == L"BaselineShift")
    {
      string css;
      if (key == L"BorderThickness")
        css = "border-width";
      else if (key == L"Padding" || key == L"TPadding")
        css = "padding";
      else if (key == L"BaselineShift")
        css = "baseline-shift";

      PdsObject* bt = attr_dict->Get(key.c_str());
      stringstream values;
      if (bt->GetObjectType() == kPdsNumber) {
        double value = userSpace2DeviceSpace((static_cast<PdsNumber*> (bt))->GetValue());
        values << static_cast<int> (value) << "px";
      }
      else if (bt->GetObjectType() == kPdsArray) {
        PdsArray* bts = static_cast<PdsArray*> (bt);
        array<double, 4> us_values;

        int bts_size = bts->GetNumObjects();
        if (bts_size == us_values.size()) {
          for (int i = 0; i < bts_size; ++i) {
            us_values[i] = bts->GetNumber(i);
          }

          array<double, 4> ds_values = userSpace2DeviceSpace(us_values);
          for (const auto& ds_val : ds_values)
            values << static_cast<int> (ds_val) << "px ";

        }

        out_css_attrs.insert({ css, values.str() });
      }
    }
    else if (key == L"SpaceBefore") {
      //todo: clarify
    }
    else if (key == L"SpaceAfter") {
      //todo: clarify
    }
    else if (key == L"StartIndent") {
      //todo: clarify
    }
    else if (key == L"EndIndent") {
      //todo: clarify
    }
    else if (key == L"TextIndent") {
      double value = userSpace2DeviceSpace(attr_dict->GetNumber(key.c_str()));
      out_css_attrs.insert({ "text-indent", std::to_string(value) + "px" });
    }
    else if (key == L"TextAlign") {
      string value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(attr_dict, key);
      string ta_value;

      if (value == "Start")
        ta_value = "left";
      else if (value == "Center")
        ta_value = "center";
      else if (value == "End")
        ta_value = "right";
      else if (value == "Justify")
        ta_value = "justify";
      else
        ta_value = "left";

      out_css_attrs.insert({ "text-align", ta_value });
    }
    else if (key == L"LineHeight") {
      PdsObject* lh = attr_dict->Get(key.c_str());
      /*      if (lh->GetObjectType() == kPdsNumber) {
              double value = userSpace2DeviceSpace((static_cast<PdsNumber*> (lh))->GetValue());
              m_m_css_attrs << "line-height: " << value << "; ";
            }*/
            // Specification says only about PDF names.
      if (lh->GetObjectType() == kPdsName) {
        string value = w2utf8(CTaggedPdfUtils::PdfNameGetText(static_cast<PdsName*> (lh)));
        string lh_value;

        if (value == "Normal")
          lh_value = "normal";
        else if (value == "Auto")
          lh_value = "initial";
        else
          lh_value = "initial";

        out_css_attrs.insert({ "line-height", lh_value });
      }
    }
    else if (key == L"TextDecorationThickness") {
      //todo: clarify
    }
    else if (key == L"TextDecorationType") {
      string value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(attr_dict, key);

      if (value == "LineThrough")
        value = "line-through";
      else {
        std::transform(value.begin(), value.end(), value.begin(), std::tolower);
      }

      out_css_attrs.insert({ "text-decoration", value });
    }
    else if (key == L"RubyAlign") {
      string value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(attr_dict, key);
      string rb_value;

      if (value == "Start")
        rb_value = "start";
      else if (value == "Center")
        rb_value = "center";
      else if (value == "End")
        rb_value = "center"; // todo: clarify
      else if (value == "Justify")
        rb_value = "space-between";
      else if (value == "Distribute")
        rb_value = "space-around";
      else
        rb_value = "space-around";

      out_css_attrs.insert({ "ruby-align", rb_value });
    }
    else if (key == L"RubyPostion") {
      string value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(attr_dict, key);
      string rb_value;

      if (value == "Before")
        rb_value = "over";
      else if (value == "After")
        rb_value = "under";
      else if (value == "Warichu")
        rb_value = "inner-character"; //todo: check
      else if (value == "Inline")
        rb_value = "inner-character"; //todo: check
      else
        rb_value = "over";

      out_css_attrs.insert({ "ruby-position", rb_value });
    }
    else
      continue;
  }
}

void CAttributeDictProcessor::process_html_or_css_owner(PdsDictionary* attr_dict, bool html_owner, attribute_map& out_html_attrs, attribute_map& out_css_attrs) {
  for (int i = 0; i < attr_dict->GetNumKeys(); ++i) {
    wstring key = CTaggedPdfUtils::PdfDictionaryGetKeyAtIndex(attr_dict, i);
    if (key == L"O")
      continue;

    string value;
    switch (attr_dict->Get(key.c_str())->GetObjectType()) {
    case kPdsName:
    case kPdsString:
      value = CTaggedPdfUtils::PdfDictionaryGetStringForKey(attr_dict, key);
      break;
    case kPdsNumber:
      value = std::to_string(static_cast<int> (attr_dict->GetNumber(key.c_str())));
      break;
    case kPdsArray:
    {
      stringstream values;
      PdsArray* ar = attr_dict->GetArray(key.c_str());
      for (int i = 0; i < ar->GetNumObjects(); ++i) {
        auto item_type = ar->Get(i)->GetObjectType();
        if (item_type == kPdsName || item_type == kPdsString) {
          value.resize(ar->GetString(i, nullptr, 0));
          ar->GetString(i, const_cast<char*> (value.c_str()), static_cast<int> (value.size()));
          values << value << " ";
        }
        else if (item_type == kPdsNumber)
          values << static_cast<int> (ar->GetNumber(i)) << " ";
        else
          continue;
      }

      value.clear();
      value = values.str();
    }
    break;
    default:
      continue;
    }

    if (!value.empty()) {
      if (html_owner)
        out_html_attrs.insert({ w2utf8(key), value });
      else
        out_css_attrs.insert({ w2utf8(key), value });
    }
  }
}

//todo:
double CAttributeDictProcessor::userSpace2DeviceSpace(double s) {
  /*int page_num = m_elem->GetPageNumber();
  PdfPage* page = m_doc->AcquirePage(page_num);

  PdfRect crop_box;
  page->GetCropBox(&crop_box);

  int page_width = crop_box.right - crop_box.left;
  int page_height = crop_box.top - crop_box.bottom;

  HDC hdc = GetDC(NULL);
  int width = GetDeviceCaps(hdc, PHYSICALWIDTH);
  int height = GetDeviceCaps(hdc, PHYSICALHEIGHT);
  int zoom = std::min(width / page_width, height / page_height);

  //PdfPageView* page_view = page->AcquirePageView(zoom, kRotate0);
  m_doc->ReleasePage(page);*/

  return s;
}

array<double, 4> CAttributeDictProcessor::userSpace2DeviceSpace(array<double, 4> input) {
  return input;
}
