cmake_minimum_required(VERSION 3.14)

project(VdpHelper
	VERSION 0.1
	DESCRIPTION "GUI tool for decoding Sega Mega Drive VDP instructions."
	LANGUAGES CXX
)

set(VDPHELPER_TARGET vdp-helper)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
find_package(Qt5 5.13 COMPONENTS Widgets REQUIRED)

set(VDPHELPER_SOURCES
	src/main.cpp
	src/mainwindow.cpp
	src/mainwindow.h
)

set(VDPHELPER_FORMS
	ui/mainwindow.ui
	ui/ramhelper.ui
)

add_executable(${VDPHELPER_TARGET}
    ${VDPHELPER_SOURCES}
    ${VDPHELPER_FORMS}
)

target_compile_definitions(${VDPHELPER_TARGET} PRIVATE
	_CRT_SECURE_NO_WARNINGS
)

target_link_libraries(${VDPHELPER_TARGET} PRIVATE
	Qt5::Widgets
)

target_include_directories(${VDPHELPER_TARGET} PUBLIC
	${CMAKE_SOURCE_DIR}/src
) 

target_compile_options(${VDPHELPER_TARGET} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W3>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

set_property(TARGET ${VDPHELPER_TARGET} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${VDPHELPER_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)

add_custom_command(
    TARGET ${VDPHELPER_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt5::Core> $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE:Qt5::Widgets>
        $<TARGET_FILE_DIR:${VDPHELPER_TARGET}>
)

install(TARGETS ${VDPHELPER_TARGET} DESTINATION bin)
