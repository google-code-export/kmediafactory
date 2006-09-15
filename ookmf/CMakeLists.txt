INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

INSTALL_FILES( /share/apps/kmediafactory FILES kmf_converter.zip )

KDE_PLACEHOLDER()



#original Makefile.am contents follow:

#SUBDIRS =
#CLEANFILES = kmf_converter.zip
#kmediafactorydir = $(kde_datadir)/kmediafactory
#kmediafactory_DATA = kmf_converter.zip
#
#ookmf_files = converter.xba\nscript.xlb
#
#kmf_converter.zip:
#	export builddir=`pwd` && cd $(srcdir) && \
#	echo -e "$(ookmf_files)" | $(ZIP) -X $$builddir/kmf_converter.zip -@
#
#install-data-hook:
#if HAVE_UNOPKG
#	$(UNOPKG) add --force --shared kmf_converter.zip
#endif
