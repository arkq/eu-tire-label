# EU-tire-label - CMakeLists.txt
# Copyright (c) 2015-2021 Arkadiusz Bokowy

cmake_minimum_required(VERSION 3.2)
project(EU-tire-label
	VERSION 2.0.0
	LANGUAGES C)

option(ENABLE_CGI "Enable Common Gateway Interface (CGI) support." OFF)
option(ENABLE_PNG "Enable SVG rasterisation support (PNG output)." OFF)

if(ENABLE_PNG)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(rSVG REQUIRED IMPORTED_TARGET librsvg-2.0)
endif()

add_executable(label2array
	${CMAKE_CURRENT_SOURCE_DIR}/src/label2array.c)

set(SOURCE_LABEL_EC_1222_2009 ${CMAKE_CURRENT_SOURCE_DIR}/src/label-EC-1222-2009.svg)
set(GENERATED_LABEL_EC_1222_2009 ${CMAKE_CURRENT_BINARY_DIR}/label_EC_1222_2009-template.h)
add_custom_command(
	DEPENDS label2array ${SOURCE_LABEL_EC_1222_2009}
	COMMAND label2array label_EC_1222_2009_template ${SOURCE_LABEL_EC_1222_2009} > ${GENERATED_LABEL_EC_1222_2009}
	OUTPUT ${GENERATED_LABEL_EC_1222_2009})

set(SOURCE_LABEL_EU_2020_740 ${CMAKE_CURRENT_SOURCE_DIR}/src/label-EU-2020-740.svg)
set(GENERATED_LABEL_EU_2020_740 ${CMAKE_CURRENT_BINARY_DIR}/label_EU_2020_740-template.h)
add_custom_command(
	DEPENDS label2array ${SOURCE_LABEL_EU_2020_740}
	COMMAND label2array label_EU_2020_740_template ${SOURCE_LABEL_EU_2020_740} > ${GENERATED_LABEL_EU_2020_740}
	OUTPUT ${GENERATED_LABEL_EU_2020_740})

set(DOWNLOADED_QRCODE_C_PATH ${CMAKE_CURRENT_BINARY_DIR}/qrcode.c)
set(DOWNLOADED_QRCODE_H_PATH ${CMAKE_CURRENT_BINARY_DIR}/qrcode.h)
if (NOT EXISTS ${DOWNLOADED_QRCODE_C_PATH})
	message(STATUS "Downloading QRCode library")
	file(DOWNLOAD https://github.com/ricmoo/QRCode/raw/master/src/qrcode.c ${DOWNLOADED_QRCODE_C_PATH})
	file(DOWNLOAD https://github.com/ricmoo/QRCode/raw/master/src/qrcode.h ${DOWNLOADED_QRCODE_H_PATH})
	message(STATUS "Downloading QRCode library - done")
endif()

add_executable(eu-tire-label
	${GENERATED_LABEL_EC_1222_2009}
	${GENERATED_LABEL_EU_2020_740}
	${DOWNLOADED_QRCODE_C_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/src/label.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

target_include_directories(eu-tire-label
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_compile_definitions(eu-tire-label
	PRIVATE -DVERSION="${PROJECT_VERSION}")

if(ENABLE_CGI)
	target_compile_definitions(eu-tire-label PRIVATE -DENABLE_CGI=1)
endif()

if(ENABLE_PNG)
	target_compile_definitions(eu-tire-label PRIVATE -DENABLE_PNG=1)
	target_sources(eu-tire-label PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/raster.c)
	target_link_libraries(eu-tire-label PkgConfig::rSVG)
endif()

install(TARGETS eu-tire-label
	RUNTIME DESTINATION bin)
