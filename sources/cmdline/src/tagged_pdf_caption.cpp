#include "tagged_pdf_caption.h"
#include "tagged_pdf_utils.h"

// ctor
CTaggedPdfCaption::CTaggedPdfCaption(PdfDoc* doc, PdsStructElement* elem, const CTaggedPdfElementConfig& config) : CTaggedPdfElement(doc, elem, config) {
  // 4.3.5.2 Caption
  // check parent or siblings if it is Figure or Formula
  PdsObject* parent_obj = m_elem->GetParentObject();
  if (parent_obj) {
    PdsStructTree* structure_tree = m_doc->GetStructTree();

    wstring type;
    if (structure_tree) {
      PdsStructElement* parent_elem = structure_tree->AcquireStructElement(parent_obj);

      type = CTaggedPdfUtils::StructElementGetType(parent_elem);

      auto get_caption_type = [](const wstring& t) {
        if (t == L"Figure" || t == L"Formula")
          return "figcaption";
        if (t == L"Table")
          return "caption";

        return "div";
      };

      m_html_tag = get_caption_type(type);
      if (m_html_tag == "div") {
        // check siblings
        for (int i = 0; i < parent_elem->GetNumKids(); ++i) {
          if (parent_elem->GetKidType(i) == kPdsStructKidElement) {
            PdsObject* kid = parent_elem->GetKidObject(i);
            if (!kid)
              continue;

            PdsStructElement* kid_elem = structure_tree->AcquireStructElement(kid);
            if (!kid_elem)
              continue;

            type = CTaggedPdfUtils::StructElementGetType(kid_elem);
            kid_elem->Release();
            m_html_tag = get_caption_type(type);
            kid_elem = nullptr;
          }
        }
      }
      parent_elem->Release();
    }
  }
}

// dtor
CTaggedPdfCaption::~CTaggedPdfCaption() {

}
