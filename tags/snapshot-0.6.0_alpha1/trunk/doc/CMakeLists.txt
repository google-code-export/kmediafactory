MESSAGE(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(AUTODIRS)")
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


KDE_PLACEHOLDER()



#original Makefile.am contents follow:

## the SUBDIRS is filled automatically by am_edit. If files are 
## in this directory they are installed into the english dir
#
#KDE_LANG = en
#KDE_DOCS = kmediafactory
#SUBDIRS = $(AUTODIRS)  
