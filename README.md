# Deriving HTML from PDF

Implementation of an algorithm that converts well-tagged pdf files into HTML

Since 2016 we have been actively participating in [PDF Association Technical Working Group](https://www.pdfa.org/community/next-generation-pdf-technical-working-group/) with the aim to address needs of industry for changing the way PDF files are consumed on mobile devices. The main concern was whether or not the traditional fixed-layout pdf contains enough information to be safely and unambiguously interpreted as html - therefore responsive and reusable in different environments.
The output of the work is the Derivation algorithm - document that describes how the process of conversion could be done.
As a part of the work we came up with referential set of pdf documents and implementation. These should provide enough insights into the whole concept.

We presented about the idea at the [PDF Days Europe 2017 in Berlin](https://www.youtube.com/watch?v=cFr7SI8pMZk)

#Download
This repository contains a commandline tool and GUI application. You can find latest version through [release link] (https://github.com/Normex/PDF-Derivation/releases) 
Choose your prefered version (32bit or 64 bit), download the zip file and unpack to your prefered location

##Command line application

To run commandline application just type:

```batch
derive.exe c:\temp\tagged.pdf c:\temp\output
``` 
first parameter is pdf file you want to derive into html
second parameter is existing output folder.

##GUI application

We utilise chromium to show result html alongside with original pdf. The use of the gui app is pretty straightforward. Just open pdf file and check html tag for result. HTML file is stored in application subfolder

#Sample files

Even though the derivation algorithm is designed to work properly for all tagged pdf files, it is expected that the best result in terms of visual accuracy is achieved with pdf files that are tagged for this purpose. In the [pdf folder] (https://github.com/Normex/PDF-Derivation/blob/master/pdf) you can find set of examples (manually crafted pdf files) that show how specific structure elements, attributes, associated files are used during the derivation and how the author can turn static pdfs into a dynamic html. 

## Feedback

Feel free to submit bugs, problematic files, comments, questions, suggestions by creating new issue in [Issues section](https://github.com/Normex/PDF-Derivation/issues) or [e-mail us] (mailto:pdf-derivation@digitaldocuments.org) 

# Licensing

Applications are only for study and reference purpose.
We use 3rd party libraries that might be subject of licence fees, therefore we are not providing source code. 
You may use output html files produced by our applications, but you are not allowed to change the metadata included in output html.
If you are interested in using this work in commercial environment [e-mail us] (mailto:pdf-derivation@digitaldocuments.org)




