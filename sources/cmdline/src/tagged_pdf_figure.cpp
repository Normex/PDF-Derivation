////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
#include "tagged_pdf_figure.h"
#include "tagged_utils.h"
#include "tagged_pdf_utils.h"
#include "tagged_pdf_element_factory.h"

#include <sstream>
#include <regex>

using std::stringstream;

// ctor
CTaggedPdfFigure::CTaggedPdfFigure(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  m_html_tag = "figure";
  m_use_figure_elem = true;

  //check parent. if Sub, P, H, Hn, H, Em, Strong or Span than don't use <figure>
  PdsObject* parent_obj = m_elem->GetParentObject();
  if (parent_obj) {
    PdsStructTree* structure_tree = m_doc->GetStructTree();
    wstring type;
    if (structure_tree) {
      PdsStructElement* parent_elem = structure_tree->AcquireStructElement(parent_obj);
      type = CTaggedPdfUtils::StructElementGetType(parent_elem);
      m_use_figure_elem = !((type == L"Sub") || (type == L"Span") || (type == L"Strong") || (type == L"P") || (type == L"Em") ||
        (type == L"H") || (type == L"H1") || (type == L"H2") || (type == L"H3") || (type == L"H4") || (type == L"H5") || (type == L"H6"));
    }
  }
}

// dtor
CTaggedPdfFigure::~CTaggedPdfFigure() {
}

// methods
void CTaggedPdfFigure::tag_begin() {
  if (m_use_figure_elem)
    m_writer->push_html("<" + m_html_tag + " " + tag_attributes() + ">\n");
}


void CTaggedPdfFigure::tag_end() {
  if (m_use_figure_elem)
    m_writer->push_html("</" + m_html_tag + ">\n");
}

void CTaggedPdfFigure::tag_substructure() {
  //here we render the whole substructure except of Caption se
  stringstream stream;
  stream << render(m_elem);
  stream << render_now();
  m_writer->push_html(stream.str());
}

string CTaggedPdfFigure::render(PdsStructElement* elem) {
  //marking all content elements for rendering
  string str;
  for (int i = 0; i < elem->GetNumKids(); i++) {
    switch (elem->GetKidType(i)) {
    case PdfStructElementType::kPdsStructKidElement: {
      PdsObject* kid_object = elem->GetKidObject(i);
      if (!kid_object)
        throw std::exception("Can't acquire kid object for rendering");
      PdsStructElement* kid_struct_element = m_doc->GetStructTree()->AcquireStructElement(kid_object);
      if (!kid_struct_element)
        throw std::exception("Can't acquire kid object for rendering");

      //if Caption ==> process figcaption
      wstring elem_type = CTaggedPdfUtils::StructElementGetType(kid_struct_element);
      if (elem_type == L"Caption") {
        unique_ptr<CTaggedPdfElement> tagged_kid_element = CTaggedPdfElementFactory::make_tagged_structure_element(m_doc, kid_struct_element, m_config);
        tagged_kid_element->tag_element();
      }
      else  str += render(kid_struct_element);
      kid_struct_element->Release();
      break;
    }
    case PdfStructElementType::kPdsStructKidPageContent: {
      int kid_mcid = elem->GetKidMcid(i);
      int kid_page = elem->GetKidPageNumber(i);
      //in theory you may have elements from different pages under single Figure 
      // then we produce several images
      if (CTaggedPdfHtmlDoc::m_actual_page.m_page_num != kid_page) {
        if (CTaggedPdfHtmlDoc::m_actual_page.m_page) {
          //rt len ked tam nieco je nachystane !!! str += render_now();
          str += render_now();
          m_obj_rect.bottom = m_obj_rect.left = m_obj_rect.right = m_obj_rect.top = 0.;
          if (CTaggedPdfHtmlDoc::m_actual_page.m_page) {
            CTaggedPdfHtmlDoc::m_actual_page.m_page->Release();
            CTaggedPdfHtmlDoc::m_actual_page.m_page = nullptr;
          }
        }
        CTaggedPdfHtmlDoc::m_actual_page.m_page_num = kid_page;
        CTaggedPdfHtmlDoc::m_actual_page.m_page = m_doc->AcquirePage(CTaggedPdfHtmlDoc::m_actual_page.m_page_num);
        for (int i = 0; i < CTaggedPdfHtmlDoc::m_actual_page.m_page->GetNumPageObjects(); i++) {
          PdsPageObject* page_object = CTaggedPdfHtmlDoc::m_actual_page.m_page->GetPageObject(i);
          page_object->SetRender(false);
        }
      }
      if (!CTaggedPdfHtmlDoc::m_actual_page.m_page)
        throw std::exception("Can't locate page object required for rendering");

      //todo: check new mcid handling
      for (int i = 0; i < CTaggedPdfHtmlDoc::m_actual_page.m_page->GetNumPageObjects(); i++) {
        PdsPageObject* page_object = CTaggedPdfHtmlDoc::m_actual_page.m_page->GetPageObject(i);
        PdsContentMark* mc = page_object->GetContentMark();
        if (mc)
          if (mc->GetTagMcid() == kid_mcid) {
            PdfRect obj_rect;
            page_object->GetBBox(&obj_rect);
            expand_rect(obj_rect);
            page_object->SetRender(true);
          }
        //switch (obj->GetObjectType()) {
        //case kPdsPageUnknown:
        //  break;
        //case kPdsPageText: {
        //  break;
        //}
        //case kPdsPageImage:
        //  break;
        //case kPdsPagePath:
        //  break;
        //case kPdsPageShading:
        //  break;
        //case kPdsPageForm: {
        //  auto form = (PdsForm*)obj;
        //  for (int i = 0; i < form->GetNumPageObjects(); i++) {
        //    auto kid = form->GetPageObject(i);
        //    ProcessPageObject(kid);
        //  }
        //  break;
        //}
        //}
      }
      break;
    }
    case PdfStructElementType::kPdsStructKidStreamContent: {
      break;
    }
    case PdfStructElementType::kPdsStructKidObject: {
      break;
    }
    default:
      break;
    }
  }
  return str;
}

