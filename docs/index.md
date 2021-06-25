## Deriving HTML from PDF 

Implementation of the [Derivation algorithm](https://www.pdfa.org/resource/deriving-html-from-pdf/) that converts well-tagged pdf files into HTML

## Download & run
This repository contains a commandline tool. Follow direct [link](https://github.com/Normex/PDF-Derivation/releases/latest/download/bin.zip) to the latest version of executables.


To run commandline application just type:

```batch
derive.exe c:\temp\tagged.pdf c:\temp\output
``` 
first parameter is pdf file you want to derive into html
second parameter is existing output folder.

## Sample files

Even though the derivation algorithm is designed to work properly for all tagged pdf files, it is expected that the best result in terms of visual accuracy is achieved with pdf files that are tagged for this purpose. In the [pdf folder](https://github.com/Normex/PDF-Derivation/blob/master/pdf/README.md) you can find set of examples (manually crafted pdf files) that show how specific structure elements, attributes, associated files are used during the derivation and how the author can turn static pdfs into a dynamic html. 



## Feedback

Feel free to submit bugs, problematic files, comments, questions, suggestions by creating new issue in [Issues section](https://github.com/Normex/PDF-Derivation/issues) or [e-mail us](mailto:pdf-derivation@digitaldocuments.org) 

