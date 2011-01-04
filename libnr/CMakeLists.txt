PROJECT(Escher)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
#ADD_DEFINITIONS(-Wall -O2)

#list all source files here
ADD_LIBRARY(libnr STATIC
	nr-gradient.c nr-gradient.h
	nr-image.c nr-image.h
	nr-macros.h
	nr-path.h              
	nr-pathops.c nr-pathops.h
	nr-pixblock-compose.c nr-pixblock-compose.h
	nr-rect.c nr-rect.h
	nr-render.c nr-render.h       
nr-types.h
nr-uta.c
nr-uta.h
nr-matrix.c                                          
nr-pixblock.c          
nr-stroke.c       
nr-values.c
nr-blit.c               
nr-matrix.h                                          
nr-pixblock.h          
nr-stroke.h       
nr-values.h
nr-blit.h               
nr-pixblock-line.c     
nr-svp.c          
pixblock.h
nr-compose.c            
nr-pixblock-line.h     
nr-svp.h          
rakugaki.h
nr-compose.h            
nr-pixblock-pattern.c  
nr-svp-private.h  
nr-compose-transform.c  
nr-pixblock-pattern.h  
nr-svp-render.c   
nr-compose-transform.h  
nr-object.c                                          
nr-pixblock-pixel.c    
nr-svp-render.h   
nr_config.h.win32       
nr-object.h                                          
nr-pixblock-pixel.h    
nr-svp-uncross.c
nr-forward.h            
nr-path.c                                            
nr-pixops.h            
nr-svp-uncross.h)

INCLUDE_DIRECTORIES(..)

