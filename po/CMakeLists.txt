add_subdirectory(de)
add_subdirectory(fi)
add_subdirectory(pl)
add_subdirectory(pt_BR)
add_subdirectory(ru)


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