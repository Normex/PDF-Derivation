# Deriving HTML from PDF

Implementation of an algorithm that converts well-tagged pdf files into HTML

Since 2016 we have been actively participating in [PDF Association Technical Working Group](https://www.pdfa.org/community/next-generation-pdf-technical-working-group/) with the aim to address needs of industry for changing the way PDF files are consumed. The main concern was whether or not the traditional fixed-layout pdf contains enough information to be safely and unambiguously interpreted as html - therefore responsive and reusable in different environments.

The output of the work is the [Deriving HTML from PDF](https://www.pdfa.org/resource/deriving-html-from-pdf/) - document that describes how the process of conversion could be done. Version 1.0 of the publication was released by [PDF Association](https://www.pdfa.org) in June 2019.

We presented about the idea at the [PDF Days Europe 2017 in Berlin](https://www.youtube.com/watch?v=cFr7SI8pMZk) and [Electronic Document Conference 2019 in Seattle](https://www.youtube.com/watch?v=hTh7KIYvj0E&list=PLfGR40SWnakjHir089hAtNZUF7pOXmW6r&index=17)

As a part of the work we came up with referential set of pdf documents and implementation. These should provide enough insights into the whole concept.

We also publishing articles about the concept like these [[1](https://www.pdfa.org/pdf-on-the-web-then-and-now/) [2](https://www.pdfa.org/deriving-html-from-pdf-an-algorithm/)] and based on real world implementations we are working on future versions of the algorithm.


# Download
This repository contains a commandline tool and GUI application. You can find latest version through [release link](https://github.com/Normex/PDF-Derivation/releases) where you want to download bin.zip file.

Follow direct link to the latest version of executables.
https://github.com/Normex/PDF-Derivation/releases/latest/download/bin.zip

We currently only build 64 bit windows version, download the zip file and unpack to your prefered location

## Command line application

To run commandline application just type:

```batch
derive.exe c:\temp\tagged.pdf c:\temp\output
``` 
first parameter is pdf file you want to derive into html
second parameter is existing output folder.

## GUI application

We utilise chromium to show result html alongside with original pdf. The use of the gui app is pretty straightforward. 

run <b>PDFDerivation_app.exe</b> application. From menu open pdf file and check html tab for result. HTML file is stored in application subfolder.


# Sample files

Even though the derivation algorithm is designed to work properly for all tagged pdf files, it is expected that the best result in terms of visual accuracy is achieved with pdf files that are tagged for this purpose. In the [pdf folder](https://github.com/Normex/PDF-Derivation/blob/master/pdf) you can find set of examples (manually crafted pdf files) that show how specific structure elements, attributes, associated files are used during the derivation and how the author can turn static pdfs into a dynamic html. 

# Sources

Currently we only provide project files for Windows, but the code is platform independent and in future we expect to add support for mac and linux. 

## Feedback

Feel free to submit bugs, problematic files, comments, questions, suggestions by creating new issue in [Issues section](https://github.com/Normex/PDF-Derivation/issues) or [e-mail us](mailto:pdf-derivation@digitaldocuments.org) 

# Licensing
All assets and code are provided under the [MIT License](https://github.com/Normex/PDF-Derivation/blob/master/LICENSE) (we use free version of [PDFix SDK](https://pdfix.net/)) 

Should you have any questions or requests contact us: [e-mail us](mailto:pdf-derivation@digitaldocuments.org)
