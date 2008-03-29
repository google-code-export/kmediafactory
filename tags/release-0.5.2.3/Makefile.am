SUBDIRS = $(TOPSUBDIRS)

$(top_srcdir)/configure.in: configure.in.in $(top_srcdir)/subdirs
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in ;

$(top_srcdir)/subdirs:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files

package-messages:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge

EXTRA_DIST = admin COPYING configure.in.in README AUTHORS CREDITS NEWS ChangeLog

# KDE_DIST adds too much files. Is there a better way to do this than rm here?
dist-hook:
	cd $(distdir) && perl admin/am_edit -padmin
	cd $(distdir) && $(MAKE) -f admin/Makefile.common subdirs
	cd $(distdir) && rm -f links.sh dist.sh snapshot.sh tags kmediafactory.kdevelop.pcs
	cd $(distdir) && rm -fR admin/CVS
	cd $(distdir) && rm -fR admin/.arch-ids
	cd $(distdir) && rm -fR admin/.svn

dist-bz2: distdir
	$(AMTAR) chof - $(distdir) | bzip2 -9 -c >$(distdir).tar.bz2
	$(am__remove_distdir)

