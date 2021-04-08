# EU-tire-label - CMakeLists.txt
# Copyright (c) 2015-2021 Arkadiusz Bokowy

cmake_minimum_required(VERSION 3.2)
project(EU-tire-label
	VERSION 1.0.0
	LANGUAGES C)

option(ENABLE_CGI "Enable Common Gateway Interface (CGI) support." OFF)
option(ENABLE_PNG "Enable SVG rasterisation support (PNG output)." OFF)

if(ENABLE_PNG)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(rSVG REQUIRED IMPORTED_TARGET librsvg-2.0)
endif()

add_executable(label2array
	${CMAKE_CURRENT_SOURCE_DIR}/src/label2array.c)
add_custom_command(
	DEPENDS label2array ${CMAKE_CURRENT_SOURCE_DIR}/src/label.svg
	COMMAND label2array label_template ${CMAKE_CURRENT_SOURCE_DIR}/src/label.svg > label-template.h
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/label-template.h)

add_executable(eu-tire-label
	${CMAKE_CURRENT_BINARY_DIR}/label-template.h
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
