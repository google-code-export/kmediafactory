MESSAGE(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(AUTODIRS)")
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


KDE_PLACEHOLDER()



#original Makefile.am contents follow:

#KDE_LANG = de
#SUBDIRS  = $(AUTODIRS)
#POFILES  = AUTO
#CLEANFILES = *.gmo