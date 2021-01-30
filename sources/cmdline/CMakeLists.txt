cmake_minimum_required (VERSION 3.10)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
# set (CMAKE_REQUIRED_LIBRARIES stdc++fs)
set(CLANG38_CXX_WARNING_FLAGS "-Winvalid-source-encoding")


project(PDFDerivation)

set(SOURCES 
	src/tagged_utils.cpp
	src/tagged_pdf_caption.cpp
	src/tagged_pdf_classmap.cpp
	src/tagged_pdf_listitem.cpp
	src/tagged_pdf_utils.cpp
	src/tagged_pdf_html_ns.cpp
	src/tagged_pdf_form.cpp
	src/derived_output_writer.cpp
	src/tagged_pdf_heading.cpp
	src/tagged_pdf_link.cpp
	src/tagged_pdf_attributes.cpp
	src/tagged_pdf_attr_dict.cpp
	src/tagged_pdf_figure.cpp
	src/tagged_pdf_element_factory.cpp
	src/tagged_pdf_html_doc.cpp
	src/tagged_pdf_list.cpp
	src/tagged_pdf_element.cpp
	src/tagged_pdf_label.cpp
	src/tagged_pdf_lbody.cpp
	)
# 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if (WIN32)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
            ${CMAKE_SOURCE_DIR}/../../bin/x86)
    else()
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
            ${CMAKE_SOURCE_DIR}/../../bin/x64)
    endif()
elseif(APPLE)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
        ${CMAKE_SOURCE_DIR}/../../bin/darwin) 
elseif(UNIX)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
        ${CMAKE_SOURCE_DIR}/../../bin/linux) 
endif()

# add_subdirectory(src)

include_directories(
	include
	../include
   )

add_executable(PDFDerivation 
	src/Main.cpp
	${SOURCES}
	)

if (UNIX)
	target_link_libraries(PDFDerivation dl)
endif()




