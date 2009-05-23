PROJECT(Escher)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O0 -g)

#list all source files here
ADD_LIBRARY(libarikkei STATIC
arikkei-cache.c  arikkei-dict.h   arikkei-strlib.c  arikkei-token.h  dict.h             token.cpp
arikkei-cache.h  arikkei-iolib.c  arikkei-strlib.h  cache.h token.h
arikkei-dict.c   arikkei-iolib.h  arikkei-token.c utext.h)


INCLUDE_DIRECTORIES(..)

