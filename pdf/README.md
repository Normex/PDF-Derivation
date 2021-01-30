# PDF files

Even though the derivation algorithm is designed to work properly for all tagged pdf files, it is expected that the best result in terms of visual accuracy is achieved with pdf files that are tagged for this purpose. In the [pdf folder](https://github.com/Normex/PDF-Derivation/blob/master/pdf) you can find set of examples (manually crafted pdf files) that show how specific structure elements, attributes, associated files are used during the derivation and how the author can turn static pdfs into a dynamic html. 
We are also providing samples of tagged pdf files that don't perform well during derivation. The reason is highlighted in each sample, but usually we see sloppy tagging, wrong semantics, not enough information like missing attributes, not following best practices.

## [PAC_Report.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/PAC_Report.pdf)
Standard PDF/UA-1 file with no attributes nor classmap. The way lists are used in pdf without additional styling makes html output look little different

## [PAC_Report_style_classmap.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/PAC_Report.pdf)
Styling of derived html is achieved by introducing CSS attributes in classmap. Structure elements are associated with those classes of attributes through "C" entry. Derivation respects this and generated css style from classmap and then refers to it through class attribute.

## [BMIBAI_form.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/BMIBAI_form.pdf)
Acroform fields with various javascript actions. Changing units recalculates values. Calculates BMI and BAI and allows submitting data into shared google spreadsheet. The same functionality works in pdf and derived html. No specific styling is applied on structure elements to keep output as simple as possible 

## [BMIBAI_form_css.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/BMIBAI_form_css.pdf)
Same as BMIBAI_form.pdf file. Styling is optimized for mobile/small devices and is achieved by attaching an associated file of type CSS on the Document structure element. Classes are referenced through "C" entry in structure element dictionary.

## [wikipedia_html_NS_no_styling.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/wikipedia_html_NS_no_styling.pdf)
File created by converting html file to pdf with capturing original html structure. Each structure element type is in HTML namespace which is PDF 2.0 feature. The derivation is pretty straightforward. This sample doesn't contain any information about styling from original html. That makes the derived html look differently from layout perspective. 

## [Animals_in_research_attributes.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/Animals_in_research_attributes.pdf)
Standard Layout attributes used to preserve layout information like color or styling in the table. 

## [table_with_attributes.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/table_with_attributes.pdf)
Standard Layout attributes used to preserve layout information like color or styling in the table. 

## [table_complex_with_headerIDs.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/table_complex_with_headerIDs.pdf)
Each table cell refers to it's headers through ID which is preserved in the html. Pdf and html would be consumed by screen reader in the same way

## [af_css_table.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/af_css_table.pdf)
the same file as table_complex_with_headerIDs.pdf. Extensive styling is achieved with associated CSS file attached to the Document structure element. The css file is used by result html.

## [af_css_js_sort_table.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/af_css_js_sort_table.pdf)
Example of interactive table with associated files. The css file is used to style "sortable" class which is assigned to table strucure element via "C" entry. The associated javascript file allows sorting of table by clicking on a table header

## [af_url_css_table.pdf](https://github.com/Normex/PDF-Derivation/blob/master/pdf/af_url_css_table.pdf)
Document structure element references external css style defined by url:https://www.w3schools.com/w3css/4/w3.css Table structrure element is then associated with class through "C" entry. Referencing external objects might be considered as unsafe and could introduce some unpredictability in the output.

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

