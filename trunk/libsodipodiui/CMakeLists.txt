cmake_minimum_required(VERSION 2.6)

#INCLUDE(FindGTK)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

EXECUTE_PROCESS(COMMAND pkg-config --cflags gtk+-2.0 OUTPUT_VARIABLE GTK_CFLAGS)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(${GTK_CFLAGS})

#list all source files here
ADD_LIBRARY(libsodipodiui STATIC
	action.h action.c
	button.h button.c
	color.h color.c
	icon.h icon.c
	interface.h interface.c
	shortcuts.h shortcuts.c
	sp-color-preview.h sp-color-preview.c
	sp-color-selector.h sp-color-selector.c
	sp-color-slider.h sp-color-slider.c
	sp-marshal.h sp-marshal.c)

INCLUDE_DIRECTORIES(..)