void CTaggedPdfFigure::expand_rect(PdfRect& other_rect) {
  if (m_obj_rect.left == 0 && m_obj_rect.right == 0 && m_obj_rect.bottom == 0 && m_obj_rect.top == 0) {
    m_obj_rect = other_rect;
  }
  else {
    m_obj_rect.left = m_obj_rect.left < other_rect.left ? m_obj_rect.left : other_rect.left;
    m_obj_rect.right = m_obj_rect.right > other_rect.right ? m_obj_rect.right : other_rect.right;
    m_obj_rect.bottom = m_obj_rect.bottom < other_rect.bottom ? m_obj_rect.bottom : other_rect.bottom;
    m_obj_rect.top = m_obj_rect.top > other_rect.top ? m_obj_rect.top : other_rect.top;
  }
}

string CTaggedPdfFigure::render_now() {
  std::string str;
  if (!CTaggedPdfHtmlDoc::m_actual_page.m_page)
    return str;

  static int figure_counter = 0;

  PdfPageRenderParams render_params;
  PdfImageParams image_params;
  PdfDevRect obj_dev_rect;

  PdfPageView* page_view = CTaggedPdfHtmlDoc::m_actual_page.m_page->AcquirePageView(1.0, kRotate0);
  page_view->RectToDevice(&m_obj_rect, &obj_dev_rect);
  std::wstring image_name = L"fig" + std::to_wstring(figure_counter++) + L".png";

  int width, height;
  width = page_view->GetDeviceWidth();
  height = page_view->GetDeviceHeight();
  page_view->GetDeviceMatrix(&render_params.matrix);

  //4.3.6.4 Alternate description
  wstring alt_text;
  alt_text.resize(m_elem->GetAlt(nullptr, 0));
  m_elem->GetAlt(const_cast<wchar_t*> (alt_text.c_str()), alt_text.size());

  str += "<img ";
  str += "src=\"" + w2utf8(image_name) + "\"";
  if (!alt_text.empty()) {
    alt_text.erase(std::remove(alt_text.begin(), alt_text.end(), '\0'), alt_text.end());
    str += " alt=\"" + CTaggedPdfUtils::EscapeHtmlText(w2utf8(alt_text)) + "\"";
  }
  str += " width=\"" + std::to_string(obj_dev_rect.right - obj_dev_rect.left) + "\"";
  str += " height=\"" + std::to_string(obj_dev_rect.bottom - obj_dev_rect.top) + "\"";
  str += ">";

  page_view->Release();

  PsImage* image = GetPdfix()->CreateImage(width, height, kImageDIBFormatArgb);
  std::wstring image_path = m_config.data_path + image_name;
  PsStream *image_file_stream = GetPdfix()->CreateFileStream(image_path.c_str(), kPsTruncate);
  if (!image_file_stream)
    throw std::exception("Can't create image file for rendering");
  if (!image)
    throw std::exception("Can't create image for rendering");

  render_params.image = image;
  //render_params.render_flags = /*kRenderNoText |*/ kRenderNoBackground;
  CTaggedPdfHtmlDoc::m_actual_page.m_page->DrawContent(&render_params, nullptr, nullptr);
  //whole page
  //image->SaveToStream(image_file_stream, &image_params);
  //just the Rect in which the Object is
  image->SaveRectToStream(image_file_stream, &image_params, &obj_dev_rect);
  image->Destroy();
  image_file_stream->Destroy();

  //set reneder flag to false for all elements
  for (int i = 0; i < CTaggedPdfHtmlDoc::m_actual_page.m_page->GetNumPageObjects(); i++) {
    PdsPageObject* page_object = CTaggedPdfHtmlDoc::m_actual_page.m_page->GetPageObject(i);
    page_object->SetRender(false);
  }

  return str;
}
