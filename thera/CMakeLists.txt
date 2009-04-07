PROJECT(Escher)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -g -O0)

#list all source files here
ADD_LIBRARY(thera STATIC
	thera.h thera.cpp io.cpp
	thera-document.h thera-node.h
	transaction.h transaction.cpp)

INCLUDE_DIRECTORIES(/usr/include/libxml2 ..)


