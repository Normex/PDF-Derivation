////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Normex (www.digitaldocuments.org). All Rights Reserved.
// project: Deriving HTML from PDF (https://github.com/Normex/PDF-Derivation)
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <exception>
#include <iostream>
#include <string>

#include "Pdfix.h"
#include "tagged_pdf_html_doc.h"

Pdfix_statics;

void show_help() {
  std::cout << "PDF-Derivation by Normex s.r.o. (c) 2020" << std::endl;
  std::cout << "derives html from well tagged pdf files. see https://github.com/Normex/PDF-Derivation for more info" << std::endl;
  std::cout << std::endl;
  std::cout << "usage:" << std::endl;
  std::cout << "  derive <input_file> <output_folder>" << std::endl;
  std::cout << std::endl;
  std::cout << "    input_file      - full pathname to input pdf" << std::endl;
  std::cout << "    output_folder   - folder for output html" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  std::vector<std::string> options;

  if (argc == 1) {
    show_help();
    return 0;
  }

  if (strcmp(argv[1], "/?") == 0) {
    show_help();
    return 0;
  }

  PDFixSDK::Pdfix* pdfix = nullptr;
  try {
    std::string input, output;
    auto i = 1;
    if (argc > i) input = argv[i++];
    if (argc > i) output = argv[i++];

    if (input.empty())
      throw std::runtime_error("Missing input file");

    input = GetAbsolutePath(input);
    output = GetAbsolutePath(output);
    if (output == "")
      output = get_path_dir(input);

    std::wstring open_path = utf82w(input.c_str());
    std::wstring save_path = utf82w(output.c_str());
    save_path = check_folder_path(save_path);
    if (!folder_exists(save_path))
      throw std::runtime_error("Output folder doesn't exist");


    // initialize Pdfix
    if (!Pdfix_init(Pdfix_MODULE_NAME))
      throw std::runtime_error("Pdfix initialization fail");

    pdfix = PDFixSDK::GetPdfix();
    if (!pdfix)
      throw std::runtime_error("GetPdfix fail");

    if (pdfix->GetVersionMajor() != PDFIX_VERSION_MAJOR ||
      pdfix->GetVersionMinor() != PDFIX_VERSION_MINOR ||
      pdfix->GetVersionPatch() != PDFIX_VERSION_PATCH)
      throw std::runtime_error("Incompatible version");

    PdfDoc* doc = pdfix->OpenDoc(open_path.c_str(), L"");
    if (!doc)
      throw std::runtime_error(pdfix->GetError());

    //derive HTML from PDF
    CTaggedPdfHtmlDoc derivation(doc);
    derivation.save(save_path);
    doc->Close();
  }
  catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
    if (!pdfix)
      pdfix->Destroy();
    return 1;
  }
  std::cout << "Success" << std::endl;
  if (!pdfix)
    pdfix->Destroy();
  return 0;
}
