## Deriving HTML from PDF 

Implementation of the [Derivation algorithm](https://www.pdfa.org/resource/deriving-html-from-pdf/) that converts well-tagged pdf files into HTML

## Sample files

Even though the derivation algorithm is designed to work properly for all tagged pdf files, it is expected that the best result in terms of visual accuracy is achieved with pdf files that are tagged for this purpose.

Following are few files that represent well-tagged pdf files from tagging perspective. These are for discussion of WT-PDF development as a representatives of the reuse concept:

### [table_with_attributes.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/table_with_attributes.pdf) [HTML](https://normex.github.io/PDF-Derivation/table_with_attributes/index.html)
Tabular information in two different forms. Standard Layout attributes used to preserve layout information like color or styling in the table. 

### [af_css_table.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/af_css_table.pdf) [HTML](https://normex.github.io/PDF-Derivation/af_css_table/index.html)
Each table cell refers to it's headers through ID which is preserved in the html. Pdf and html would be consumed by screen reader in the same way
the same file as table_complex_with_headerIDs.pdf. Extensive styling is achieved with associated CSS file attached to the Document structure element. The css file is used by result html.

### [af_css_js_sort_table.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/af_css_js_sort_table.pdf) [HTML](https://normex.github.io/PDF-Derivation/af_css_js_sort_table/index.html)
Example of interactive table with associated files. The css file is used to style "sortable" class which is assigned to table strucure element via "C" entry. The associated javascript file allows sorting of table by clicking on a table header

### [BMIBAI_form_css.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/BMIBAI_form_css.pdf) [HTML](https://normex.github.io/PDF-Derivation/BMIBAI_form_css/index.html)
Acroform fields with various javascript actions. Changing units recalculates values. Calculates BMI and BAI and allows submitting data into shared google spreadsheet. The same functionality works in pdf and derived html. Styling is optimized for mobile/small devices and is achieved by attaching an associated file of type CSS on the Document structure element. Classes are referenced through "C" entry in structure element dictionary.


In the [pdf folder](https://github.com/Normex/PDF-Derivation/blob/master/pdf/README.md) you can find full set of examples (manually crafted pdf files) that show how specific structure elements, attributes, associated files are used during the derivation and how the author can turn static pdfs into a dynamic html. 


## Feedback

Feel free to submit bugs, problematic files, comments, questions, suggestions by creating new issue in [Issues section](https://github.com/Normex/PDF-Derivation/issues) or [e-mail us](mailto:pdf-derivation@digitaldocuments.org) 

