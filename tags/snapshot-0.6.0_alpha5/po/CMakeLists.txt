MESSAGE(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(AUTODIRS)")
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


KDE_PLACEHOLDER()



#original Makefile.am contents follow:

#SUBDIRS = $(AUTODIRS)
#
#message-merge:
#	@for f in *; do \
#		if test -d "$$f"; then \
#			for pot in *.pot; do \
#				msgmerge -U "$$f/$${pot%*.*}.po" "$$pot"; \
#			done \
#		fi \
#	done