# PDF files

Even though the derivation algorithm is designed to work properly for all tagged pdf files, it is expected that the best result in terms of visual accuracy is achieved with pdf files that are tagged for this purpose. In the [pdf folder](https://github.com/Normex/PDF-Derivation/blob/master/pdf) you can find set of examples (manually crafted pdf files) that show how specific structure elements, attributes, associated files are used during the derivation and how the author can turn static pdfs into a dynamic html. 
We are also providing samples of tagged pdf files that don't perform well during derivation. The reason is highlighted in each sample, but usually we see sloppy tagging, wrong semantics, not enough information like missing attributes, not following best practices.

## [PAC_Report.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/PAC_Report.pdf)
Standard PDF/UA-1 file with no attributes or classmap. The way lists are used in pdf without additional styling makes html output look little different

## [BMIBAI_form.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/BMIBAI_form.pdf)
Acroform fields with various javascript actions. Changing units recalculates values. Calculates BMI and BAI and allows submitting data into shared google spreadsheet. The same functionality works in pdf and derived html. No specific styling is applied on structure elements to keep output as simple as possible 

## [wikipedia_html_NS_no_styling.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/wikipedia_html_NS_no_styling.pdf)
File created by converting html file to pdf with capturing original html structure. Each structure element type is in HTML namespace which is PDF 2.0 feature. The derivation is pretty straightforward. This sample doesn't contain any information about styling from original html. That makes the derived html look differently from layout perspective. 

## [Animals_in_research_attributes.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/Animals_in_research_attributes.pdf)
Standard Layout attributes used to preserve layout information like color or styling in the table. 

## [table_with_attributes.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/table_with_attributes.pdf)
Standard Layout attributes used to preserve layout information like color or styling in the table. 

## [table_complex_with_headerIDs.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/table_complex_with_headerIDs.pdf)
Each table cell refers to it's headers through ID which is preserved in the html. Pdf and html would be consumed by screen reader in the same way

## [Letter_DOC.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/Letter_DOC.pdf)
Standard Microsoft Word template saved as PDF in MS Word 2016. Microsoft Word doesn't convert any styling information into attributes. The layout isn't preserved either.

## [Invoice_XLS.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/Invoice_XLS.pdf)
Standard Microsoft Excel invoice template saved as PDF in MS Excel 2016. Microsoft Excel doesn't convert any styling information into attributes.


# Samples we are working on
- more complex styling (css in form of associated file or on attributes)
- structure destination used for better navigation inside of the document
- used AF to provide additional information (mathml, css styling etc.)

# Feedback
Feel free to submit bugs, problematic files, comments, questions, suggestions by creating new issue in [Issues section](https://github.com/Normex/PDF-Derivation/issues) or [e-mail us](mailto:pdf-derivation@digitaldocuments.org) 

