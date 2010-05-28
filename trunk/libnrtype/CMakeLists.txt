PROJECT(libnrtype)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

include(FindPkgConfig)
INCLUDE(FindJPEG)
pkg_check_modules(FT2 REQUIRED freetype2)
pkg_check_modules(XFT REQUIRED xft)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O0 -g ${FT2_CFLAGS} ${XFT_CFLAGS})

#list all source files here
ADD_LIBRARY(libnrtype STATIC
nr-font.c nr-font.h
nr-glyphs.c nr-glyphs.h
nr-rasterfont.c nr-rasterfont.h
nr-type-directory.c nr-type-directory.h
nr-typeface.c nr-typeface.h
nr-type-ft2.c nr-type-ft2.h
nr-type-primitives.c nr-type-primitives.h
nr-type-xft.c nr-type-xft.h)

INCLUDE_DIRECTORIES(..)
